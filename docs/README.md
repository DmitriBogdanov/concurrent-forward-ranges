> [!Warning]
> Current README is an **expected** specification and architectural guide, not an accurate representation of the actual feature set.

# concurrent-forward-ranges

**CFR** is a library of concurrent range algorithms that implements most of C++20 [`<algorithm>`](https://en.cppreference.com/cpp/header/algorithm) and a range-based version of [`<numeric>`](https://en.cppreference.com/cpp/header/numeric).

The core feature of this library is its first class support of various [parallelization strategies](#motivation) and explicit control over the execution. This makes it distinctly different from the parallel range algorithms standardized in C++26 (based on [P3179R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p3179r5.html)).

Internally the library relies on OneTBB as its parallelism backend due to its wide industry adoption, in many codebases TBB is a de-facto standard way of concurrent execution with special tools developed specifically for its threads, ranges and task arenas.

> [!Tip]
> Usage examples can be found [below](#basic-examples). Full API documentation can be found the [website]().

## Motivation

Existing parallel algorithm implementations are usually limited to nothing but random access iterators, this is a very limiting requirement that makes them incompatible with a wide range of useful data structures. In reality a lot of these data structures do very much support efficient parallelization, we just need to tell the algorithms how to subdivide their data correctly:

| Data structures | Efficient subdivision |
| - | - |
| Octrees, R-Trees, KD-Trees | Recursive |
| Spatial hashes | Bucket-based |
| Unordered flat maps | Segmented |
| Sparse index arenas & ECSs | Fast midpoint |
| Lists & Sparse views | Linear chunking |

C++20 ranges give us a perfect mechanism to do exactly that, using ranges themselves as a customization point for work subdivision (approach inspired by [TBB]() splittable objects) we can achieve a practically 1-to-1 compatibility with regular algorithm syntax while enabling their concurrent use with a whole bunch of additional data structures. 

Fundamentally, range is an **inherently more powerful abstraction** than just a pair of iterators, a range can carry its:

- Size
- Grain
- Strategy
- Payload

For example, parallel algorithms highly benefit from the fact that most ranges in the wild have an `O( 1 )` size, even through their iterator pair might not support `O( 1 )` difference, knowing the size we can better select automatic grain size and perform initial subdivision.

Additionally, lazy evaluation and zero-copy views into data allows us to run parallel algorithms like a sort of "execution pipeline" without having to assemble the data into its own data structure beforehand (like it's frequently done to work around the iterator limitations).

## Design goals

### API goals

- All algorithms (and related functionality) should support `constexpr` evaluation
- Library API should follow conventions and style of the standard library
- TBB usage should be easily compatible with the library, but not leak into its core API
- Potentially slow operations should not happen implicitly, there must be an explicit opt-in

### Quality goals

- Performance characteristics should be confirmed though benchmark coverage
- Algorithms should have proper test coverage and pass basic fuzzing
- API reference should fully cover the functionality and provide the examples

### Differences relative to `std`

- Output iterators are replaced with output ranges to enable bounded writing
- Reductions and scans use identity values instead of init values for better concurrency
- Algorithm functions and projections should be thread-safe to invoke

## Parallelizable ranges

### Range concepts

Most `cfr` algorithms can operate on 4 kinds of ranges:

1. [`std::ranges::random_access_range`](https://en.cppreference.com/cpp/ranges/random_access_range)
2. [`cfr::ranges::divisible_range`]()
3. [`cfr::ranges::chunkable_range`]()
4. [`cfr::ranges::tbb_compatible_range`](https://oneapi-spec.uxlfoundation.org/specifications/oneapi/latest/elements/onetbb/source/named_requirements/algorithms/range)

Additionally, all ranges are required to be bounded and forward-iterable to allow multiple passes.

Any range that doesn't belong to one of these categories (aka any general range like `std::list` or `boost::unordered_flat_map`) can be automatically adapted using `cfr::views::adapt( range )`, while this helper cannot know the truly "optimal" way to split its underlying container, in most cases we can still parallelize decently well with its built-in general strategies.

### Customizing grain

Grain size can be passed as an optional second parameter to `cfr::views::adapt( range, grain )`.

By default, reasonable grain size is estimated based on the range size and current concurrency level. In case `range` doesn't support `O( 1 )` size computation, the library will prohibit this overload and require that user provides grain size explicitly to avoid accidental `O( N )` evaluation. If automatic grain is desired regardless it can be provided explicitly using `cfr::views::adapt( range, cfr::ranges::automatic_grain( range ) )`.

In case given range is already adapted to subdivide in a certain way, the range will be forwarded and the grain argument will be ignored.

### Customizing thread count

Concurrency level can be set locally for the code block:

```cpp
cfr::invoke_with_concurrency( thread_count, [&]{
    cfr::ranges::for_each( range, []( auto && x ) { /* ... */ } );
} );
```

Internally, `cfr` algorithms operate with respect to TBB task arenas, so the snippet above is effective same as:

```cpp
tbb::task_arena{ thread_count }.run( [&]{
    cfr::ranges::for_each( range, []( auto && x ) { /* ... */ } );
} );
```

### Customizing subdivision strategy

#### Divisible ranges

Divisible ranges split through recursive subdivision.

This is a default strategy used by TBB ranges. It is generally preferable to linear chunking since assuming `O( 1 )` splitting we can guarantee worst case complexity of no more than `O( log( N ) )` operations to subdivide the entire range down to its smallest (single-element) pieces.

To satisfy `cfr::ranges::divisible_range<R>` concept a range needs to provide following API:

```cpp
struct custom_divisible_range {
    
    // Must be a range
    std::forward_iterator auto begin() { /* ... */ }
    std::forward_iterator auto   end() { /* ... */ }
    
    // Must be able to split recursively
    custom_divisible_range split() { /* ... */ }
    
    bool is_divisible() const { /* ... */ }
    
};
```

To adapt arbitrary range explicitly into a divisible one use `cfr::views::divisible( range, grain )`.

Note that doing so for ranges without random access is generally ill-advised as those cases are better served by a similar linear chunking adaptor.

#### Chunkable ranges

Chunkable ranges split through linear subdivision.

Linear chunking strategy allows decently efficient parallelization of ranges without random access or quick iterator spacing. 

To satisfy `cfr::ranges::chunkable_range<R>` concept a range needs to provide following API:

```cpp
struct custom_chunkable_range {
    
    // Must be a range
    std::forward_iterator auto begin() { /* ... */ }
    std::forward_iterator auto   end() { /* ... */ }
    
    // Must be able to chunk linearly
    std::ranges::forward_range auto chunk() { /* ... */ }
    
    bool is_chunkable() const { /* ... */ }
    
};
```

To adapt arbitrary range explicitly into a chunkable one use `cfr::views::chunkable( range, grain )`.

Note that return type of `chunk()` doesn't have to match the original range. We can use it to efficiently chunk hierarchical data structures such as segmented arrays (like `std::deque`) or bucket-based containers (like `std::unordered_map`) by returning subranges with cheap local iterators, e.g. in case of segmented arrays local chunks may be returned as a simple `std::span`. Depending on the data structure, using hierarchical iteration might lead to as much as a [2x-3x boost in performance](https://boostedcpp.net/2026/05/18/neoclassical-c-segmented-iterators-revisited-1/) compared to a regular "flattened" approach.

Worst-case scenario cost of parallelization when using `cfr::views::chunkable` adaptor is an additional `O( N )` iteration pass (so we can distribute initial tasks). Assuming the workload per iteration is higher than the cost of iteration itself (which is usually the case) we can still get a decently large benefit from parallelization even when working with exceedingly "bad" ranges such as `std::list`.

### Advanced details

A range can be both chunkable and divisible at the same time, in that case it's up to the algorithms to decide which strategy they prefer.

Additionally, ranges can provide an optional method for querying `grain_size()`, some algorithms can make use of it to improve their internal heuristics.

> [!Tip]
> When implementing custom parallel ranges, inherit [`std::ranges::view_interface<>`]() CRTP to generate all the range boilerplate based on `begin()` / `end()`. 

> [!Tip]
> If your range is non-owning view, specialize [`std::ranges::enable_borrowed_range<>`]() to make it compatible with standard borrowed iterator safeguards. 

## Basic Examples

### Parallel for each

```cpp
const auto range = vector{ 0, 1, 2, 3, 4, 5 };

cfr::ranges::for_each( range, []( auto & x ) { x += 1; } );
```

### Find max element by a member

```cpp
struct vec3 { double x, y, z };

const auto range = std::vector<vec3>{ { 2., 3., 4. }, { 5., 7., 6. } };

const auto max = cfr::ranges::max( range, std::less{}, &vec3::x );
```

### Enumerate range from 0 to N

```cpp
std::vector<std::size_t> range( 100 );

cfr::ranges::iota( range );
```

### Verify predicate

```cpp
const auto range = std::unordered_map<std::string, int>{ { "lorem", 10 }, { "ipsum", 20 } };

const auto predicate = []( int x ) { return x > 0; };

const auto positive = cfr::ranges::all_of( cfr::views::adapt( range ), predicate, cfr::projections::second );
```

### Dot product

```cpp
const auto lhs = std::vector{ 1., 3., 5., 7. };
const auto rhs = std::vector{ 4., 7., 8., 2. };

const auto dot = cfr::ranges::dot( lhs, rhs );
```

### Transform-reduce

```cpp
const auto range = std::list{ 2., 4., 7., 8., 9. };

const auto projection = []( double x ) { return std::exp( std::cos( x ) ); };

const auto product = cfr::ranges::reduce( cfr::views::adapt( range ), 1., std::multiplies{}, projection );
```

### Sort by size

```cpp
auto range = std::vector<std::flat_set<int>>{ { 0, 2 }, { 3, 4, 5, 7 }, { 3, 4 } };

cfr::ranges::stable_sort( range, std::less{}, std::ranges::size );
```

## Advanced examples

### Move `std::unordered_set` into `std::vector`

```cpp
auto source = std::unordered_set<int>( { 0, 1, 2, 3 } );
auto target = std::vector       <int>(  source.size() );

cfr::ranges::move( cfr::views::adapt( source ), target );
```

### Deterministically collect values satisfying a predicate into `std::vector`

```cpp
const auto range = std::views::iota( 0, 800 );

const auto predicate = []( int x ) { return x % 4 == 0; };

const auto filtered = cfr::ranges::slice_deterministic_reduce( range, std::vector<int>{},
    []( auto && local, auto && slice ) {
    	std::ranges::copy_if( slice, predicate, std::back_inserter( local ) );
    },
    []( auto && local, auto && other ) {
        std::ranges::move( other, std::back_inserter( local ) );
    }
);
```

### Sort range of 3D vectors

```cpp
auto range = std::vector<std::array<double, 3>>{ { 6., 7., 8. }, { 3., 4., 5. }, { 2., 3., 4. } };

cfr::ranges::sort( range, std::less{}, std::ranges::lexicographic_compare );
```

### Flatten vector of vectors

```cpp
auto source = std::vector<std::vector<int>>( { { 0, 2 }, { 5, 4, 3, 7 }, { 1, 3, 9 } } );

const auto flattened_size = cfr::ranges::reduce( source, 0uz, std::plus{}, std::ranges::size );

auto target = std::vector<int>( flattened_size );

// Using parallel prefix sum we can compute offsets at which to store in the destination vector

cfr::ranges::scan( source, 0uz, std::plus{}, std::ranges::size, []( auto && prefix, auto && subrange ) {
    std::ranges::move( subrange, std::ranges::next( std::ranges::begin( target ), prefix ) );
} );
```

### Parallelize range of unknown size

```cpp
auto range = std::forward_list{ 0., 1., 2., 3., 4., 5. }; 

// `std::forward_list` doesn't know its own `size()` so we pass the grain size explicitly

cfr::ranges::for_each( cfr::ranges::adapt( range, 1000 ), []( auto & x ) { x = std::riemann_zeta( x ); } );
```

## Building with CMake

### Regular build

The library itself is header only, as such it exposes an `INTERFACE` target, linking is as simple as:

```cmake
add_subdirectory( concurrent-forward-ranges )

target_link_libraries( my_project PRIVATE cfr::lib )
```

TBB dependency is fetched **automatically** using [CMake CPM](https://github.com/cpm-cmake/cpm.cmake), no further action should be required on user side.

### Offline build

For offline builds CPM provides an option to search for the locally installed package instead, simply define

```cmake
set( CPM_USE_LOCAL_PACKAGES ON )
```

And the library will look for local `find_package( TBB )` instead.

### Fetch from the GitHub

To download the library automatically use CMake [`FetchContent()`](https://cmake.org/cmake/help/latest/module/FetchContent.html).

```cmake
include( FetchContent )

FetchContent_Declare(
    CFR
    GIT_REPOSITORY https://github.com/DmitriBogdanov/concurrent-forward-ranges.git
    GIT_TAG        v0.1.0
)

FetchContent_MakeAvailable( CFR )
```

Or, alternatively, do the same with [CMake CPM](https://github.com/cpm-cmake/cpm.cmake):

```cpp
include( cmake/CPM.cmake )

CPMAddPackage( "gh:DmitriBogdanov/concurrent-forward-ranges#v0.1.0" )
```

## License

This project is licensed under the MIT License - see the [LICENSE.md](../LICENSE.md) for details.

# concurrent-forward-ranges

`cfr::ranges` implements concurrent versions of most C++20 range algorithms.

The core feature of this library is its first class support of "bad" iterator types that mostly get ignored by existing concurrency frameworks.

The library relies on OneTBB as its parallelism backend due to its wide industry adoption, in many codebases TBB is a de-facto standard way of concurrent execution with special tools developed specifically for its threads and task arenas.

## Accepted ranges

Most `cfr` algorithms can operate on 3 kinds of ranges:

1) [std::ranges::random_access_range]()
2) [cfr::ranges::divisible_range](https://oneapi-spec.uxlfoundation.org/specifications/oneapi/latest/elements/onetbb/source/named_requirements/algorithms/range)
3) [cfr::ranges::chunkable_range]()

Additionally, all ranges are required to be bounded and forward-iterable to allow multiple passes.

Any range that doesn't belong to one of these categories (aka any general range like `std::list`) can be automatically adapted using `cfr::adapt( range )` helper.

This is a significantly relaxed requirement compared to most parallel STL implementations which tend to ignore parallel execution policies unless given range provides random access. Additionally, even if forward range parallelization is supported, there is usually no way of providing an explicit grain size, thread count and chunking strategy, which are all crucial parts of getting robust performance characteristics outside of trivial examples.

## Design guarantees

- All algorithms can be executed in `constexpr` context
- All algorithms, concepts and helper functions follow the naming convention and style of their standard counterparts
- Output iterators are replaced with output ranges to enable concurrent writing

## Customizing grain

Grain size can be passed as an optional second parameter to `cfr::adapt( range, grain )`.

By default, reasonable grain size is estimated based on the range size and current concurrency level. In case `range` doesn't support `O( 1 )` size computation, the library will prohibit this overload and require that user provides grain size explicitly to avoid accidental `O( N )` evaluation. If automatic grain is desired regardless it can be provided explicitly using `cfr::adapt( range, cfr::ranges::automatic_grain( range ) )`.

In case given range is already adapted to split in a certain way, the grain argument will be ignored.

## Customizing thread count

`cfr` algorithms operate with respect to TBB task arenas, to customize number of threads in a given code segment simply wrap it into a `tbb::task_arena::run()` with desired concurrency level:

```cpp
tbb::task_arena{ thread_count }.run( [&]{
    // Executed with `thread_count`
    cfr::ranges::for_each( range, []( auto && x ) { /* ... */ } );
} );
```

## Customizing chunking strategy

### Recursive splitting

Recursive splitting is a default strategy used by TBB ranges. It is generally preferable to linear chunking since we can guarantee complexity of no more than `O( log( N ) )` operations to subdivide the entire range down to its smallest pieces.

To be recursively splittable a range needs to satisfy [[tbb.req.range]](https://oneapi-spec.uxlfoundation.org/specifications/oneapi/latest/elements/onetbb/source/named_requirements/algorithms/range) requirement, aka it requires following API:

```cpp
struct custom_divisible_range {
    
    // Must be a range
    std::forward_iterator auto begin();
    std::forward_iterator auto   end();
    
    // Must be copyable
    custom_divisible_range( const custom_divisible_range & );
    
    // Must provide splitting constructor
    custom_divisible_range( custom_divisible_range & other, tbb::split );
    
    // Must provide status getters
    bool        empty() const;
    bool is_divisible() const;
    
};
```

To adapt arbitrary range explicitly into a divisible one use `cfr::views::divisible_adaptor( range, grain )`. Note that doing so for ranges without random access is generally ill-advised as those cases are better served by a similar linear chunking adaptor.

### Linear chunking

Linear chunking strategy allows decently efficient parallelization of ranges without random access or quick iterator spacing. 

To be linearly chunkable a range needs to provide following API:

```cpp
struct custom_chunkable_range {
    
    // Must be a range
    std::forward_iterator auto begin() { /* ... */ }
    std::forward_iterator auto   end() { /* ... */ }
    
    // Must be able to chunk linearly
    std::ranges::forward_range auto chunk() { /* ... */ }
    
};
```

Note that return type of `chunk()` doesn't have to match the original range. We can use it to efficiently chunk hierarchical data structures such as segmented arrays (like `std::deque`) or bucket-based containers (like `std::unordered_map`) by returning subranges with cheaper local iterators, e.g. in case of segmented arrays local chunks may be returned as a simple `std::span`.

To adapt arbitrary range explicitly into a chunkable one use `cfr::views::chunkable_adaptor( range, grain )`.

The cost of parallelization when using this general-case adaptor is an additional `O( N )` iteration pass (so we can distribute initial tasks). Assuming the workload per iteration is higher than the cost of iteration itself (which is usually the case) we can get decently large benefit even when working with exceedingly "bad" ranges such as `std::list`.

## Basic Examples

## Advanced examples

### Concurrently move `std::unordered_set` into `std::vector`

```cpp
auto source = std::unordered_set<int>( { 0, 1, 2, 3 } );
auto target = std::vector       <int>(  source.size() );

cfr::ranges::move( source, target );
```

### Concurrently & deterministically collect values satisfying a predicate into `std::vector`

```cpp
const auto range = std::views::iota( 0, 800 );

const auto predicate = []( int x ) { return x % 4 == 0; };

cfr::ranges::slice_deterministic_reduce( range, std::vector<int>{},
    []( auto && local, auto && slice ) {
    	std::ranges::copy_if( slice, predicate, std::back_inserter( local ) );
    },
    []( auto && local, auto && other ) {
        std::ranges::move( other, std::back_inserter( local ) );
    }
);
```

### Concurrently sort range of 3D vectors

```cpp
auto range = std::vector<std::array<double, 3>>{ { 6, 7, 8 }, { 3, 4, 5 }, { 2, 3, 4 } };

cfr::ranges::sort( range, {}, std::ranges::lexicographic_compare );
```

### Concurrently flatten vector of vectors

```cpp
auto source = std::vector<std::vector<int>>( { { 0, 2 }, { 5, 4, 3, 7 }, { 1, 3, 9 } } );

const auto flattened_size = cfr::ranges::reduce( source, 0, std::plus{}, std::ranges::size );

auto target = std::vector<int>( flattened_size );

cfr::ranges::scan( source, 0, std::plus{}, []( auto && prefix, auto && value ) {
    target[prefix] = std::move( value );
}, std::ranges::size );
```

## Installing TBB

OneTBB is quite easy to build from source, after running the install `find_package(TBB)` will become available in CMake with alias target `TBB::tbb`:

**Prerequisites:** [CMake 3.5+](https://cmake.org/)

```bash
# Clone
git clone https://github.com/uxlfoundation/oneTBB.git

# Build
cd oneTBB
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$HOME/.local/tbb -DTBB_TEST=OFF
cmake --build build

# Install
cmake --install build
```

It's also provided by most package managers. For additional details refer to the [official documentation](https://www.intel.com/content/www/us/en/docs/onetbb/get-started-guide/2023-0/install-on-linux.html) or [GitHub repo](https://github.com/uxlfoundation/oneTBB/blob/master/INSTALL.md).

## License

This project is licensed under the MIT License - see the [LICENSE.md](../LICENSE.md) for details.

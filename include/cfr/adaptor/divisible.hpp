// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: View that enables recursive subdivision for an arbitrary range.

#include <iterator> // std::forward_iterator<>, std::sentinel_for<>, std::ranges::next
#include <ranges>   // std::ranges::view_interface<>, std::ranges::subrange<>

#include <cfr/utility/automatic_grain.hpp> // cfr::ranges::automatic_grain
#include <cfr/utility/explicit_size.hpp>   // cfr::ranges::explicit_size

namespace cfr::ranges {

// View over an arbitrary range that makes it `cfr::ranges::divisible_range<>`.
//
// - Single subdivision complexity: O( 1         ) for random access, O( N / 2             ) otherwise
// - Full   subdivision complexity: O( log2( N ) ) for random access, O( N / 2 * log2( N ) ) otherwise
//
// For random access ranges this adaptor is effectively a slightly more modern version
// of `tbb::blocked_range<I>`, making it a "perfect scenario" for parallel algorithms.
//
// In general case it behaves like a best-attempt recursive splitter. Multiple options
// were evaluated and the one with cached size and split in the middle consistently came
// up on top (relatively to other recursive splitters) since caching the size minimizes
// the constant factor in subdivision complexity.
//
// While in general "bad" case chunkable strategy is preferred we still provide an option to use 
// recursive adaptor by specifying it explicitly in order to not over-constrain the general code.
//
template <std::forward_iterator I>
struct divisible_view : std::ranges::view_interface<divisible_view<I>> {
    
    using iterator_type = I;
    using     size_type = std::size_t;
   
protected: 
    
    iterator_type borrowed_beg;
    iterator_type borrowed_end;
    size_type     cached_count;
    size_type     cached_grain;

public:
    
    divisible_view() = delete;

    divisible_view( const divisible_view &  ) = default;
    divisible_view(       divisible_view && ) = default;

    divisible_view & operator=( const divisible_view &  ) = default;
    divisible_view & operator=(       divisible_view && ) = default;

    constexpr divisible_view( iterator_type beg, iterator_type end, size_type count, size_type grain ) :
        borrowed_beg(   beg ),
        borrowed_end(   end ),
        cached_count( count ),
        cached_grain( grain )
    {}

    // Range API
    
    [[nodiscard]] constexpr auto begin() const { return this->borrowed_beg; }
    [[nodiscard]] constexpr auto end  () const { return this->borrowed_end; }
    
    // Subdivision API
    
    [[nodiscard]] constexpr auto split() {
        const size_type beg_offset = size_type( 0 );
        const size_type end_offset = this->cached_count;
        const size_type mid_offset = this->cached_count / size_type( 2 );
        
        const size_type former_count = mid_offset - beg_offset;
        const size_type latter_count = end_offset - mid_offset;
        
        auto mid = std::ranges::next( this->borrowed_beg, former_count );
        auto end =                    this->borrowed_end                ;
        
        this->borrowed_end = mid;
        this->cached_count = former_count;
        
        return divisible_view{ std::move( mid ), std::move( end ), latter_count, this->cached_grain };
        
        // Note: Following TBB convention, new segment splits on the right.
    }
    
    [[nodiscard]] constexpr bool is_divisible() const noexcept { return this->cached_count > this->cached_grain; }
    
    // Additional API
    
    [[nodiscard]] constexpr size_type grainsize() const noexcept { return this->cached_grain; }
};

} // namespace cfr::ranges

namespace cfr::views {
    
struct divisible_fn {
    
    template <class R>
        requires cfr::ranges::sizable_range<R> && std::ranges::sized_range<R>
    [[nodiscard]] constexpr auto operator()( R && range ) const
        -> cfr::ranges::divisible_view<std::ranges::iterator_t<R>>
    {
        return cfr::ranges::divisible_view<std::ranges::iterator_t<R>>{
            std::ranges::begin          ( range ),
            std::ranges::end            ( range ),
            cfr::ranges::explicit_size  ( range ),
            cfr::ranges::automatic_grain( range )
        };
    }
    
    template <class R>
        requires cfr::ranges::sizable_range<R>
    [[nodiscard]] constexpr auto operator()( R && range, std::size_t grain ) const
        -> cfr::ranges::divisible_view<std::ranges::iterator_t<R>>
    {
        return cfr::ranges::divisible_view<std::ranges::iterator_t<R>>{
            std::ranges::begin        ( range ),
            std::ranges::end          ( range ),
            cfr::ranges::explicit_size( range ),
            grain
        };
    }
    
};

inline constexpr auto divisible = divisible_fn{};
    
} // namespace cfr::views

template <class I>
inline constexpr bool std::ranges::enable_borrowed_range<cfr::ranges::divisible_view<I>> = true;

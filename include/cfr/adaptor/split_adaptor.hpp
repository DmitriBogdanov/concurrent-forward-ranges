// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Adaptor view that turns arbitrary range into a recursively splittable range.

#include <iterator> // std::forward_iterator<>, std::sentinel_for<>, std::ranges::next

#include <tbb/blocked_range.h> // tbb::split

#include <cfr/utility/automatic_grain.hpp> // cfr::ranges::automatic_grain
#include <cfr/utility/explicit_size.hpp>   // cfr::ranges::explicit_size

namespace cfr {
    
using split = tbb::split; // tag type for recursively splittable ranges
    
} // namespace cfr

namespace cfr::ranges {

// A view over an arbitrary subrange that makes it TBB-splittable.
//
// - Split constructor complexity: O( 1 ) for random access, O( N / 2 ) otherwise
// - Split check       complexity: O( 1 )
//
// For random access ranges it's effectively the same thing as a regular `tbb::blocked_range<I>`.
//
// For arbitrary forward ranges it's a best-attempt recursive splitter, multiple implementation
// options were evaluated and the version with cached size & split in the middle consistently 
// came up on top (relative to the other recursive splitters, not the linear chunk strategy).
// In the worst case scenario (full subdivision down to the single-element grain) this
// results in { N / 2 * log2( N ) } operations spent on splitting, many workloads can
// still benefit from concurrency despite the additional { log2( N ) } complexity,
// but outside of extremely specific cases chunking adaptor should be preferred.
//
template <std::forward_iterator I, std::sentinel_for<I> S = I>
struct split_adaptor_view {
    
    using iterator_type = I;
    using sentinel_type = S;
    using     size_type = std::size_t;
    
    iterator_type borrowed_beg;
    sentinel_type borrowed_end;
    size_type     cached_count;
    size_type     cached_grain;

    split_adaptor_view() = delete;

    split_adaptor_view( const split_adaptor_view &  ) = default;
    split_adaptor_view(       split_adaptor_view && ) = default;

    split_adaptor_view & operator=( const split_adaptor_view &  ) = default;
    split_adaptor_view & operator=(       split_adaptor_view && ) = default;

    constexpr split_adaptor_view( iterator_type beg, sentinel_type end, size_type count, size_type grain ) :
        borrowed_beg(   beg ),
        borrowed_end(   end ),
        cached_count( count ),
        cached_grain( grain )
    {}

    constexpr split_adaptor_view( split_adaptor_view & other, cfr::split ) {
        const size_type beg_offset = 0;
        const size_type end_offset = other.cached_count;
        const size_type mid_offset = other.cached_count / size_type(2);

        const auto beg = other.borrowed_beg;
        const auto end = other.borrowed_end;
        const auto mid = std::ranges::next( beg, mid_offset );

        other.borrowed_end = mid;
        other.cached_count = mid_offset - beg_offset;

        this->borrowed_beg = mid;
        this->borrowed_end = end;
        this->cached_count = end_offset - mid_offset;
        this->cached_grain = other.cached_grain;
        
        // Note: TBB doesn't dictate whether a new range should split on the right or on the left
        //       and will work correctly in both cases, however it's standard convention is to
        //       always split new subrange on the right for a more cache-friendly iteration order.
    }

    [[nodiscard]] constexpr bool is_divisible() const noexcept { return this->cached_count > this->cached_grain; };

    [[nodiscard]] constexpr size_type  size() const noexcept { return this->cached_count; }
    [[nodiscard]] constexpr size_type grain() const noexcept { return this->cached_grain; }
    
    [[nodiscard]] constexpr bool empty() const noexcept { return not bool(this->cached_count); };

    [[nodiscard]] constexpr iterator_type begin() const { return this->borrowed_beg; };
    [[nodiscard]] constexpr sentinel_type end  () const { return this->borrowed_end; };
    [[nodiscard]] constexpr iterator_type begin()       { return this->borrowed_beg; };
    [[nodiscard]] constexpr sentinel_type end  ()       { return this->borrowed_end; };
};

} // namespace cfr::ranges

namespace cfr::views {
    
struct split_adaptor_fn {
    
    template <class R>
        requires cfr::ranges::sizable_range<R>
    [[nodiscard]] constexpr auto operator()( R && range ) const
        -> cfr::ranges::split_adaptor_view<std::ranges::iterator_t<R>>
    {
        return cfr::ranges::split_adaptor_view<std::ranges::iterator_t<R>>{
            std::ranges::begin          ( range ),
            std::ranges::end            ( range ),
            cfr::ranges::explicit_size  ( range ),
            cfr::ranges::automatic_grain( range )
        };
    }
    
    template <class R>
        requires cfr::ranges::sizable_range<R>
    [[nodiscard]] constexpr auto operator()( R && range, std::size_t grain ) const
        -> cfr::ranges::split_adaptor_view<std::ranges::iterator_t<R>>
    {
        return cfr::ranges::split_adaptor_view<std::ranges::iterator_t<R>>{
            std::ranges::begin        ( range ),
            std::ranges::end          ( range ),
            cfr::ranges::explicit_size( range ),
            grain
        };
    }
    
};

inline constexpr auto split_adaptor = split_adaptor_fn{};
    
} // namespace cfr::views

template <class I, class S>
inline constexpr bool std::ranges::enable_borrowed_range<cfr::ranges::split_adaptor_view<I, S>> = true;

template <class I>
inline constexpr bool std::ranges::enable_borrowed_range<tbb::blocked_range<I>> = true; // TBB <ranges> compat

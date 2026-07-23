// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: View that enables linear subdivision for an arbitrary range.

#include <iterator> // std::forward_iterator<>, std::sentinel_for<>, std::ranges::next
#include <ranges>   // std::ranges::view_interface<>, std::ranges::subrange<>
#include <utility>  // std::exchange()

#include <cfr/concept/bounded_range.hpp> // cfr::ranges::bounded_range<>
#include <cfr/range/automatic_grain.hpp> // cfr::ranges::automatic_grain

namespace cfr::ranges {

// View over an arbitrary range that makes it `cfr::ranges::chunkable_range<>`.
//
// - Single subdivision complexity: O( 1      ) for random access, O( grain ) otherwise
// - Full   subdivision complexity: O( grains ) for random access, O( N     ) otherwise
//
// Linear subdivision is generally less desirable than recursive subdivision,
// for random access ranges or ranges with a fast ~ O( 1 ) midpoint
// recursive strategy should be preferred.
//
// In general case however, linear subdivision provides a better worst case complexity
// and proves rather robust in practical use cases assuming the workload performed is
// non-trivial enough to not be overshadowed by the cost of additional iteration pass.
//
template <std::forward_iterator I, std::sentinel_for<I> S = I>
struct chunkable_view : std::ranges::view_interface<chunkable_view<I, S>> {
    
    using iterator_type = I;
    using sentinel_type = S;
    using     size_type = std::size_t;
    
protected:

    iterator_type borrowed_beg;
    sentinel_type borrowed_end;
    size_type     cached_grain;
    
public:
    
    chunkable_view() = delete;
    
    chunkable_view( const chunkable_view &  ) = default;
    chunkable_view(       chunkable_view && ) = default;
    
    chunkable_view & operator=( const chunkable_view &  ) = default;
    chunkable_view & operator=(       chunkable_view && ) = default;
    
    constexpr chunkable_view( iterator_type beg, sentinel_type end, size_type grain ) :
        borrowed_beg(   beg ),
        borrowed_end(   end ),
        cached_grain( grain )
    {}
    
    // Range API
    
    [[nodiscard]] constexpr auto begin() const { return this->borrowed_beg; }
    [[nodiscard]] constexpr auto end  () const { return this->borrowed_end; }
    
    // Subdivision API
    
    [[nodiscard]] constexpr auto chunk() {
        auto mid = std::ranges::next( this->borrowed_beg, this->cached_grain, this->borrowed_end );
        
        auto beg = std::exchange( this->borrowed_beg, mid );
        
        return std::ranges::subrange{ std::move( beg ), std::move( mid ) };
    }
    
    [[nodiscard]] constexpr bool is_chunkable() const noexcept { return this->borrowed_beg != this->borrowed_end; }
    
    // Additional API
    
    [[nodiscard]] constexpr size_type grain_size() const noexcept { return this->cached_grain; }
};

} // namespace cfr::ranges

namespace cfr::views {

struct chunkable_fn {
    
    template <class R>
        requires cfr::ranges::bounded_range<R> && std::ranges::sized_range<R>
    [[nodiscard]] constexpr auto operator()( R && range ) const
        -> cfr::ranges::chunkable_view<std::ranges::iterator_t<R>>
    {
        return cfr::ranges::chunkable_view<std::ranges::iterator_t<R>>{
            std::ranges::begin          ( range ),
            std::ranges::end            ( range ),
            cfr::ranges::automatic_grain( range )
        };
    }
    
    template <class R>
        requires cfr::ranges::bounded_range<R>
    [[nodiscard]] constexpr auto operator()( R && range, std::size_t grain ) const
        -> cfr::ranges::chunkable_view<std::ranges::iterator_t<R>>
    {
        return cfr::ranges::chunkable_view<std::ranges::iterator_t<R>>{
            std::ranges::begin( range ),
            std::ranges::end  ( range ),
            grain
        };
    }
    
};

inline constexpr auto chunkable = chunkable_fn{};

} // namespace cfr::views

template <class I, class S>
inline constexpr bool std::ranges::enable_borrowed_range<cfr::ranges::chunkable_view<I, S>> = true;

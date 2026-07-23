// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: View that adapts `cfr::ranges::divisible_range<>` into a TBB-compatible range.

#include <ranges> // std::ranges::iterator_t<>
#include <utility> // std::forward()

#include <tbb/blocked_range.h> // tbb::split

#include <cfr/range/grain_size.hpp>            // cfr::ranges::grain_size
#include <cfr/requirement/divisible_range.hpp> // cfr::ranges::divisible_range<>

namespace cfr::ranges {

// Thin wrapper over a divisible range that adapts it to TBB range requirements.
//
// This is a bridge between ranges that are standard-like and uniform with the rest
// of the library and ranges that can actually be consumed by TBB algorithms.
//
// This bridge wouldn't be necessary if we just made divisible ranges TBB-compatible
// by themselves, but we try to avoid allowing TBB quirks to "leak" into the core API,
// TBB should be well-insulated and easy to replace shall we decide to change backends.
//
template <cfr::ranges::divisible_range V>
struct tbb_compatible_view : std::ranges::view_interface<tbb_compatible_view<V>> {
    
    using iterator_type = std::ranges::iterator_t<V>;
    using     size_type = std::size_t;
    
protected:
    
    V range;
    
public:
    
    tbb_compatible_view() = delete;
    
    tbb_compatible_view( const tbb_compatible_view &  ) = default;
    tbb_compatible_view(       tbb_compatible_view && ) = default;
    
    tbb_compatible_view & operator=( const tbb_compatible_view &  ) = default;
    tbb_compatible_view & operator=(       tbb_compatible_view && ) = default;
    
    template <class R>
        requires std::constructible_from<V, R> 
    constexpr tbb_compatible_view( R && range ) : range( std::forward<R>( range ) ) {}
    
    // Range API
    
    [[nodiscard]] constexpr auto begin() const { return std::ranges::begin( this->range ); }
    [[nodiscard]] constexpr auto end  () const { return std::ranges::end  ( this->range ); }
    
    // Subdivision API
    
    constexpr tbb_compatible_view( tbb_compatible_view & other, tbb::split ) : range( other.split() ) {}
    
    [[nodiscard]] constexpr bool is_divisible() const noexcept { return this->range.is_divisible(); }
    
    // Additional API
    
    [[nodiscard]] constexpr size_type grainsize() const noexcept { return cfr::ranges::grain_size( this->range ); }
    
};

} // namespace cfr::ranges

namespace cfr::views {
    
struct tbb_compatible_fn {
    
    template <class R>
    [[nodiscard]] constexpr auto operator()( R && range ) const
        -> cfr::ranges::tbb_compatible_view<std::remove_cvref_t<R>>
    {
        return cfr::ranges::tbb_compatible_view<std::remove_cvref_t<R>>( std::forward<R>( range ) );
    }
    
};

inline constexpr auto tbb_compatible = tbb_compatible_fn{};
    
} // namespace cfr::views

template <class R>
inline constexpr bool std::ranges::enable_borrowed_range<cfr::ranges::tbb_compatible_view<R>> = true;

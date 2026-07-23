// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Top-level niebloid that dispatches range to the most appropriate adaptor view.

#include <cfr/adaptor/chunkable.hpp>                // cfr::views::chunkable
#include <cfr/adaptor/divisible.hpp>                // cfr::views::divisible
#include <cfr/concept/bounded_range.hpp>            // cfr::ranges::bounded_range<>
#include <cfr/concept/indexable_range.hpp>          // cfr::ranges::indexable_range<>
#include <cfr/range/automatic_grain.hpp>            // cfr::ranges::automatic_grain
#include <cfr/requirement/chunkable_range.hpp>      // cfr::ranges::chunkable_range<>
#include <cfr/requirement/divisible_range.hpp>      // cfr::ranges::divisible_range<>
#include <cfr/requirement/tbb_compatible_range.hpp> // cfr::ranges::tbb_compatible_range<>

namespace cfr::views {

struct adapt_fn {
    
    template <class R>
        requires cfr::ranges::bounded_range<R> && std::ranges::sized_range<R>
    [[nodiscard]] constexpr auto operator()( R && range ) const
        -> std::ranges::borrowed_range auto
    {
        const auto grain = cfr::ranges::automatic_grain( range );
        
        return this->operator()( std::forward<R>( range ), grain );
    }
    
    template <class R>
        requires cfr::ranges::bounded_range<R>
    [[nodiscard]] constexpr auto operator()( R && range, [[maybe_unused]] std::size_t grain ) const
        -> std::ranges::borrowed_range auto
    {
        // Range is already adapted
        if      constexpr (cfr::ranges::     divisible_range<R>) return std::forward<R>( range );
        else if constexpr (cfr::ranges::     chunkable_range<R>) return std::forward<R>( range );
        else if constexpr (cfr::ranges::tbb_compatible_range<R>) return std::forward<R>( range );
        // Range requires adaptation
        else if constexpr (cfr::ranges::indexable_range<R>) return cfr::views::divisible( std::forward<R>( range ), grain );
        else                                                return cfr::views::chunkable( std::forward<R>( range ), grain );
    }
    
};

inline constexpr auto adapt = adapt_fn{};

} // namespace cfr::views

// Enable compatibility of `tbb::blocked_range<>` with C++20 range concepts, but only if given
// instantiation is actually a range. In general case `tbb::blocked_range<>` might contain any
// value as its so-called `const_iterator`, to the point of not even being compatible with
// `for (auto && e : range)`, let alone standard concepts that require semiregularity.
template <class I>
    requires std::ranges::range<tbb::blocked_range<I>>
inline constexpr bool std::ranges::enable_borrowed_range<tbb::blocked_range<I>> = true;

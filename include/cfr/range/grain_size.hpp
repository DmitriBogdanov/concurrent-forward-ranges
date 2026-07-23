// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Generalized grain size query.

#include <cfr/concept/grained_range.hpp> // cfr::ranges::grained_range<>

namespace cfr::ranges {

struct grain_size_fn {

    template <class R>
        requires cfr::ranges::grained_range<R>
    [[nodiscard]] constexpr auto operator()( R && range ) const
        -> std::size_t
    {
        if consteval {
            return std::size_t( 1 );
        }
        else {
            if constexpr (requires{ range.grainsize(); }) return std::size_t( range.grainsize () );
            else                                          return std::size_t( range.grain_size() );
        }
    }
};

inline constexpr auto grain_size = grain_size_fn{};

} // namespace cfr::ranges

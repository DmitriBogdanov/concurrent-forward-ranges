// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Niebloid for explicitly computing range size, uses O(1) size if possible,
//          otherwise falls back onto a full O(N) distance-to-sentinel evaluation.

#include <ranges> // std::ranges::size, std::ranges::distance

#include <cfr/concept/sizable_range.hpp> // cfr::ranges::sizable_range<>

namespace cfr::ranges {

struct explicit_size_fn {

    template <class R>
        requires cfr::ranges::sizable_range<R>
    [[nodiscard]] constexpr auto operator()( R && range ) const
        -> std::size_t
    {
        if constexpr (std::ranges::sized_range<R>) return std::size_t( std::ranges::    size( range ) );
        else                                       return std::size_t( std::ranges::distance( range ) );
    }
};

inline constexpr auto explicit_size = explicit_size_fn{};

} // namespace cfr::ranges

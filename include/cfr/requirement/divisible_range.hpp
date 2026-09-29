// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for divisible (recursively subdividable) ranges.

#include <concepts> // std::convertible_to<>

#include <cfr/concept/bounded_range.hpp> // cfr::ranges::bounded_range<>
#include <cfr/customization/split.hpp>   // cfr::split<>

namespace cfr::ranges {

template <class R>
concept divisible_range = requires ( R && range ) {
    requires cfr::ranges::bounded_range<R>;
    
    { cfr::split<std::remove_cvref_t<R>>::subdivide   ( range ) } -> std::convertible_to<R>;
    { cfr::split<std::remove_cvref_t<R>>::subdivisible( range ) } -> std::convertible_to<bool>;
};

} // namespace cfr::ranges

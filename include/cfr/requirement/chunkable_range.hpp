// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for chunkable (linearly subdividable) ranges.

#include <concepts> // std::convertible_to<>
#include <ranges>   // std::ranges::forward_range<>

#include <cfr/concept/bounded_range.hpp> // cfr::ranges::bounded_range<>
#include <cfr/customization/chunk.hpp>   // cfr::chunk<>

namespace cfr::ranges {

template <class R>
concept chunkable_range = requires ( R && range ) {
    requires cfr::ranges::bounded_range<R>;
    
    { cfr::chunk<std::remove_cvref_t<R>>::subdivide   ( range ) } -> std::ranges::forward_range;
    { cfr::chunk<std::remove_cvref_t<R>>::subdivisible( range ) } -> std::convertible_to<bool>;
};

} // namespace cfr::ranges

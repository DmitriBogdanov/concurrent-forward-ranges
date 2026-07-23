// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for chunkable (linearly subdividable) ranges.

#include <concepts> // std::convertible_to<>
#include <ranges>   // std::ranges::forward_range<>

#include <cfr/concept/bounded_range.hpp> // cfr::ranges::bounded_range<>

namespace cfr::ranges {

template <class R>
concept chunkable_range = requires ( R && range ) {
    requires cfr::ranges::bounded_range<R>;
    
    { range.chunk() } -> std::ranges::forward_range;
    
    { range.is_chunkable() } -> std::convertible_to<bool>;
};

} // namespace cfr::ranges

// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for chunkable (linear subdivision) ranges.

#include <concepts> // std::convertible_to<>
#include <ranges>   // std::ranges::forward_range<>

#include <cfr/concept/sizable_range.hpp> // cfr::ranges::sizable_range<>

namespace cfr::ranges {

template <class R>
concept chunkable_range = requires ( R && range ) {
    requires cfr::ranges::sizable_range<R>;
    
    { range.chunk() } -> std::ranges::forward_range;
    
    { range.is_chunkable() } -> std::convertible_to<bool>;
};

} // namespace cfr::ranges

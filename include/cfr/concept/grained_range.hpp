// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for ranges that define a grain size.

#include <concepts> // std::convertible_to<>

#include <ranges> // std::ranges::range<>

namespace cfr::ranges {

template <class R>
concept grained_range = std::ranges::range<R> && (
    requires ( R && range ) { { range.grainsize () } -> std::convertible_to<std::size_t>; } ||
    requires ( R && range ) { { range.grain_size() } -> std::convertible_to<std::size_t>; }
);

} // namespace cfr::ranges

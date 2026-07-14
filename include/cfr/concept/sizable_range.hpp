// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for ranges with computable size (aka possibly using an explicit O(N) distance check).

#include <ranges> // std::ranges::forward_range<>

#include <cfr/concept/finite_range.hpp> // cfr::ranges::finite_range<>

namespace cfr::ranges {

template <class R>
concept sizable_range = std::ranges::forward_range<R> && cfr::ranges::finite_range<R>;

} // namespace cfr::ranges

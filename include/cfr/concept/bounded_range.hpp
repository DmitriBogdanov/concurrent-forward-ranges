// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for ranges with computable finite size.

#include <concepts> // std::convertible_to<>
#include <iterator> // std::unreachable_sentinel_t
#include <ranges>   // std::ranges::forward_range<>, std::ranges::sentinel_t<>

namespace cfr::ranges {

template <class R>
concept bounded_range = std::ranges::forward_range<R> && !std::convertible_to<std::ranges::sentinel_t<R>, std::unreachable_sentinel_t>;

} // namespace cfr::ranges

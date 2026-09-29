// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Customization point for recursively divisible ranges.

#include <ranges> // std::ranges::forward_range<>

namespace cfr {

// Specialize this struct to add the ability to split recursively for range `R`:
//
// > template <>
// > struct cft::split<custom_range> {
// > 
// >     [[nodiscard]] constexpr auto subdivide( custom_range & range ) const
// >         -> custom_range
// >     {
// >         // subdivide range into smaller parts
// >     }
// >
// >     [[nodiscard]] constexpr auto subdivisible( const custom_range & range ) const
// >         -> bool
// >     {
// >         // check whether range subdivision is possible
// >     }
// > 
// > };
//
template <std::ranges::forward_range>
struct split;

} // namespace cfr

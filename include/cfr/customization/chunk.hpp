// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Customization point for linearly chunkable ranges.

#include <ranges> // std::ranges::forward_range<>

namespace cfr {

// Specialize this struct to add the ability to chunk linearly for range `R`:
//
// > template <>
// > struct cft::split<custom_range> {
// > 
// >     [[nodiscard]] constexpr auto subdivide( custom_range & range ) const
// >         -> custom_subrange
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
// Note that subrange doesn't have to match the type of the original range.
//
template <std::ranges::forward_range>
struct chunk;

} // namespace cfr

// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for bounded random access ranges.

#include <ranges> // std::ranges::random_access_range<>

#include <cfr/concept/bounded_range.hpp> // cfr::ranges::bounded_range<>

namespace cfr::ranges {

template <class R>
concept indexable_range = requires ( R && range ) {
    requires cfr::ranges::bounded_range<R>;
    
    requires std::ranges::random_access_range<R>;
};

} // namespace cfr::ranges

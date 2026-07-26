// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concept for ranges suitable for parallel execution.

#include <cfr/concept/indexable_range.hpp>          // cfr::ranges::indexable_range<>
#include <cfr/requirement/chunkable_range.hpp>      // cfr::ranges::chunkable_range<>
#include <cfr/requirement/divisible_range.hpp>      // cfr::ranges::divisible_range<>
#include <cfr/requirement/tbb_compatible_range.hpp> // cfr::ranges::tbb_compatible_range<>

namespace cfr::ranges {

template <class R>
concept parallel_range =
    cfr::ranges::tbb_compatible_range<R> ||
    cfr::ranges::     divisible_range<R> ||
    cfr::ranges::     chunkable_range<R> ||
    cfr::ranges::     indexable_range<R>;

} // namespace cfr::ranges

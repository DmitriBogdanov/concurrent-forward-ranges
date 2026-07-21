// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <array> // std::array<>

#include <tbb/blocked_range.h> // tbb::blocked_range<>

#include <cfr/concept/tbb_compatible_range.hpp> // cfr::ranges::tbb_compatible_range<>

TEST_CASE( "cfr::ranges::tbb_compatible_range<> / tbb::blocked_range<>" ) {
    
    constexpr auto range = std::array{ 0, 1, 2, 3, 4, 5 };
    
    static_assert( !cfr::ranges::tbb_compatible_range<decltype( range )> );
    
    const auto blocked = tbb::blocked_range{ range.begin(), range.end() };
    
    static_assert( cfr::ranges::tbb_compatible_range<decltype( blocked )> );
    
}

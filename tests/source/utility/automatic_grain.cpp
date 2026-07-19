// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <array> // std::array<>

#include <cfr/utility/automatic_grain.hpp> // cfr::ranges::automatic_grain

TEST_CASE( "cfr::ranges::automatic_grain / invariants" ) {
    
    constexpr auto range = std::array{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    
    STATIC_CHECK( cfr::ranges::automatic_grain( range ) > 0 );
    
}

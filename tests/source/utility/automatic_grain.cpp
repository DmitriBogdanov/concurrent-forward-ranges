// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/too-many-containers
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <array> // std::array<>

#include <cfr/utility/automatic_grain.hpp> // cfr::ranges::automatic_grain

TEST_CASE("`constexpr` evaluation") {
    
    constexpr auto range = std::array{ 0, 1, 2, 3, 4 };
    constexpr auto grain = cfr::ranges::automatic_grain( range );
    
    static_assert( grain > 0 );
    
}

TEST_CASE("Uninitialized task arena grain") {
    
    const auto range = std::array{ 0, 1, 2, 3, 4 };
    const auto grain = cfr::ranges::automatic_grain( range );
    
    static_assert( grain > 0 );
    
}

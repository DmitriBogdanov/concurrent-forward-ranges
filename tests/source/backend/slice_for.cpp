// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <array> // std::array<>

#include <cfr/backend/slice_for.hpp> // cfr::ranges::slice_for

TEST_CASE( "cfr::ranges::slice_for / compile-time evaluation" ) {
    
    constexpr auto res = [] {
        auto range = std::array{ 0, 1, 2, 3 };
        
        cfr::ranges::slice_for( range, [&]( auto && slice ) { for (auto && value : slice) value += 10; });
        
        return range;
    }();
    
    static_assert( res == std::array{ 10, 11, 12, 13 } );
    
}

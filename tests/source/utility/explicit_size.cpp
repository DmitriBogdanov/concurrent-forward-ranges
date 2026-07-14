// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/too-many-containers
//
// SPDX-License-Identifier: MIT

#include <common/framework.hpp>

#include <array>        // std::array<>
#include <forward_list> // std::forward_list<>
#include <vector>       // std::vector<>

#include <cfr/utility/explicit_size.hpp> // cfr::ranges::explicit_size

TEST_CASE("`constexpr` evaluation") {
    
    constexpr auto range = std::array{ 0, 1, 2, 3, 4 };
    constexpr auto size  = cfr::ranges::explicit_size(range);
    
    static_assert( size == 5 );
    
}

TEST_CASE("O( 1 ) sizable range") {
    
    const auto range = std::vector{ 0, 1, 2, 3, 4 };
    const auto size  = cfr::ranges::explicit_size(range);
    
    static_assert( std::ranges::sized_range<decltype(range)> );
    
    CHECK( size == 5 );
    
}

TEST_CASE("O( N ) sizable range") {
    
    const auto range = std::forward_list{ 0, 1, 2, 3, 4 };
    const auto size  = cfr::ranges::explicit_size(range);
    
    static_assert( !std::ranges::sized_range<decltype(range)> );
    
    CHECK( size == 5 );
    
}

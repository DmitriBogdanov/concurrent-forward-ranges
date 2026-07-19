// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/too-many-containers
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <array>        // std::array<>
#include <forward_list> // std::forward_list<>

#include <cfr/utility/explicit_size.hpp> // cfr::ranges::explicit_size

TEST_CASE( "cfr::ranges::explicit_size / O( 1 ) case" ) {
    
    constexpr auto range = std::array{ 0, 1, 2, 3, 4 };
    
    static_assert( std::ranges::sized_range<decltype( range )> );
    
    STATIC_CHECK( cfr::ranges::explicit_size( range ) == 5 );
    
}

TEST_CASE( "cfr::ranges::explicit_size / O( N ) case" ) {
    
    const auto range = std::forward_list{ 0, 1, 2, 3, 4 };
    
    static_assert( !std::ranges::sized_range<decltype( range )> );
    
    CHECK( cfr::ranges::explicit_size( range ) == 5 );
    
}

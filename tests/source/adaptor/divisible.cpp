// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/too-many-containers
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <forward_list> // std::forward_list<>
#include <list>         // std::list<>
#include <vector>       // std::vector<>

#include <cfr/adaptor/divisible.hpp> // cfr::views::divisible

TEST_CASE( "cfr::views::divisible / range category" ) {
    
    const auto range_0 = std::vector      <int>{};
    const auto range_1 = std::list        <int>{};
    const auto range_2 = std::forward_list<int>{};
    
    static_assert( std::ranges::   contiguous_range<decltype( cfr::views::divisible( range_0 ) )> );
    static_assert( std::ranges::random_access_range<decltype( cfr::views::divisible( range_0 ) )> );
    static_assert( std::ranges::bidirectional_range<decltype( cfr::views::divisible( range_1 ) )> );
    static_assert( std::ranges::      forward_range<decltype( cfr::views::divisible( range_2 ) )> );
    
}

TEST_CASE( "cfr::views::divisible / view interface" ) {
    
    constexpr auto range = std::array{ 10, 20, 30 };
    
    STATIC_CHECK( cfr::views::divisible( range ) );
    
    STATIC_CHECK( cfr::views::divisible( range ).empty() == false );
    STATIC_CHECK( cfr::views::divisible( range ).size () == 3     );
    
    STATIC_CHECK( cfr::views::divisible( range ).front() == 10 );
    STATIC_CHECK( cfr::views::divisible( range ).back () == 30 );
    
    STATIC_CHECK( cfr::views::divisible( range )[0] == 10 );
    STATIC_CHECK( cfr::views::divisible( range )[1] == 20 );
    STATIC_CHECK( cfr::views::divisible( range )[2] == 30 );
    
}

TEST_CASE( "cfr::views::divisible / subdivision" ) {
    
    constexpr auto range = std::array{ 10, 20, 30 };
    
    static_assert( cfr::views::divisible( range ).split() );
    
    static_assert( cfr::views::divisible( range ).is_divisible() );
    
    static_assert( cfr::views::divisible( range ).grainsize() );
    
}

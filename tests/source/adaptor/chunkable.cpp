// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <forward_list> // std::forward_list<>
#include <list>         // std::list<>
#include <vector>       // std::vector<>

#include <cfr/adaptor/chunkable.hpp> // cfr::views::chunkable

TEST_CASE( "cfr::views::chunkable / range category" ) {
    
    const auto range_0 = std::vector      <int>{};
    const auto range_1 = std::list        <int>{};
    const auto range_2 = std::forward_list<int>{};
    
    static_assert( std::ranges::   contiguous_range<decltype( cfr::views::chunkable( range_0 ) )> );
    static_assert( std::ranges::random_access_range<decltype( cfr::views::chunkable( range_0 ) )> );
    static_assert( std::ranges::bidirectional_range<decltype( cfr::views::chunkable( range_1 ) )> );
    static_assert( std::ranges::      forward_range<decltype( cfr::views::chunkable( range_2 ) )> );
    
}

TEST_CASE( "cfr::views::chunkable / view interface" ) {
    
    constexpr auto range = std::array{ 10, 20, 30 };
    
    STATIC_CHECK( cfr::views::chunkable( range ) );
    
    STATIC_CHECK( cfr::views::chunkable( range ).empty() == false );
    STATIC_CHECK( cfr::views::chunkable( range ).size () == 3     );
    
    STATIC_CHECK( cfr::views::chunkable( range ).front() == 10 );
    STATIC_CHECK( cfr::views::chunkable( range ).back () == 30 );
    
    STATIC_CHECK( cfr::views::chunkable( range )[0] == 10 );
    STATIC_CHECK( cfr::views::chunkable( range )[1] == 20 );
    STATIC_CHECK( cfr::views::chunkable( range )[2] == 30 );
    
}

TEST_CASE( "cfr::views::chunkable / subdivision" ) {
    
    constexpr auto range = std::array{ 10, 20, 30 };
    
    STATIC_CHECK( cfr::views::chunkable( range ).chunk() );
    
    STATIC_CHECK( cfr::views::chunkable( range ).is_chunkable() );
    
    STATIC_CHECK( cfr::views::chunkable( range ).grainsize() );
    
}

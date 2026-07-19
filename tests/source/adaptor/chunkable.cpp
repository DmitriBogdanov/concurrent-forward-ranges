// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/too-many-containers
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <forward_list> // std::forward_list<>
#include <list>         // std::list<>
#include <vector>       // std::vector<>

#include <cfr/adaptor/chunkable.hpp> // cfr::views::chunkable

TEST_CASE("Range category preservation") {
    
    const auto range_0 = std::vector      <int>{};
    const auto range_1 = std::list        <int>{};
    const auto range_2 = std::forward_list<int>{};
    
    static_assert( std::ranges::   contiguous_range<decltype( cfr::views::chunkable( range_0 ) )> );
    static_assert( std::ranges::random_access_range<decltype( cfr::views::chunkable( range_0 ) )> );
    static_assert( std::ranges::bidirectional_range<decltype( cfr::views::chunkable( range_1 ) )> );
    static_assert( std::ranges::      forward_range<decltype( cfr::views::chunkable( range_2 ) )> );
    
}

TEST_CASE("`constexpr` viewing support") {
    
    constexpr auto range = std::array{ 10, 20, 30 };
    
    static_assert( cfr::views::chunkable( range ) );
    
    static_assert( cfr::views::chunkable( range ).empty() == false );
    static_assert( cfr::views::chunkable( range ).size () == 3     );
    
    static_assert( cfr::views::chunkable( range ).front() == 10 );
    static_assert( cfr::views::chunkable( range ).back () == 30 );
    
    static_assert( cfr::views::chunkable( range )[0] == 10 );
    static_assert( cfr::views::chunkable( range )[1] == 20 );
    static_assert( cfr::views::chunkable( range )[2] == 30 );
    
}

TEST_CASE("`constexpr` subdivision support") {
    
    constexpr auto range = std::array{ 10, 20, 30 };
    
    static_assert( cfr::views::chunkable( range ).chunk() );
    
    static_assert( cfr::views::chunkable( range ).is_chunkable() );
    
    static_assert( cfr::views::chunkable( range ).grainsize() );
    
}


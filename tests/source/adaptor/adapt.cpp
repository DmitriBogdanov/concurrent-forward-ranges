// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <array>        // std::array<>
#include <deque>        // std::deque<>
#include <forward_list> // std::forward_list<>
#include <list>         // std::list<>

#include <tbb/blocked_range.h> // tbb::blocked_range<>

#include <cfr/adaptor/adapt.hpp>     // cfr::views::adapt
#include <cfr/adaptor/chunkable.hpp> // cfr::views::chunkable
#include <cfr/adaptor/divisible.hpp> // cfr::views::divisible

TEST_CASE( "cfr::view::adapt / range category" ) {
    
    const auto range_0 = std::vector      <int>{};
    const auto range_1 = std::deque       <int>{};
    const auto range_2 = std::list        <int>{};
    const auto range_3 = std::forward_list<int>{};
    
    static_assert( std::ranges::   contiguous_range<decltype( cfr::views::adapt( range_0    ) )> ); //
    static_assert( std::ranges::random_access_range<decltype( cfr::views::adapt( range_1    ) )> ); //
    static_assert( std::ranges::bidirectional_range<decltype( cfr::views::adapt( range_2    ) )> ); //
    static_assert( std::ranges::      forward_range<decltype( cfr::views::adapt( range_3, 1 ) )> ); // not O( 1 ) sized
    
}

TEST_CASE( "cfr::view::adapt / preserve cfr::ranges::divisible_range<>" ) {
    
    auto data = std::array{ 0, 1, 2, 3, 4 };
    
    auto range = cfr::views::divisible( data );
    
    static_assert( cfr::ranges::divisible_range<decltype( cfr::views::adapt( range ) )> ); // should preserve kind
    static_assert( std::ranges:: borrowed_range<decltype( cfr::views::adapt( range ) )> ); // should borrow
    
}

TEST_CASE( "cfr::view::adapt / preserve cfr::ranges::chunkable_range<>" ) {
    
    auto data = std::array{ 0, 1, 2, 3, 4 };
    
    auto range = cfr::views::chunkable( data );
    
    static_assert( cfr::ranges::chunkable_range<decltype( cfr::views::adapt( range ) )> ); // should preserve kind
    static_assert( std::ranges:: borrowed_range<decltype( cfr::views::adapt( range ) )> ); // should borrow
    
}

TEST_CASE( "cfr::view::adapt / preserve cfr::ranges::tbb_compatible_range<>" ) {
    
    auto data = std::array{ 0, 1, 2, 3, 4 };
    
    auto range = tbb::blocked_range{ data.begin(), data.end() };
    
    static_assert( cfr::ranges::tbb_compatible_range<decltype( cfr::views::adapt( range ) )> ); // should preserve kind
    static_assert( std::ranges::      borrowed_range<decltype( cfr::views::adapt( range ) )> ); // should borrow
    
}

TEST_CASE( "cfr::view::adapt / adapt std::ranges::random_access_range<>" ) {
    
    auto range = std::array{ 0, 1, 2, 3, 4 };
    
    static_assert( cfr::ranges::divisible_range<decltype( cfr::views::adapt( range ) )> ); // should deduce division
    static_assert( std::ranges:: borrowed_range<decltype( cfr::views::adapt( range ) )> ); // should borrow
    
}

TEST_CASE( "cfr::view::adapt / adapt std::ranges::forward_range<>" ) {
    
    auto range = std::list{ 0, 1, 2, 3, 4 };
    
    static_assert( cfr::ranges::chunkable_range<decltype( cfr::views::adapt( range ) )> ); // should deduce chunking
    static_assert( std::ranges:: borrowed_range<decltype( cfr::views::adapt( range ) )> ); // should borrow
    
}

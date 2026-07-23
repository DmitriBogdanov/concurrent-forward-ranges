// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <deque>        // std::deque<>
#include <forward_list> // std::forward_list<>
#include <list>         // std::list<>
#include <vector>       // std::vector<>

// <includes: tbb>

#include <cfr/adaptor/divisible.hpp> // cfr::views::divisible
#include <cfr/adaptor/tbb_compatible.hpp> // cfr::views::tbb_compatible
#include <cfr/requirement/tbb_compatible_range.hpp> // cfr::ranges::tbb_compatible_range<>

namespace {
    
constexpr auto cfr_adapt = cfr::views::divisible;      // to make expressions
constexpr auto tbb_adapt = cfr::views::tbb_compatible; // fit on a single line
    
} // namespace

TEST_CASE( "cfr::views::tbb_compatible / range category" ) {
    
    const auto range_0 = std::vector      <int>{};
    const auto range_1 = std::deque       <int>{};
    const auto range_2 = std::list        <int>{};
    const auto range_3 = std::forward_list<int>{};
    
    static_assert( std::ranges::   contiguous_range<decltype( tbb_adapt( cfr_adapt( range_0    ) ) )> ); //
    static_assert( std::ranges::random_access_range<decltype( tbb_adapt( cfr_adapt( range_1    ) ) )> ); //
    static_assert( std::ranges::bidirectional_range<decltype( tbb_adapt( cfr_adapt( range_2    ) ) )> ); //
    static_assert( std::ranges::      forward_range<decltype( tbb_adapt( cfr_adapt( range_3, 1 ) ) )> ); // not O( 1 ) sized
    
}

TEST_CASE( "cfr::views::tbb_compatible / borrowing" ) {
    
    const auto range_0 = std::vector      <int>{};
    const auto range_1 = std::deque       <int>{};
    const auto range_2 = std::list        <int>{};
    const auto range_3 = std::forward_list<int>{};
    
    static_assert( std::ranges::borrowed_range<decltype( tbb_adapt( cfr_adapt( range_0    ) ) )> ); //
    static_assert( std::ranges::borrowed_range<decltype( tbb_adapt( cfr_adapt( range_1    ) ) )> ); //
    static_assert( std::ranges::borrowed_range<decltype( tbb_adapt( cfr_adapt( range_2    ) ) )> ); //
    static_assert( std::ranges::borrowed_range<decltype( tbb_adapt( cfr_adapt( range_3, 1 ) ) )> ); // not O( 1 ) sized
    
}

TEST_CASE( "cfr::views::tbb_compatible / view interface" ) {
    
    constexpr auto range = std::array{ 10, 20, 30 };
    
    STATIC_CHECK( tbb_adapt( cfr_adapt( range ) ) );
    
    STATIC_CHECK( tbb_adapt( cfr_adapt( range ) ).empty() == false );
    STATIC_CHECK( tbb_adapt( cfr_adapt( range ) ).size () == 3     );
    
    STATIC_CHECK( tbb_adapt( cfr_adapt( range ) ).front() == 10 );
    STATIC_CHECK( tbb_adapt( cfr_adapt( range ) ).back () == 30 );
    
    STATIC_CHECK( tbb_adapt( cfr_adapt( range ) )[0] == 10 );
    STATIC_CHECK( tbb_adapt( cfr_adapt( range ) )[1] == 20 );
    STATIC_CHECK( tbb_adapt( cfr_adapt( range ) )[2] == 30 );
    
}

TEST_CASE( "cfr::views::tbb_compatible / subdivision" ) {
    
    constexpr auto range = std::array{ 10, 20, 30 };
    
    static_assert( tbb_adapt( cfr_adapt( range ) ).is_divisible() );
    
    static_assert( tbb_adapt( cfr_adapt( range ) ).grainsize() );
    
}

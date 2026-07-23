// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <array> // std::array<>

#include <tbb/blocked_range.h> // tbb::blocked_range<>

#include <cfr/adaptor/chunkable.hpp> // cfr::views::chunkable
#include <cfr/adaptor/divisible.hpp> // cfr::views::divisible
#include <cfr/range/grain_size.hpp>  // cfr::ranges::grain_size

TEST_CASE( "cfr::ranges::grain_size / compile-time evaluation" ) {
    
    constexpr auto range = std::array{ 0, 1, 2, 3, 4 };
    
    STATIC_CHECK( cfr::ranges::grain_size( cfr::views::chunkable( range ) ) >= 1 );
    STATIC_CHECK( cfr::ranges::grain_size( cfr::views::divisible( range ) ) >= 1 );
    
}

TEST_CASE( "cfr::ranges::grain_size / tbb::blocked_range<>" ) {
    
    const auto range = std::array{ 0, 1, 2, 3, 4 };
    
    static_assert( !cfr::ranges::grained_range<decltype( range )> );
    
    const auto adapted = tbb::blocked_range{ range.begin(), range.end() };
    
    static_assert( cfr::ranges::grained_range<decltype( adapted )> );
    
    CHECK( cfr::ranges::grain_size( adapted ) >= 1 );
    
}

TEST_CASE( "cfr::ranges::grain_size / cfr::views::chunkable" ) {
    
    const auto range = std::array{ 0, 1, 2, 3, 4 };
    
    static_assert( !cfr::ranges::grained_range<decltype( range )> );
    
    const auto adapted = cfr::views::chunkable( range );
    
    static_assert( cfr::ranges::grained_range<decltype( adapted )> );
    
    CHECK( cfr::ranges::grain_size( adapted ) >= 1 );
    
}

TEST_CASE( "cfr::ranges::grain_size / cfr::views::divisible" ) {
    
    const auto range = std::array{ 0, 1, 2, 3, 4 };
    
    static_assert( !cfr::ranges::grained_range<decltype( range )> );
    
    const auto adapted = cfr::views::divisible( range );
    
    static_assert( cfr::ranges::grained_range<decltype( adapted )> );
    
    CHECK( cfr::ranges::grain_size( adapted ) >= 1 );
    
}

// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/fuzz_test.hpp>
#include <common/unit_test.hpp>

#include <array>  // std::array<>
#include <deque>  // std::deque<>
#include <list>   // std::list<>
#include <vector> // std::vector<>

#include <cfr/adaptor/adapt.hpp>     // cfr::views::adapt
#include <cfr/adaptor/chunkable.hpp> // cfr::views::chunkable
#include <cfr/adaptor/divisible.hpp> // cfr::views::divisible
#include <cfr/backend/slice_for.hpp> // cfr::ranges::slice_for

TEST_CASE( "cfr::ranges::slice_for / compile-time evaluation" ) {
    
    constexpr auto evaluate_checked = [] {
        auto range = std::array{ 0, 1, 2, 3 };
        
        cfr::ranges::slice_for( range, [&]( auto && slice ) { for (auto && value : slice) value += 10; });
        
        return range;
    };
    
    constexpr auto evaluate_correct = [] {
        return std::array{ 10, 11, 12, 13 };
    };
    
    STATIC_CHECK( evaluate_checked() == evaluate_correct() );
    
}

namespace {

template <cfr::ranges::bounded_range R, std::invocable<R> A>
void fuzz_case( A adaptor ) {
    ut::fuzz_test( [&]{
        /* */ auto checked_range = R{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9 };
        const auto correct_range = R{ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
        
        cfr::ranges::slice_for( adaptor( checked_range ), [&]( auto && slice ) {
            for (auto && value : slice) value += 10;
        } );
        
        CHECK( std::ranges::equal( checked_range, correct_range ) );
    } );
}

} // namespace

TEST_CASE( "cfr::ranges::slice_for / fuzzing" ) {
    
    fuzz_case<std::vector<int>>( std::views::all       );
    fuzz_case<std::vector<int>>( cfr::views::adapt     );
    fuzz_case<std::vector<int>>( cfr::views::chunkable );
    fuzz_case<std::vector<int>>( cfr::views::divisible );
    
    fuzz_case<std:: deque<int>>( std::views::all       );
    fuzz_case<std:: deque<int>>( cfr::views::adapt     );
    fuzz_case<std:: deque<int>>( cfr::views::chunkable );
    fuzz_case<std:: deque<int>>( cfr::views::divisible );
    
    fuzz_case<std::  list<int>>( cfr::views::adapt     );
    fuzz_case<std::  list<int>>( cfr::views::chunkable );
    fuzz_case<std::  list<int>>( cfr::views::divisible );
    
}

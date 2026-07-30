// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/fuzz_test.hpp>
#include <common/unit_test.hpp>

#include <array>  // std::array<>
#include <deque>  // std::deque<>
#include <vector> // std::vector<>

#include <cfr/adaptor/adapt.hpp>       // cfr::views::adapt
#include <cfr/adaptor/chunkable.hpp>   // cfr::views::chunkable
#include <cfr/adaptor/divisible.hpp>   // cfr::views::divisible
#include <cfr/algorithm/transform.hpp> // cfr::ranges::transform

TEST_CASE( "cfr::ranges::transform / compile-time evaluation" ) {
    
    constexpr auto evaluate_checked = [] {
        auto range = std::array{ 0, 1, 2, 3 };
        
        auto result = std::array{ 0, 0, 0, 0 };
        
        cfr::ranges::transform( range, result, [&]( auto && value ) { return value + 10; } );
        
        return result;
    };
    
    constexpr auto evaluate_correct = [] {
        return std::array{ 10, 11, 12, 13 };
    };
    
    STATIC_CHECK( evaluate_checked() == evaluate_correct() );
    
}

namespace {

struct callable {
    std::size_t state = 0;
    
    constexpr auto operator()( auto && value ) { 
        return value + 10;
    }
};

template <cfr::ranges::bounded_range R, std::invocable<R> A>
void verify( A adaptor ) {
    
    /* */ auto checked_range = R{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9 };
    /* */ auto storage_range = R{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };
    const auto correct_range = R{ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
    
    const auto [source_iter, target_iter] = cfr::ranges::transform( adaptor( checked_range ), adaptor( storage_range ), callable{} );
    
    REQUIRE( std::ranges::equal( storage_range, correct_range ) ); // result is correct
    
    REQUIRE( source_iter == std::ranges::end( checked_range ) );
    REQUIRE( target_iter == std::ranges::end( storage_range ) );
    
}

} // namespace

TEST_CASE( "cfr::ranges::transform / fuzzing" ) {
    
    ut::fuzz_test( [] { verify<std::vector<int>>( std::views::all       ); } );
    ut::fuzz_test( [] { verify<std::vector<int>>( cfr::views::adapt     ); } );
    ut::fuzz_test( [] { verify<std::vector<int>>( cfr::views::chunkable ); } );
    ut::fuzz_test( [] { verify<std::vector<int>>( cfr::views::divisible ); } );
    
    ut::fuzz_test( [] { verify<std:: deque<int>>( std::views::all       ); } );
    ut::fuzz_test( [] { verify<std:: deque<int>>( cfr::views::adapt     ); } );
    ut::fuzz_test( [] { verify<std:: deque<int>>( cfr::views::chunkable ); } );
    ut::fuzz_test( [] { verify<std:: deque<int>>( cfr::views::divisible ); } );
    
}

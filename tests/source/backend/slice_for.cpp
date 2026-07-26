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

struct callable {
    std::size_t state = 0;
    
    void operator()( auto && slice ) { 
        for (auto && value : slice) { value += 10; ut::atomic_increment( this->state ); }
    }
};

template <cfr::ranges::bounded_range R, std::invocable<R> A>
void verify( A adaptor ) {
    
    /* */ auto checked_range = R{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9 };
    const auto correct_range = R{ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
    
    const auto [iter, func] = cfr::ranges::slice_for( adaptor( checked_range ), callable{} );
    
    CHECK( std::ranges::equal( checked_range, correct_range ) ); // result is correct
    
    CHECK( func.state == std::ranges::size( correct_range ) ); // functor state is accumulated
    
}

} // namespace

TEST_CASE( "cfr::ranges::slice_for / fuzzing" ) {
    
    ut::fuzz_test( [] { verify<std::vector<int>>( std::views::all       ); } );
    ut::fuzz_test( [] { verify<std::vector<int>>( cfr::views::adapt     ); } );
    ut::fuzz_test( [] { verify<std::vector<int>>( cfr::views::chunkable ); } );
    ut::fuzz_test( [] { verify<std::vector<int>>( cfr::views::divisible ); } );
    
    ut::fuzz_test( [] { verify<std:: deque<int>>( std::views::all       ); } );
    ut::fuzz_test( [] { verify<std:: deque<int>>( cfr::views::adapt     ); } );
    ut::fuzz_test( [] { verify<std:: deque<int>>( cfr::views::chunkable ); } );
    ut::fuzz_test( [] { verify<std:: deque<int>>( cfr::views::divisible ); } );
    
    ut::fuzz_test( [] { verify<std::  list<int>>( cfr::views::adapt     ); } );
    ut::fuzz_test( [] { verify<std::  list<int>>( cfr::views::chunkable ); } );
    ut::fuzz_test( [] { verify<std::  list<int>>( cfr::views::divisible ); } );
    
}

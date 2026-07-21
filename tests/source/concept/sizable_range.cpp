// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <forward_list> // std::forward_list()
#include <list>         // std::list<>
#include <ranges>       // std::views::iota

#include <cfr/concept/sizable_range.hpp> // cfr::ranges::sizable_range<>

TEST_CASE( "cfr::ranges::sizable_range<> / O(1) sizable" ) {
    
    const auto range = std::list{ 0, 1, 2, 3 };
    
    static_assert( cfr::ranges::sizable_range<decltype( range )> );
    
}

TEST_CASE( "cfr::ranges::sizable_range<> / O(N) sizable" ) {
    
    const auto range = std::forward_list{ 0, 1, 2, 3 };
    
    static_assert( cfr::ranges::sizable_range<decltype( range )> );
    
}

TEST_CASE( "cfr::ranges::sizable_range<> / bound sentinel range" ) {
    
    const auto range = std::views::iota(0, 15);
    
    static_assert( cfr::ranges::sizable_range<decltype( range )> );
    
}

TEST_CASE( "cfr::ranges::sizable_range<> / unbound sentinel range" ) {
    
    const auto range = std::views::iota(0);
    
    static_assert( !cfr::ranges::sizable_range<decltype( range )> );
    
}

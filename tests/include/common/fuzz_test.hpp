// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Common include for fuzz testing.

#include <array> // std::array<>
#include <random> // std::mt19937, std::uniform_int_distribution<>

#include <cfr/utility/invoke_with_concurrency.hpp> // cfr::invoke_with_concurrency

namespace ut {

using random_generator          = std::mt19937;
using uniform_size_distribution = std::uniform_int_distribution<std::size_t>;
using uniform_sint_distribution = std::uniform_int_distribution<        int>;

constexpr auto worker_counts = std:: array{ 1, 2, 3, 4, 5, 6, 8, 12, 16, 24, 37 };
constexpr auto tester_cycles = std::size_t( 2 );

template <std::invocable F>
void fuzz_test( F && func ) {
    for (const auto count : worker_counts) {
        for (std::size_t cycle = 0; cycle < tester_cycles; ++cycle) {
            cfr::invoke_with_concurrency( count, func );
        }
    }
};

} // namespace ut

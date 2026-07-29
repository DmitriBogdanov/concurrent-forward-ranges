// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Common unit test include.

#define DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
// makes `CHECK_THROWS()` not warn for discarding [[nodiscard]]

#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
// makes assert macros faster to compile in exchange for slightly uglifying debugger stack trace

#define DOCTEST_CONFIG_USE_STD_HEADERS
// removes non-compliant usage of forward declarations at the cost of slightly increasing compile time

#include <doctest.h> // TEST_CASE(), CHECK(), REQUIRE(), ...

// Combined constexpr + runtime assertions
#define STATIC_CHECK(...)         static_assert( __VA_ARGS__ ); CHECK        ( __VA_ARGS__ )
#define STATIC_CHECK_FALSE(...)   static_assert( __VA_ARGS__ ); CHECK_FALSE  ( __VA_ARGS__ )
#define STATIC_REQUIRE(...)       static_assert( __VA_ARGS__ ); REQUIRE      ( __VA_ARGS__ )
#define STATIC_REQUIRE_FALSE(...) static_assert( __VA_ARGS__ ); REQUIRE_FALSE( __VA_ARGS__ )

namespace ut {
    using namespace doctest;
    
    template <class T> constexpr bool discard( T && ) { return true; }
}

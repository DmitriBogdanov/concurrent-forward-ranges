// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <cfr/utility/invoke_with_concurrency.hpp> // invoke_with_concurrency

TEST_CASE( "cfr::invoke_with_concurrency / return propagation" ) {
    
    STATIC_CHECK( cfr::invoke_with_concurrency( 8, []{ return 42; } ) == 42 );
    
}

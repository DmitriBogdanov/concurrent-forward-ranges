// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/too-many-containers
//
// SPDX-License-Identifier: MIT

#include <common/unit_test.hpp>

#include <cfr/utility/current_concurrency.hpp> // cfr::current_concurrency()

TEST_CASE( "cfr::current_concurrency / invariants" ) {
    
    static_assert( cfr::current_concurrency() == 1 );
    STATIC_CHECK ( cfr::current_concurrency() >= 1 );
    
}

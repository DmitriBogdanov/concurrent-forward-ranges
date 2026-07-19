// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Common include of all tests

#define DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
// makes `CHECK_THROWS()` not warn for discarding [[nodiscard]]

#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
// makes assert macros faster to compile in exchange for slightly uglifying debugger stack trace

#define DOCTEST_CONFIG_USE_STD_HEADERS
// remove non-compliant usage of forward declarations at the cost of slightly increasing compile time

#include <thirdparty/doctest.h> // TEST_CASE(), CHECK(), ...

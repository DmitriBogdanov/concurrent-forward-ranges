// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Contexpr-friendly invocation with local concurrency level.

#include <cassert>     // assert()
#include <concepts>    // std::invocable<>
#include <functional>  // std::invoke()
#include <type_traits> // std::invoke_result_t<>
#include <utility>     // std::forward(), std::in_range()

#include <tbb/task_arena.h> // tbb::task_arena

namespace cfr {

struct invoke_with_concurrency_fn {
    
    template <class Func>
        requires std::invocable<Func>
    constexpr auto operator()( std::size_t concurrency, Func && func ) const
        -> std::invoke_result_t<Func>
    {
        if consteval {
            return std::invoke( std::forward<Func>( func ) );
        }
        else {
            assert( std::in_range<int>( concurrency ) );
            
            return tbb::task_arena{ int( concurrency ) }.execute( std::forward<Func>( func ) );
        }
    }
    
};

constexpr auto invoke_with_concurrency = invoke_with_concurrency_fn{};

} // namespace cfr

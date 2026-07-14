// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Niebloid for querying current task arena concurrency in a `constexpr`-compatible fashion.

#include <tbb/task_arena.h> // tbb::this_task_arena::max_concurrency()

namespace cfr::this_task_arena {

struct max_concurrency_fn {
    
    [[nodiscard]] constexpr auto operator()() const
        -> std::size_t
    {
        if consteval {
            return std::size_t( 1 );
        }
        else {
            return std::size_t( tbb::this_task_arena::max_concurrency() );
        }
    }
    
};

inline constexpr auto max_concurrency = max_concurrency_fn{};

} // namespace cfr::ranges

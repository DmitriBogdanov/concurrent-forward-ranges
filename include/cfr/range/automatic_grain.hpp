// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Reasonable automatic estimate of grain size suitable for a given range.

#include <cfr/range/explicit_size.hpp>         // cfr::ranges::explicit_size
#include <cfr/utility/current_concurrency.hpp> // cfr::current_concurrency

namespace cfr::ranges {

struct automatic_grain_fn {
    
    constexpr static std::size_t tasks_per_worker = 16;
    
    template <class R>
        requires cfr::ranges::bounded_range<R>
    [[nodiscard]] constexpr auto operator()( R && range ) const
        -> std::size_t
    {
        const std::size_t workers = cfr::current_concurrency();
            // in case task arena is not yet initialized, TBB will try to return hardware concurrency
        
        const std::size_t tasks = workers * tasks_per_worker;
        
        const std::size_t estimate = cfr::ranges::explicit_size( range ) / tasks;
            // most parallel ranges will have O( 1 ) size, but in pathological cases we might have to 
            // fallback onto O( N ). Algorithm / adaptor APIs are protected against falling into this 
            // branch implicitly so we only perform O( N ) check if the user explicitly requests it.
        
        return estimate ? estimate : std::size_t( 1 );
            // TBB makes a choice of defaulting grain size to `1` (assuming we use `tbb::blocked_range`),
            // this is generally a good choice assuming recursively splittable ranges with auto partitioner,
            // however this makes it easy to accidentally wreck the performance of deterministic algorithms
            // by forgetting to manually specify their grain size (which cases simple partitioner to evaluate
            // single-element tasks). Having a decently granular default estimate based on the size is safer.
            // In our case this is also necessary for initializing chunkable ranges that don't really function
            // without a reasonable grain estimate due to their flat (rather than hierarchical) subdivision nature.
    }
    
};

inline constexpr auto automatic_grain = automatic_grain_fn{};

} // namespace cfr::ranges

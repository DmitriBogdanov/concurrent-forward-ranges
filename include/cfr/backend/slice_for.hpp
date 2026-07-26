// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Backend algorithm - parallel for.

#include <concepts>   // std::invocable<>
#include <functional> // std::invoke(), std::ref()
#include <ranges>     // std::ranges::borrowed_iterator_t<>, std::ranges::borrowed_subrange_t<>
#include <utility>    // std::forward(), std::move()

#include <tbb/parallel_for.h> // tbb::parallel_for()
#include <tbb/task_group.h>   // tbb::task_group

#include <cfr/concept/bounded_range.hpp>            // cfr::ranges::bounded_range<>
#include <cfr/concept/parallel_range.hpp>           // cfr::ranges::parallel_range<>
#include <cfr/detail/dispatch_backend.hpp>          // cfr::ranges::detail::dispatch_backend
#include <cfr/requirement/chunkable_range.hpp>      // cfr::ranges::chunkable_range<>
#include <cfr/requirement/tbb_compatible_range.hpp> // cfr::ranges::tbb_compatible_range<>

namespace cfr::ranges::detail {

struct serial_slice_for_fn {
    
    template <class R, class F>
        requires cfr::ranges::bounded_range<R> && std::invocable<F, std::ranges::borrowed_subrange_t<R>>
    constexpr auto operator()( R && range, F func ) const
        -> void
    {
        std::invoke( func, std::forward<R>( range ) );
    }
    
};

inline constexpr auto serial_slice_for = serial_slice_for_fn{};

struct native_slice_for_fn {
    
    template <class R, class F>
        requires cfr::ranges::tbb_compatible_range<R> && std::invocable<F, std::ranges::borrowed_subrange_t<R>>
    /* not constexpr */ auto operator()( R && range, F func ) const
        -> void
    {
        tbb::parallel_for( std::forward<R>( range ), func );
    }
    
};

inline constexpr auto native_slice_for = native_slice_for_fn{};

struct linear_slice_for_fn {
    
    template <class R, class F>
        requires cfr::ranges::chunkable_range<R> && std::invocable<F, std::ranges::borrowed_subrange_t<R>>
    /* not constexpr */ auto operator()( R && range, F func ) const
        -> void
    {
        tbb::task_group group;
        
        while (range.is_chunkable())
            group.run( [slice = range.chunk(), &func] { std::invoke( func, std::move( slice ) ); } );
        
        group.wait();
    }
    
};

inline constexpr auto linear_slice_for = linear_slice_for_fn{};

} // namespace cfr::ranges::detail 

namespace cfr::ranges {

struct slice_for_fn {
    
    template <class R, class F>
        requires cfr::ranges::parallel_range<R> && std::invocable<F, std::ranges::borrowed_subrange_t<R>>
    constexpr auto operator()( R && range, F func ) const
        -> std::ranges::in_fun_result<std::ranges::borrowed_iterator_t<R>, F>
    {
        auto iter = std::ranges::end( range );
        
        detail::dispatch_backend(
            detail::serial_slice_for,
            detail::native_slice_for,
            detail::linear_slice_for,
            std::forward<R>( range ),
            std::ref       ( func  )
        );
        
        // Note: Reference `func` so all threads work on the same object that can accumulate state,
        //       ensuring thread safety of such accumulation (if it exists) is up to the user.
        
        return { std::move( iter ), std::move( func ) };
    }
    
};

inline constexpr auto slice_for = slice_for_fn{};

} // namespace cfr::ranges

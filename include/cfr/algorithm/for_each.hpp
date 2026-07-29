// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Parallel implementation of `std::ranges::for_each`.

#include <algorithm>  // std::ranges::for_each, std::ranges::in_fun_result<>, std::ranges::borrowed_iterator_t<>, ...
#include <functional> // std::ref(), std::identity
#include <utility>    // std::forward(), std::move()

#include <cfr/backend/slice_for.hpp>      // cfr::ranges::slice_for
#include <cfr/concept/parallel_range.hpp> // cfr::ranges::parallel_range<>

namespace cfr::ranges {

template <class R, class F, class Proj>
concept for_each_suitable = std::indirectly_unary_invocable<F, std::projected<std::ranges::iterator_t<R>, Proj>>;
    
struct for_each_fn {
    
    template <class R, class F, class Proj = std::identity>
        requires cfr::ranges::parallel_range<R> && cfr::ranges::for_each_suitable<R, F, Proj>
    constexpr auto operator()( R && range, F func, Proj proj = {} ) const
        -> std::ranges::in_fun_result<std::ranges::borrowed_iterator_t<R>, F>
    {
        auto iter = cfr::ranges::slice_for( std::forward<R>( range ), [&] <class S> ( S && slice ) {
            std::ranges::for_each( std::forward<S>( slice ), std::ref( func ), proj );
        } ).in;
        
        return { std::move( iter ), std::move( func ) };
    }
    
};

inline constexpr auto for_each = for_each_fn{};

} // namespace cfr::ranges

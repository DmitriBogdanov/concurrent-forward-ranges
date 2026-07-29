// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Parallel implementation of `std::ranges::transform`.

#include <functional> // std::invoke(), std::identity
#include <utility>    // std::forward(), std::move()

#include <cfr/algorithm/for_each.hpp>     // cfr::ranges::for_each
#include <cfr/concept/parallel_range.hpp> // cfr::ranges::parallel_range<>

namespace cfr::ranges {

template <class R, class O, class F, class Proj>
concept transform_suitable = std::indirectly_writable<
    std::ranges::iterator_t<O>, std::indirect_result_t<F, std::projected<std::ranges::iterator_t<R>, Proj>>
>;
    
struct transform_fn {
    
    template <class R, class O, class F, class Proj = std::identity>
        requires cfr::ranges::parallel_range<R> && cfr::ranges::transform_suitable<R, O, F, Proj>
    constexpr auto operator()( R && range, O && result, F func, Proj proj ) const
        -> std::ranges::in_out_result<std::ranges::borrowed_iterator_t<R>, std::ranges::borrowed_iterator_t<O>>
    {
        auto source_end = std::ranges::end( range  );
        auto target_end = std::ranges::end( result );
        
        // Zip and subdivide 2 ranges in sync, this enables concurrent writing in general case. In case of 
        // random access the underlying behavior will be the same as in a classic offset-based implementation.
        cfr::ranges::for_each( std::views::zip( std::forward<R>( range ), std::forward<O>( result ) ),
            [&] ( auto && pair ) { std::get<1>( pair ) = std::invoke( func, std::invoke( proj, std::get<0>( pair ) ) ); }
        );
        
        return { std::move( source_end ), std::move( target_end ) };
    }
    
};

inline constexpr auto transform = transform_fn{};

} // namespace cfr::ranges

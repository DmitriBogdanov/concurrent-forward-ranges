// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Backend algorithm selector, hides ugly dispatch from everywhere else.

#include <functional> // std::invoke()
#include <utility>    // std::forward()

#include <cfr/adaptor/divisible.hpp>                // cfr::views::divisible
#include <cfr/adaptor/tbb_compatible.hpp>           // cfr::views::tbb_compatible
#include <cfr/concept/indexable_range.hpp>          // cfr::ranges::indexable_range<>
#include <cfr/concept/parallel_range.hpp>           // cfr::ranges::parallel_range<>
#include <cfr/requirement/chunkable_range.hpp>      // cfr::ranges::chunkable_range<>
#include <cfr/requirement/divisible_range.hpp>      // cfr::ranges::divisible_range<>
#include <cfr/requirement/tbb_compatible_range.hpp> // cfr::ranges::tbb_compatible_range<>

namespace cfr::ranges::detail {

struct dispatch_backend_fn {
    
    template <class S, class N, class L, class R, class... Args>
        requires cfr::ranges::parallel_range<R>
    constexpr auto operator()( S serial, N native, L linear, R && range, Args && ... args ) const
        -> auto
    {
        if consteval {
            return std::invoke( serial, std::forward<R>( range ), std::forward<Args>(args)... );
        }
        else {
            constexpr auto compat = cfr::views::tbb_compatible;
            constexpr auto divide = cfr::views::divisible;
            
            if constexpr (cfr::ranges::tbb_compatible_range<R>)
                return std::invoke( native,                 std::forward<R>( range )    , std::forward<Args>(args)... );
            else if constexpr (cfr::ranges::divisible_range<R>)
                return std::invoke( native, compat(         std::forward<R>( range )   ), std::forward<Args>(args)... );
            else if constexpr (cfr::ranges::chunkable_range<R>)
                return std::invoke( linear,                 std::forward<R>( range )    , std::forward<Args>(args)... );
            else if constexpr (cfr::ranges::indexable_range<R>)
                return std::invoke( native, compat( divide( std::forward<R>( range ) ) ), std::forward<Args>(args)... );
            else
                return std::invoke( linear,                 std::forward<R>( range )    , std::forward<Args>(args)... );
        } 
    }  
    
};

inline constexpr auto dispatch_backend = dispatch_backend_fn{};

} // namespace cfr::ranges::detail

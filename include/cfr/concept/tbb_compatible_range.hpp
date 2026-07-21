// SPDX-FileCopyrightText: Copyright (c) 2026 - present, Dmitri Bogdanov
// SPDX-FileCopyrightText: https://github.com/DmitriBogdanov/concurrent-forward-ranges
//
// SPDX-License-Identifier: MIT

#pragma once

// Content: Concepts for ranges compatible with TBB [req.range] named requirements

#include <concepts>    // std::constructible_from<>, std::convertible_to<>
#include <type_traits> // std::remove_cvref_t<>, std::add_lvalue_reference_t<>

#include <tbb/blocked_range.h> // tbb::split

namespace cfr::ranges {

template <class R>
concept tbb_compatible_range = requires ( R && range ) {
    requires std::constructible_from<
        std::remove_cvref_t<R>, std::add_lvalue_reference_t<std::remove_cvref_t<R>>, tbb::split
    >;

    { range.empty       () } -> std::convertible_to<bool>;
    { range.is_divisible() } -> std::convertible_to<bool>;
};

} // namespace cfr::ranges

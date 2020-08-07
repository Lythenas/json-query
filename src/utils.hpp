#ifndef JSONQUERY_UTILS_HPP
#define JSONQUERY_UTILS_HPP

#include <concepts>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

/**
 * Checks if the first parameter `Check` is `std::same_as` one of the following
 * (where `Fixed` is the second parameter):
 *
 * - `Fixed`
 * - `const Fixed`
 * - `Fixed&`
 * - `const Fixed&`
 */
// clang-format off
template<typename Check, typename Fixed>
concept basically_same_as =
    std::same_as<Check, Fixed> ||
    std::same_as<Check, const Fixed> ||
    std::same_as<Check, Fixed&> ||
    std::same_as<Check, const Fixed&>;
// clang-format on

#endif

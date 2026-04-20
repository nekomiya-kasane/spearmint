#pragma once

/**
 * @file string_utils.h
 * @brief Constexpr string utilities for compile-time operations.
 */

#include <cstddef>
#include <cstdint>

namespace spearmint::detail {

constexpr bool str_eq(const char *a, const char *b) noexcept {
    if (a == nullptr || b == nullptr) return a == b;
    while (*a && *b) {
        if (*a != *b) return false;
        ++a;
        ++b;
    }
    return *a == *b;
}

constexpr std::size_t str_len(const char *s) noexcept {
    if (s == nullptr) return 0;
    std::size_t n = 0;
    while (s[n]) ++n;
    return n;
}

constexpr bool str_starts_with(const char *s, std::size_t slen, const char *prefix, std::size_t plen) noexcept {
    if (slen < plen) return false;
    for (std::size_t i = 0; i < plen; ++i)
        if (s[i] != prefix[i]) return false;
    return true;
}

} // namespace spearmint::detail

#pragma once

/**
 * @file constexpr_map.h
 * @brief Compile-time sorted lookup table with constexpr binary search.
 *
 * Usage:
 *   constexpr auto map = spearmint::detail::make_constexpr_map<int, const char*>({{
 *       {1, "one"}, {2, "two"}, {3, "three"},
 *   }});
 *   static_assert(map.at(2) != nullptr);
 *   static_assert(map.contains(3));
 */

#include <array>
#include <cstddef>
#include <utility>

namespace spearmint::detail {

    /**
     * @brief A compile-time sorted associative array with binary search lookup.
     *
     * Keys must support operator<. The array must be sorted by key at construction.
     */
    template <typename Key, typename Value, std::size_t N> struct constexpr_map {
        std::array<std::pair<Key, Value>, N> data{};

        /**
         * @brief Find a value by key. Returns nullptr if not found.
         */
        [[nodiscard]] constexpr const Value *find(const Key &key) const noexcept {
            std::size_t lo = 0;
            std::size_t hi = N;
            while (lo < hi) {
                std::size_t mid = lo + (hi - lo) / 2;
                if (data[mid].first < key) {
                    lo = mid + 1;
                } else if (key < data[mid].first) {
                    hi = mid;
                } else {
                    return &data[mid].second;
                }
            }
            return nullptr;
        }

        /**
         * @brief Check if a key exists.
         */
        [[nodiscard]] constexpr bool contains(const Key &key) const noexcept { return find(key) != nullptr; }

        /**
         * @brief Get value by key. UB if key not found — use only when certain.
         */
        [[nodiscard]] constexpr const Value &at(const Key &key) const noexcept {
            const Value *v = find(key);
            return *v;
        }

        /**
         * @brief Number of entries.
         */
        [[nodiscard]] constexpr std::size_t size() const noexcept { return N; }
    };

    /**
     * @brief Constexpr insertion sort for building sorted maps.
     */
    template <typename Key, typename Value, std::size_t N>
    consteval constexpr_map<Key, Value, N> make_sorted_map(std::array<std::pair<Key, Value>, N> arr) {
        // Insertion sort
        for (std::size_t i = 1; i < N; ++i) {
            auto tmp = arr[i];
            std::size_t j = i;
            while (j > 0 && tmp.first < arr[j - 1].first) {
                arr[j] = arr[j - 1];
                --j;
            }
            arr[j] = tmp;
        }
        return {arr};
    }

} // namespace spearmint::detail

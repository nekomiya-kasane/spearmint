#pragma once

/**
 * @file filter.h
 * @brief Token stream filter base and composable filter pipeline.
 *
 * Filters transform a token_stream in-place or produce a new one.
 * They can be composed via operator| for pipeline syntax.
 */

#include <functional>
#include <vector>

#include "spearmint/core/token_stream.h"

namespace spearmint {

/**
 * @brief A filter is a function that transforms a token_stream.
 */
using token_filter = std::function<token_stream(const token_stream&)>;

/**
 * @brief Compose two filters into a pipeline: (f | g)(ts) == g(f(ts)).
 */
inline token_filter operator|(token_filter lhs, token_filter rhs) {
    return [l = std::move(lhs), r = std::move(rhs)](const token_stream& ts) {
        return r(l(ts));
    };
}

/**
 * @brief Apply a filter to a token_stream.
 */
inline token_stream operator|(const token_stream& ts, const token_filter& f) {
    return f(ts);
}

}  // namespace spearmint

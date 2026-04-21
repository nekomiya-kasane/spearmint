#pragma once

/**
 * @file builtin_filters.h
 * @brief Built-in token stream filters.
 */

#include "spearmint/exports.h"
#include "spearmint/filters/filter.h"

namespace spearmint::filters {

    /**
     * @brief Merge consecutive tokens of the same type into one.
     */
    SPEARMINT_API token_filter merge_consecutive();

    /**
     * @brief Remove all whitespace-only tokens.
     */
    SPEARMINT_API token_filter strip_whitespace();

    /**
     * @brief Remove tokens matching a specific type.
     */
    SPEARMINT_API token_filter remove_token(token_type type);

    /**
     * @brief Keep only tokens matching specific types.
     */
    SPEARMINT_API token_filter keep_only(std::vector<token_type> types);

    /**
     * @brief Remap a token type to another.
     */
    SPEARMINT_API token_filter remap_token(token_type from, token_type to);

    /**
     * @brief Replace token type based on a predicate.
     */
    SPEARMINT_API token_filter remap_if(std::function<bool(const token_entry &)> pred, token_type new_type);

    /**
     * @brief Trim leading and trailing whitespace tokens from the stream.
     */
    SPEARMINT_API token_filter trim();

    /**
     * @brief Collapse runs of whitespace tokens into a single space.
     */
    SPEARMINT_API token_filter normalize_whitespace();

} // namespace spearmint::filters

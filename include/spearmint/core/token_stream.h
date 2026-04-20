#pragma once

/**
 * @file token_stream.h
 * @brief Token entry and token stream types for semantic tokenization output.
 */

#include "spearmint/core/token.h"

#include <string>
#include <string_view>
#include <vector>

namespace spearmint {

/**
 * @brief A single token: a semantic type paired with its source text.
 */
struct token_entry {
    token_type type;
    std::string_view text;

    [[nodiscard]] constexpr bool operator==(const token_entry &o) const noexcept {
        return type == o.type && text == o.text;
    }
};

/**
 * @brief A sequence of semantic tokens produced by a lexer.
 *
 * When constructed manually (e.g. in tests), views point into user-owned strings.
 */
using token_stream = std::vector<token_entry>;

/**
 * @brief Owning tokenization result: holds the source string and token views into it.
 *
 * Returned by lexer::tokenize(). The source string is kept alive so that
 * string_view entries remain valid for the lifetime of this object.
 */
struct tokenize_result {
    std::string source;
    token_stream tokens;

    [[nodiscard]] auto begin() const noexcept { return tokens.begin(); }
    [[nodiscard]] auto end() const noexcept { return tokens.end(); }
    [[nodiscard]] std::size_t size() const noexcept { return tokens.size(); }
    [[nodiscard]] bool empty() const noexcept { return tokens.empty(); }
    [[nodiscard]] const token_entry &operator[](std::size_t i) const { return tokens[i]; }
};

} // namespace spearmint

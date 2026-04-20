#pragma once

/**
 * @file ftl.h
 * @brief Fluent Translation List (.ftl) lexer.
 *
 * Provides syntax highlighting for Mozilla's Project Fluent localization format.
 * Recognizes messages, terms, attributes, placeables, selectors, comments,
 * and function calls.
 *
 * @see https://projectfluent.org/fluent/guide/
 */

#include "spearmint/core/regex_lexer.h"
#include "spearmint/core/lexer_registry.h"

namespace spearmint::lexers {

class SPEARMINT_API ftl_lexer : public regex_lexer {
public:
    [[nodiscard]] const lexer_info& info() const noexcept override;
    [[nodiscard]] float analyse_text(std::string_view source) const noexcept override;

protected:
    [[nodiscard]] state_map get_rules() const override;
};

SPEARMINT_API void register_ftl_lexer();

}  // namespace spearmint::lexers

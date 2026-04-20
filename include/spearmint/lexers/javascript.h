#pragma once

/**
 * @file javascript.h
 * @brief JavaScript / ECMAScript lexer.
 */

#include "spearmint/core/regex_lexer.h"
#include "spearmint/core/lexer_registry.h"

namespace spearmint::lexers {

class SPEARMINT_API javascript_lexer : public regex_lexer {
public:
    [[nodiscard]] const lexer_info& info() const noexcept override;
    [[nodiscard]] float analyse_text(std::string_view source) const noexcept override;

protected:
    [[nodiscard]] state_map get_rules() const override;
};

SPEARMINT_API void register_javascript_lexer();

}  // namespace spearmint::lexers

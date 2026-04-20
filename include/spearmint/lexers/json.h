#pragma once

/**
 * @file json.h
 * @brief JSON lexer.
 */

#include "spearmint/core/lexer_registry.h"
#include "spearmint/core/regex_lexer.h"

namespace spearmint::lexers {

class SPEARMINT_API json_lexer : public regex_lexer {
  public:
    [[nodiscard]] const lexer_info &info() const noexcept override;

  protected:
    [[nodiscard]] state_map get_rules() const override;
};

SPEARMINT_API void register_json_lexer();

} // namespace spearmint::lexers

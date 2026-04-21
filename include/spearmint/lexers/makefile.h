#pragma once
#include "spearmint/core/lexer_registry.h"
#include "spearmint/core/regex_lexer.h"
namespace spearmint::lexers {
    class SPEARMINT_API makefile_lexer : public regex_lexer {
      public:
        [[nodiscard]] const lexer_info &info() const noexcept override;
        [[nodiscard]] float analyse_text(std::string_view source) const noexcept override;

      protected:
        [[nodiscard]] state_map get_rules() const override;
    };
    SPEARMINT_API void register_makefile_lexer();
} // namespace spearmint::lexers

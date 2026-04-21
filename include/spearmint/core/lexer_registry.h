#pragma once

/**
 * @file lexer_registry.h
 * @brief Runtime lexer registry for discovery by name, alias, filename, or mime type.
 */

#include "spearmint/core/lexer.h"
#include "spearmint/exports.h"

#include <memory>
#include <string_view>
#include <vector>

namespace spearmint {

    SPEARMINT_API void register_lexer(lexer_factory factory, const lexer_info &info);

    SPEARMINT_API std::unique_ptr<lexer> get_lexer_by_name(std::string_view name);
    SPEARMINT_API std::unique_ptr<lexer> get_lexer_by_filename(std::string_view filename);
    SPEARMINT_API std::unique_ptr<lexer> get_lexer_by_mime(std::string_view mime);
    SPEARMINT_API std::unique_ptr<lexer> guess_lexer(std::string_view source);

    SPEARMINT_API std::vector<const lexer_info *> get_all_lexers();

} // namespace spearmint

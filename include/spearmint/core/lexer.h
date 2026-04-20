#pragma once

/**
 * @file lexer.h
 * @brief Abstract lexer base class and lexer metadata.
 *
 * All lexers inherit from `lexer` and implement `tokenize()`.
 * `lexer_info` provides metadata for discovery and registration.
 */

#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/exports.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace spearmint {

// ── Lexer metadata ─────────────────────────────────────────────────────

struct lexer_info {
    const char *name = "";
    const char *display_name = "";
    std::span<const char *const> aliases;
    std::span<const char *const> filenames; ///< glob patterns, e.g. "*.py"
    std::span<const char *const> mime_types;
    const char *url = "";
    int priority = 0; ///< higher = preferred
};

// ── Lexer options ──────────────────────────────────────────────────────

struct lexer_options {
    bool strip_nl = true;
    bool strip_all = false;
    bool ensure_nl = true;
    int tab_size = 0; ///< 0 = no tab expansion
};

// ── Abstract lexer ─────────────────────────────────────────────────────

class SPEARMINT_API lexer {
  public:
    virtual ~lexer() = default;

    [[nodiscard]] virtual const lexer_info &info() const noexcept = 0;

    /**
     * @brief Tokenize source code into an owning result.
     */
    [[nodiscard]] virtual tokenize_result tokenize(std::string_view source) const = 0;

    /**
     * @brief Score how well this lexer matches the given source (0.0–1.0).
     *
     * Used for auto-detection. Default returns 0.
     */
    [[nodiscard]] virtual float analyse_text(std::string_view /*source*/) const noexcept { return 0.0f; }

    void set_options(const lexer_options &opts) noexcept { options_ = opts; }
    [[nodiscard]] const lexer_options &options() const noexcept { return options_; }

  protected:
    lexer_options options_;

    /**
     * @brief Apply strip/ensure options to source before tokenizing.
     */
    [[nodiscard]] std::string preprocess(std::string_view source) const;
};

// ── Lexer factory ──────────────────────────────────────────────────────

using lexer_factory = std::function<std::unique_ptr<lexer>()>;

} // namespace spearmint

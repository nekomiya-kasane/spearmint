#pragma once

/**
 * @file style.h
 * @brief Constexpr style rule and style definition types.
 *
 * Styles are metadata — they describe how tokens *should* look,
 * but produce no output themselves. Exporters consume style_def_view
 * to render tokens with appropriate visual attributes.
 *
 * Usage:
 *   using namespace spearmint;
 *   constexpr style_rule kw_rule{0xF92672, 0, true};  // magenta bold
 *   static_assert(kw_rule.fg == 0xF92672);
 */

#include "spearmint/core/token.h"
#include "spearmint/exports.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace spearmint {

    // ── Style rule ─────────────────────────────────────────────────────────

    /**
     * @brief Visual attributes for a single token type.
     *
     * Colors are stored as 0xRRGGBB. A value of 0 means "default/inherit".
     * Use has_fg/has_bg to distinguish "black" (0x000000 with flag) from "default".
     */
    struct style_rule {
        uint32_t fg = 0;
        uint32_t bg = 0;
        bool bold = false;
        bool italic = false;
        bool underline = false;
        bool strike = false;
        bool nobold = false;
        bool noitalic = false;
        bool border = false;
        bool has_fg = false;
        bool has_bg = false;

        [[nodiscard]] constexpr bool operator==(const style_rule &) const noexcept = default;

        /**
         * @brief Create a rule with foreground color.
         */
        [[nodiscard]] static constexpr style_rule with_fg(uint32_t color) noexcept {
            style_rule r{};
            r.fg = color;
            r.has_fg = true;
            return r;
        }
    };

    // ── Token-to-rule mapping entry ────────────────────────────────────────

    struct style_entry {
        token_type token;
        style_rule rule;
    };

    // ── Static style definition (constexpr, compile-time) ──────────────────

    /**
     * @brief A complete style definition with a fixed number of rules.
     *
     * Used for builtin styles defined as constexpr globals.
     */
    template <std::size_t N> struct static_style_def {
        const char *name = "";
        const char *display_name = "";
        uint32_t background_color = 0;
        bool has_background = false;
        uint32_t highlight_color = 0;
        bool has_highlight = false;
        uint32_t line_number_fg = 0;
        uint32_t line_number_bg = 0;
        std::array<style_entry, N> entries{};

        /**
         * @brief Look up a style rule for a token type.
         *
         * Walks up the token parent chain until a match is found.
         * Returns nullptr if no rule matches.
         */
        [[nodiscard]] constexpr const style_rule *lookup(token_type t) const noexcept {
            // Try exact match first, then walk parents
            for (int depth = 0; depth < 8; ++depth) {
                for (std::size_t i = 0; i < N; ++i) {
                    if (entries[i].token == t) {
                        return &entries[i].rule;
                    }
                }
                // Walk to parent
                if (t.id >= detail::token_count) {
                    return nullptr;
                }
                uint32_t pid = detail::token_table[t.id].parent_id;
                if (pid == t.id) {
                    return nullptr; // reached root with no match
                }
                t = detail::token_table[pid];
            }
            return nullptr;
        }

        [[nodiscard]] constexpr std::size_t size() const noexcept { return N; }
    };

    // ── Type-erased runtime style view ─────────────────────────────────────

    /**
     * @brief Non-owning view into a style definition for runtime use.
     *
     * Can be constructed from any static_style_def<N>.
     */
    struct style_def_view {
        std::string_view name;
        std::string_view display_name;
        uint32_t background_color = 0;
        bool has_background = false;
        uint32_t highlight_color = 0;
        bool has_highlight = false;
        uint32_t line_number_fg = 0;
        uint32_t line_number_bg = 0;
        std::span<const style_entry> entries;

        /**
         * @brief Look up a style rule for a token type with parent fallback.
         */
        [[nodiscard]] SPEARMINT_API const style_rule *lookup(token_type t) const noexcept;

        /**
         * @brief Construct from a static_style_def.
         */
        template <std::size_t N>
        constexpr style_def_view(const static_style_def<N> &def) noexcept
            : name(def.name), display_name(def.display_name), background_color(def.background_color),
              has_background(def.has_background), highlight_color(def.highlight_color),
              has_highlight(def.has_highlight), line_number_fg(def.line_number_fg), line_number_bg(def.line_number_bg),
              entries(def.entries.data(), def.entries.size()) {}

        constexpr style_def_view() noexcept = default;
    };

} // namespace spearmint

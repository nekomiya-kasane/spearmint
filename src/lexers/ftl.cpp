/**
 * @file ftl.cpp
 * @brief Fluent Translation List (.ftl) lexer implementation.
 *
 * Fluent syntax reference: https://projectfluent.org/fluent/guide/
 *
 * Key constructs:
 *   - Messages:    `hello = Hello, world!`
 *   - Terms:       `-brand = Firefox`
 *   - Attributes:  `.label = Click me`
 *   - Placeables:  `{ $var }`, `{ -term }`, `{ FUNC($arg) }`
 *   - Selectors:   `{ $num -> \n [one] item \n *[other] items }`
 *   - Comments:    `# comment`, `## group`, `### resource`
 */

#include "spearmint/lexers/ftl.h"

namespace spearmint::lexers {

namespace {

constexpr const char* aliases[] = {"ftl", "fluent"};
constexpr const char* filenames[] = {"*.ftl"};
constexpr const char* mimes[] = {"text/x-fluent"};

const lexer_info ftl_info = {
    "ftl", "Fluent (FTL)",
    {aliases}, {filenames}, {mimes},
    "https://projectfluent.org", 10,
};

}  // namespace

const lexer_info& ftl_lexer::info() const noexcept { return ftl_info; }

float ftl_lexer::analyse_text(std::string_view source) const noexcept {
    float score = 0.0f;
    // Terms start with `-`
    if (source.find("\n-") != std::string_view::npos) score += 0.1f;
    // Placeables use `{ $var }` or `{ -term }`
    if (source.find("{ $") != std::string_view::npos) score += 0.2f;
    if (source.find("{ -") != std::string_view::npos) score += 0.15f;
    // Selectors use `->` inside placeables
    if (source.find(" ->") != std::string_view::npos) score += 0.1f;
    // Variant keys `[one]`, `*[other]`
    if (source.find("*[") != std::string_view::npos) score += 0.15f;
    // Resource comments `###`
    if (source.find("###") != std::string_view::npos) score += 0.1f;
    // Attributes `.attr =`
    if (source.find("\n    .") != std::string_view::npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map ftl_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;

    // ── Root state ──────────────────────────────────────────────────
    rules["root"] = {
        // Blank lines
        {R"(\n)", tk::whitespace, state_action::none()},
        {R"([ \t]+)", tk::whitespace, state_action::none()},

        // Resource comment (###)
        {R"(###[^\n]*)", tk::comment::special, state_action::none()},
        // Group comment (##)
        {R"(##[^\n]*)", tk::comment::special, state_action::none()},
        // Regular comment (#)
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},

        // Term definition: -identifier =
        {R"(-[a-zA-Z][a-zA-Z0-9_-]*)", tk::name::function, state_action::none()},

        // Attribute: .identifier =
        {R"(\.[a-zA-Z][a-zA-Z0-9_-]*)", tk::name::attribute, state_action::none()},

        // Message identifier (at start of line, before =)
        {R"([a-zA-Z][a-zA-Z0-9_-]*)", tk::name::variable, state_action::none()},

        // Assignment operator
        {R"(=)", tk::operator_::self, state_action::none()},

        // Placeable open
        {R"(\{)", tk::punctuation::self, state_action::push_state("placeable")},

        // String content (anything else on a line)
        {R"([^\n\{\}]+)", tk::literal::string::self, state_action::none()},
    };

    // ── Placeable state { ... } ─────────────────────────────────────
    rules["placeable"] = {
        {R"([ \t]+)", tk::whitespace, state_action::none()},
        {R"(\n)", tk::whitespace, state_action::none()},

        // Close placeable
        {R"(\})", tk::punctuation::self, state_action::pop_state()},

        // Selector arrow
        {R"(->)", tk::operator_::self, state_action::none()},

        // Variable reference: $name
        {R"(\$[a-zA-Z][a-zA-Z0-9_-]*)", tk::name::variable, state_action::none()},

        // Term reference: -name
        {R"(-[a-zA-Z][a-zA-Z0-9_-]*)", tk::name::function, state_action::none()},

        // Function call: FUNC(
        {R"([A-Z][A-Z0-9_-]*(?=\())", tk::name::builtin, state_action::none()},

        // Variant key: [key] or *[key]
        {R"(\*?\[)", tk::punctuation::self, state_action::push_state("variant_key")},

        // Nested placeable
        {R"(\{)", tk::punctuation::self, state_action::push_state("placeable")},

        // Parentheses (function args)
        {R"(\()", tk::punctuation::self, state_action::push_state("call_args")},

        // Number literal
        {R"(-?\d+(?:\.\d+)?)", tk::literal::number::float_, state_action::none()},

        // Named argument: name:
        {R"([a-zA-Z][a-zA-Z0-9_-]*(?=\s*:))", tk::name::attribute, state_action::none()},

        // Colon (named arg separator)
        {R"(:)", tk::punctuation::self, state_action::none()},

        // String literal in placeable
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},

        // Attribute access: .attr
        {R"(\.[a-zA-Z][a-zA-Z0-9_-]*)", tk::name::attribute, state_action::none()},

        // Comma
        {R"(,)", tk::punctuation::self, state_action::none()},

        // Any other text inside placeable
        {R"([^\s\{\}\[\]\(\)\",:>$\-.]+)", tk::literal::string::self, state_action::none()},
    };

    // ── Variant key state [key] ─────────────────────────────────────
    rules["variant_key"] = {
        {R"([ \t]+)", tk::whitespace, state_action::none()},
        {R"(\])", tk::punctuation::self, state_action::pop_state()},
        // Identifier or number inside variant key
        {R"(-?\d+(?:\.\d+)?)", tk::literal::number::float_, state_action::none()},
        {R"([a-zA-Z][a-zA-Z0-9_-]*)", tk::keyword::self, state_action::none()},
    };

    // ── Function call arguments ─────────────────────────────────────
    rules["call_args"] = {
        {R"([ \t]+)", tk::whitespace, state_action::none()},
        {R"(\n)", tk::whitespace, state_action::none()},
        {R"(\))", tk::punctuation::self, state_action::pop_state()},

        // Variable reference
        {R"(\$[a-zA-Z][a-zA-Z0-9_-]*)", tk::name::variable, state_action::none()},

        // Term reference
        {R"(-[a-zA-Z][a-zA-Z0-9_-]*)", tk::name::function, state_action::none()},

        // Named argument: name:
        {R"([a-zA-Z][a-zA-Z0-9_-]*(?=\s*:))", tk::name::attribute, state_action::none()},
        {R"(:)", tk::punctuation::self, state_action::none()},

        // String literal
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},

        // Number
        {R"(-?\d+(?:\.\d+)?)", tk::literal::number::float_, state_action::none()},

        // Comma
        {R"(,)", tk::punctuation::self, state_action::none()},

        // Nested placeable
        {R"(\{)", tk::punctuation::self, state_action::push_state("placeable")},
    };

    // ── String literal state "..." ──────────────────────────────────
    rules["string"] = {
        {R"(\\[\\"])", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\U[0-9a-fA-F]{6})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        // Placeable inside string
        {R"(\{)", tk::punctuation::self, state_action::push_state("placeable")},
        {R"([^"\\{]+)", tk::literal::string::double_, state_action::none()},
    };

    return rules;
}

SPEARMINT_API void register_ftl_lexer() {
    register_lexer(
        []() -> std::unique_ptr<lexer> { return std::make_unique<ftl_lexer>(); },
        ftl_info
    );
}

}  // namespace spearmint::lexers

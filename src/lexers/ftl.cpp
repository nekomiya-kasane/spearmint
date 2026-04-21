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

constexpr const char *aliases[] = {"ftl", "fluent"};
constexpr const char *filenames[] = {"*.ftl"};
constexpr const char *mimes[] = {"text/x-fluent"};

const lexer_info ftl_info = {
    .name = "ftl",
    .display_name = "Fluent (FTL)",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://projectfluent.org",
    .priority = 10,
};

} // namespace

const lexer_info &ftl_lexer::info() const noexcept {
    return ftl_info;
}

float ftl_lexer::analyse_text(std::string_view source) const noexcept {
    float score = 0.0f;
    // Terms start with `-`
    if (source.find("\n-") != std::string_view::npos) {
        score += 0.1f;
    }
    // Placeables use `{ $var }` or `{ -term }`
    if (source.find("{ $") != std::string_view::npos) {
        score += 0.2f;
    }
    if (source.find("{ -") != std::string_view::npos) {
        score += 0.15f;
    }
    // Selectors use `->` inside placeables
    if (source.find(" ->") != std::string_view::npos) {
        score += 0.1f;
    }
    // Variant keys `[one]`, `*[other]`
    if (source.find("*[") != std::string_view::npos) {
        score += 0.15f;
    }
    // Resource comments `###`
    if (source.find("###") != std::string_view::npos) {
        score += 0.1f;
    }
    // Attributes `.attr =`
    if (source.find("\n    .") != std::string_view::npos) {
        score += 0.1f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map ftl_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;

    // ── Root state ──────────────────────────────────────────────────
    rules["root"] = {
        // Blank lines
        {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([ \t]+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},

        // Resource comment (###)
        {.pattern = R"(###[^\n]*)", .token = tk::comment::special, .action = state_action::none(), .group_tokens = {}},
        // Group comment (##)
        {.pattern = R"(##[^\n]*)", .token = tk::comment::special, .action = state_action::none(), .group_tokens = {}},
        // Regular comment (#)
        {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},

        // Term definition: -identifier =
        {.pattern = R"(-[a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::name::function,
         .action = state_action::none(),
         .group_tokens = {}},

        // Attribute: .identifier =
        {.pattern = R"(\.[a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},

        // Message identifier (at start of line, before =)
        {.pattern = R"([a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::name::variable,
         .action = state_action::none(),
         .group_tokens = {}},

        // Assignment operator
        {.pattern = R"(=)", .token = tk::operator_::self, .action = state_action::none(), .group_tokens = {}},

        // Placeable open
        {.pattern = R"(\{)",
         .token = tk::punctuation::self,
         .action = state_action::push_state("placeable"),
         .group_tokens = {}},

        // String content (anything else on a line)
        {.pattern = R"([^\n\{\}]+)", .token = tk::literal::string::self, .action = state_action::none()},
    };

    // ── Placeable state { ... } ─────────────────────────────────────
    rules["placeable"] = {
        {.pattern = R"([ \t]+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},

        // Close placeable
        {.pattern = R"(\})", .token = tk::punctuation::self, .action = state_action::pop_state(), .group_tokens = {}},

        // Selector arrow
        {.pattern = R"(->)", .token = tk::operator_::self, .action = state_action::none(), .group_tokens = {}},

        // Variable reference: $name
        {.pattern = R"(\$[a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::name::variable,
         .action = state_action::none(),
         .group_tokens = {}},

        // Term reference: -name
        {.pattern = R"(-[a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::name::function,
         .action = state_action::none(),
         .group_tokens = {}},

        // Function call: FUNC(
        {.pattern = R"([A-Z][A-Z0-9_-]*(?=\())",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},

        // Variant key: [key] or *[key]
        {.pattern = R"(\*?\[)",
         .token = tk::punctuation::self,
         .action = state_action::push_state("variant_key"),
         .group_tokens = {}},

        // Nested placeable
        {.pattern = R"(\{)",
         .token = tk::punctuation::self,
         .action = state_action::push_state("placeable"),
         .group_tokens = {}},

        // Parentheses (function args)
        {.pattern = R"(\()",
         .token = tk::punctuation::self,
         .action = state_action::push_state("call_args"),
         .group_tokens = {}},

        // Number literal
        {.pattern = R"(-?\d+(?:\.\d+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},

        // Named argument: name:
        {.pattern = R"([a-zA-Z][a-zA-Z0-9_-]*(?=\s*:))",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},

        // Colon (named arg separator)
        {.pattern = R"(:)", .token = tk::punctuation::self, .action = state_action::none(), .group_tokens = {}},

        // String literal in placeable
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("string"),
         .group_tokens = {}},

        // Attribute access: .attr
        {.pattern = R"(\.[a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},

        // Comma
        {.pattern = R"(,)", .token = tk::punctuation::self, .action = state_action::none(), .group_tokens = {}},

        // Any other text inside placeable
        {.pattern = R"([^\s\{\}\[\]\(\)\",:>$\-.]+)",
         .token = tk::literal::string::self,
         .action = state_action::none(),
         .group_tokens = {}},
    };

    // ── Variant key state [key] ─────────────────────────────────────
    rules["variant_key"] = {
        {.pattern = R"([ \t]+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\])", .token = tk::punctuation::self, .action = state_action::pop_state(), .group_tokens = {}},
        // Identifier or number inside variant key
        {.pattern = R"(-?\d+(?:\.\d+)?)", .token = tk::literal::number::float_, .action = state_action::none()},
        {.pattern = R"([a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
    };

    // ── Function call arguments ─────────────────────────────────────
    rules["call_args"] = {
        {.pattern = R"([ \t]+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\))", .token = tk::punctuation::self, .action = state_action::pop_state(), .group_tokens = {}},

        // Variable reference
        {.pattern = R"(\$[a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::name::variable,
         .action = state_action::none(),
         .group_tokens = {}},

        // Term reference
        {.pattern = R"(-[a-zA-Z][a-zA-Z0-9_-]*)",
         .token = tk::name::function,
         .action = state_action::none(),
         .group_tokens = {}},

        // Named argument: name:
        {.pattern = R"([a-zA-Z][a-zA-Z0-9_-]*(?=\s*:))",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(:)", .token = tk::punctuation::self, .action = state_action::none(), .group_tokens = {}},

        // String literal
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("string"),
         .group_tokens = {}},

        // Number
        {.pattern = R"(-?\d+(?:\.\d+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},

        // Comma
        {.pattern = R"(,)", .token = tk::punctuation::self, .action = state_action::none()},

        // Nested placeable
        {.pattern = R"(\{)",
         .token = tk::punctuation::self,
         .action = state_action::push_state("placeable"),
         .group_tokens = {}},
    };

    // ── String literal state "..." ──────────────────────────────────
    rules["string"] = {
        {.pattern = R"(\\[\\"])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\u[0-9a-fA-F]{4})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\U[0-9a-fA-F]{6})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        // Placeable inside string
        {.pattern = R"(\{)",
         .token = tk::punctuation::self,
         .action = state_action::push_state("placeable"),
         .group_tokens = {}},
        {.pattern = R"([^"\\{]+)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
    };

    return rules;
}

SPEARMINT_API void register_ftl_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<ftl_lexer>(); }, ftl_info);
}

} // namespace spearmint::lexers

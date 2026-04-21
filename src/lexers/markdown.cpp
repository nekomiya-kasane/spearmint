#include "spearmint/lexers/markdown.h"

namespace spearmint::lexers {

    namespace {
        constexpr const char *aliases[] = {"markdown", "md"};
        constexpr const char *filenames[] = {"*.md", "*.markdown", "*.mkd"};
        constexpr const char *mimes[] = {"text/markdown"};
        const lexer_info md_info = {
            .name = "markdown",
            .display_name = "Markdown",
            .aliases = {aliases},
            .filenames = {filenames},
            .mime_types = {mimes},
            .url = "https://commonmark.org",
            .priority = 10,
        };
    } // namespace

    const lexer_info &markdown_lexer::info() const noexcept {
        return md_info;
    }

    float markdown_lexer::analyse_text(std::string_view src) const noexcept {
        float score = 0.0f;
        if (src.find("# ") != src.npos) {
            score += 0.1f;
        }
        if (src.find("## ") != src.npos) {
            score += 0.1f;
        }
        if (src.find("```") != src.npos) {
            score += 0.2f;
        }
        if (src.find("**") != src.npos) {
            score += 0.1f;
        }
        if (src.find("](") != src.npos) {
            score += 0.1f;
        }
        return score > 1.0f ? 1.0f : score;
    }

    state_map markdown_lexer::get_rules() const {
        namespace tk = token;
        state_map rules;
        rules["root"] = {
            {.pattern = R"(^#{1,6}\s+[^\n]+)",
             .token = tk::generic::heading,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(^[=-]{3,}\s*$)",
             .token = tk::generic::heading,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(^>\s?[^\n]*)",
             .token = tk::generic::emph,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(^```[^\n]*)",
             .token = tk::literal::string::backtick,
             .action = state_action::push_state("fenced"),
             .group_tokens = {}},
            {.pattern = R"(^[ \t]*[-*+]\s)",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(^[ \t]*[0-9]+\.\s)",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\*\*[^*]+\*\*)",
             .token = tk::generic::strong,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(__[^_]+__)",
             .token = tk::generic::strong,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\*[^*]+\*)", .token = tk::generic::emph, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(_[^_]+_)", .token = tk::generic::emph, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(~~[^~]+~~)",
             .token = tk::generic::deleted,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(`[^`]+`)",
             .token = tk::literal::string::backtick,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\[([^\]]+)\]\([^)]+\))",
             .token = tk::name::label,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(!\[([^\]]*)\]\([^)]+\))",
             .token = tk::name::label,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(https?://[^\s<>]+)",
             .token = tk::name::label,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(^---\s*$)",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"([^\n*_`\[!#>\-=~]+)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(.)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
        };
        rules["fenced"] = {
            {.pattern = R"(^```\s*$)",
             .token = tk::literal::string::backtick,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"([^\n]+)",
             .token = tk::literal::string::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        };
        return rules;
    }

    SPEARMINT_API void register_markdown_lexer() {
        register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<markdown_lexer>(); }, md_info);
    }

} // namespace spearmint::lexers

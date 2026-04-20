#include "spearmint/lexers/markdown.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"markdown", "md"};
constexpr const char *filenames[] = {"*.md", "*.markdown", "*.mkd"};
constexpr const char *mimes[] = {"text/markdown"};
const lexer_info md_info = {
    "markdown", "Markdown", {aliases}, {filenames}, {mimes}, "https://commonmark.org", 10,
};
} // namespace

const lexer_info &markdown_lexer::info() const noexcept {
    return md_info;
}

float markdown_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("# ") != src.npos) score += 0.1f;
    if (src.find("## ") != src.npos) score += 0.1f;
    if (src.find("```") != src.npos) score += 0.2f;
    if (src.find("**") != src.npos) score += 0.1f;
    if (src.find("](") != src.npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map markdown_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(^#{1,6}\s+[^\n]+)", tk::generic::heading, state_action::none()},
        {R"(^[=-]{3,}\s*$)", tk::generic::heading, state_action::none()},
        {R"(^>\s?[^\n]*)", tk::generic::emph, state_action::none()},
        {R"(^```[^\n]*)", tk::literal::string::backtick, state_action::push_state("fenced")},
        {R"(^[ \t]*[-*+]\s)", tk::punctuation::self, state_action::none()},
        {R"(^[ \t]*[0-9]+\.\s)", tk::punctuation::self, state_action::none()},
        {R"(\*\*[^*]+\*\*)", tk::generic::strong, state_action::none()},
        {R"(__[^_]+__)", tk::generic::strong, state_action::none()},
        {R"(\*[^*]+\*)", tk::generic::emph, state_action::none()},
        {R"(_[^_]+_)", tk::generic::emph, state_action::none()},
        {R"(~~[^~]+~~)", tk::generic::deleted, state_action::none()},
        {R"(`[^`]+`)", tk::literal::string::backtick, state_action::none()},
        {R"(\[([^\]]+)\]\([^)]+\))", tk::name::label, state_action::none()},
        {R"(!\[([^\]]*)\]\([^)]+\))", tk::name::label, state_action::none()},
        {R"(https?://[^\s<>]+)", tk::name::label, state_action::none()},
        {R"(^---\s*$)", tk::punctuation::self, state_action::none()},
        {R"(\n)", tk::whitespace, state_action::none()},
        {R"([^\n*_`\[!#>\-=~]+)", tk::text, state_action::none()},
        {R"(.)", tk::text, state_action::none()},
    };
    rules["fenced"] = {
        {R"(^```\s*$)", tk::literal::string::backtick, state_action::pop_state()},
        {R"([^\n]+)", tk::literal::string::self, state_action::none()},
        {R"(\n)", tk::whitespace, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_markdown_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<markdown_lexer>(); }, md_info);
}

} // namespace spearmint::lexers

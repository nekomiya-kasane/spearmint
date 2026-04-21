#include "spearmint/lexers/html.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"html", "htm"};
constexpr const char *filenames[] = {"*.html", "*.htm", "*.xhtml"};
constexpr const char *mimes[] = {"text/html"};
const lexer_info html_info = {
    .name = "html",
    .display_name = "HTML",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://html.spec.whatwg.org",
    .priority = 10,
};
} // namespace

const lexer_info &html_lexer::info() const noexcept {
    return html_info;
}

float html_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("<!DOCTYPE") != src.npos || src.find("<!doctype") != src.npos) score += 0.5f;
    if (src.find("<html") != src.npos) score += 0.3f;
    if (src.find("<div") != src.npos || src.find("<span") != src.npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map html_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(<!--)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern = R"(<!DOCTYPE[^>]*>)",
         .token = tk::comment::preproc,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(<script\b)",
         .token = tk::name::tag,
         .action = state_action::push_state("script_tag"),
         .group_tokens = {}},
        {.pattern = R"(<style\b)",
         .token = tk::name::tag,
         .action = state_action::push_state("style_tag"),
         .group_tokens = {}},
        {.pattern = R"(</[a-zA-Z][\w-]*\s*>)",
         .token = tk::name::tag,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(<[a-zA-Z][\w-]*)",
         .token = tk::name::tag,
         .action = state_action::push_state("tag"),
         .group_tokens = {}},
        {.pattern = R"(&[a-zA-Z]+;)", .token = tk::name::entity, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(&#[0-9]+;)", .token = tk::name::entity, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(&#x[0-9a-fA-F]+;)",
         .token = tk::name::entity,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([^<&]+)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([<&])", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
    };
    rules["comment"] = {
        {.pattern = R"(-->)", .token = tk::comment::multiline, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"([^-]+)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(-)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
    };
    rules["tag"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([a-zA-Z][\w-]*)",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(=)", .token = tk::operator_::self, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"("[^"]*")",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"('[^']*')",
         .token = tk::literal::string::single,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(/?>)", .token = tk::name::tag, .action = state_action::pop_state(), .group_tokens = {}},
    };
    rules["script_tag"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([a-zA-Z][\w-]*)",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(=)", .token = tk::operator_::self, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"("[^"]*")",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"('[^']*')",
         .token = tk::literal::string::single,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(>)",
         .token = tk::name::tag,
         .action = state_action::push_state("script_body"),
         .group_tokens = {}},
    };
    rules["script_body"] = {
        {.pattern = R"(</script\s*>)", .token = tk::name::tag, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"([^<]+)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(<)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
    };
    rules["style_tag"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([a-zA-Z][\w-]*)",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(=)", .token = tk::operator_::self, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"("[^"]*")",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"('[^']*')",
         .token = tk::literal::string::single,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(>)",
         .token = tk::name::tag,
         .action = state_action::push_state("style_body"),
         .group_tokens = {}},
    };
    rules["style_body"] = {
        {.pattern = R"(</style\s*>)", .token = tk::name::tag, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"([^<]+)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(<)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
    };
    return rules;
}

SPEARMINT_API void register_html_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<html_lexer>(); }, html_info);
}

} // namespace spearmint::lexers

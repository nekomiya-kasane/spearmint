#include "spearmint/lexers/html.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"html", "htm"};
constexpr const char *filenames[] = {"*.html", "*.htm", "*.xhtml"};
constexpr const char *mimes[] = {"text/html"};
const lexer_info html_info = {
    "html", "HTML", {aliases}, {filenames}, {mimes}, "https://html.spec.whatwg.org", 10,
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
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(<!--)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(<!DOCTYPE[^>]*>)", tk::comment::preproc, state_action::none()},
        {R"(<script\b)", tk::name::tag, state_action::push_state("script_tag")},
        {R"(<style\b)", tk::name::tag, state_action::push_state("style_tag")},
        {R"(</[a-zA-Z][\w-]*\s*>)", tk::name::tag, state_action::none()},
        {R"(<[a-zA-Z][\w-]*)", tk::name::tag, state_action::push_state("tag")},
        {R"(&[a-zA-Z]+;)", tk::name::entity, state_action::none()},
        {R"(&#[0-9]+;)", tk::name::entity, state_action::none()},
        {R"(&#x[0-9a-fA-F]+;)", tk::name::entity, state_action::none()},
        {R"([^<&]+)", tk::text, state_action::none()},
        {R"([<&])", tk::text, state_action::none()},
    };
    rules["comment"] = {
        {R"(-->)", tk::comment::multiline, state_action::pop_state()},
        {R"([^-]+)", tk::comment::multiline, state_action::none()},
        {R"(-)", tk::comment::multiline, state_action::none()},
    };
    rules["tag"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"([a-zA-Z][\w-]*)", tk::name::attribute, state_action::none()},
        {R"(=)", tk::operator_::self, state_action::none()},
        {R"("[^"]*")", tk::literal::string::double_, state_action::none()},
        {R"('[^']*')", tk::literal::string::single, state_action::none()},
        {R"(/?>)", tk::name::tag, state_action::pop_state()},
    };
    rules["script_tag"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"([a-zA-Z][\w-]*)", tk::name::attribute, state_action::none()},
        {R"(=)", tk::operator_::self, state_action::none()},
        {R"("[^"]*")", tk::literal::string::double_, state_action::none()},
        {R"('[^']*')", tk::literal::string::single, state_action::none()},
        {R"(>)", tk::name::tag, state_action::push_state("script_body")},
    };
    rules["script_body"] = {
        {R"(</script\s*>)", tk::name::tag, state_action::pop_state()},
        {R"([^<]+)", tk::text, state_action::none()},
        {R"(<)", tk::text, state_action::none()},
    };
    rules["style_tag"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"([a-zA-Z][\w-]*)", tk::name::attribute, state_action::none()},
        {R"(=)", tk::operator_::self, state_action::none()},
        {R"("[^"]*")", tk::literal::string::double_, state_action::none()},
        {R"('[^']*')", tk::literal::string::single, state_action::none()},
        {R"(>)", tk::name::tag, state_action::push_state("style_body")},
    };
    rules["style_body"] = {
        {R"(</style\s*>)", tk::name::tag, state_action::pop_state()},
        {R"([^<]+)", tk::text, state_action::none()},
        {R"(<)", tk::text, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_html_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<html_lexer>(); }, html_info);
}

} // namespace spearmint::lexers

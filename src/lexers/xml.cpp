#include "spearmint/lexers/xml.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"xml", "xsl", "xslt", "svg_xml"};
constexpr const char *filenames[] = {"*.xml", "*.xsl", "*.xslt", "*.rss", "*.atom", "*.plist", "*.svg"};
constexpr const char *mimes[] = {"text/xml", "application/xml"};
const lexer_info xml_info = {
    "xml", "XML", {aliases}, {filenames}, {mimes}, "https://www.w3.org/XML/", 10,
};
} // namespace

const lexer_info &xml_lexer::info() const noexcept {
    return xml_info;
}

float xml_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("<?xml") != src.npos) score += 0.5f;
    if (src.find("xmlns") != src.npos) score += 0.2f;
    if (src.find("</") != src.npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map xml_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(<!--)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(<!\[CDATA\[)", tk::comment::preproc, state_action::push_state("cdata")},
        {R"(<!DOCTYPE[^>]*>)", tk::comment::preproc, state_action::none()},
        {R"(<\?xml[^?]*\?>)", tk::comment::preproc, state_action::none()},
        {R"(<\?[^?]*\?>)", tk::comment::preproc, state_action::none()},
        {R"(</[a-zA-Z_][\w:.-]*\s*>)", tk::name::tag, state_action::none()},
        {R"(<[a-zA-Z_][\w:.-]*)", tk::name::tag, state_action::push_state("tag")},
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
    rules["cdata"] = {
        {R"(\]\]>)", tk::comment::preproc, state_action::pop_state()},
        {R"([^\]]+)", tk::text, state_action::none()},
        {R"(\])", tk::text, state_action::none()},
    };
    rules["tag"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"([a-zA-Z_][\w:.-]*)", tk::name::attribute, state_action::none()},
        {R"(=)", tk::operator_::self, state_action::none()},
        {R"("[^"]*")", tk::literal::string::double_, state_action::none()},
        {R"('[^']*')", tk::literal::string::single, state_action::none()},
        {R"(/?>)", tk::name::tag, state_action::pop_state()},
    };
    return rules;
}

SPEARMINT_API void register_xml_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<xml_lexer>(); }, xml_info);
}

} // namespace spearmint::lexers

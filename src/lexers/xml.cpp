#include "spearmint/lexers/xml.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"xml", "xsl", "xslt", "svg_xml"};
constexpr const char *filenames[] = {"*.xml", "*.xsl", "*.xslt", "*.rss", "*.atom", "*.plist", "*.svg"};
constexpr const char *mimes[] = {"text/xml", "application/xml"};
const lexer_info xml_info = {
    .name = "xml",
    .display_name = "XML",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://www.w3.org/XML/",
    .priority = 10,
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
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(<!--)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern = R"(<!\[CDATA\[)",
         .token = tk::comment::preproc,
         .action = state_action::push_state("cdata"),
         .group_tokens = {}},
        {.pattern = R"(<!DOCTYPE[^>]*>)",
         .token = tk::comment::preproc,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(<\?xml[^?]*\?>)",
         .token = tk::comment::preproc,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(<\?[^?]*\?>)",
         .token = tk::comment::preproc,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(</[a-zA-Z_][\w:.-]*\s*>)",
         .token = tk::name::tag,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(<[a-zA-Z_][\w:.-]*)",
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
    rules["cdata"] = {
        {.pattern = R"(\]\]>)", .token = tk::comment::preproc, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"([^\]]+)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\])", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
    };
    rules["tag"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([a-zA-Z_][\w:.-]*)",
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
    return rules;
}

SPEARMINT_API void register_xml_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<xml_lexer>(); }, xml_info);
}

} // namespace spearmint::lexers

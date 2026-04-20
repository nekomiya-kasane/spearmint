#include "spearmint/lexers/css.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"css"};
constexpr const char *filenames[] = {"*.css"};
constexpr const char *mimes[] = {"text/css"};
const lexer_info css_info = {
    "css", "CSS", {aliases}, {filenames}, {mimes}, "https://www.w3.org/Style/CSS/", 10,
};
} // namespace

const lexer_info &css_lexer::info() const noexcept {
    return css_info;
}

float css_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("{") != src.npos && src.find("}") != src.npos) score += 0.1f;
    if (src.find("color:") != src.npos || src.find("margin:") != src.npos) score += 0.2f;
    if (src.find("@media") != src.npos) score += 0.2f;
    if (src.find("display:") != src.npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map css_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(@[a-zA-Z-]+)", tk::keyword::self, state_action::none()},
        {R"(\.[a-zA-Z_][\w-]*)", tk::name::class_, state_action::none()},
        {R"(#[a-zA-Z_][\w-]*)", tk::name::label, state_action::none()},
        {R"(::[a-zA-Z-]+)", tk::name::decorator, state_action::none()},
        {R"(:[a-zA-Z-]+)", tk::name::decorator, state_action::none()},
        {R"(\b(inherit|initial|unset|revert|none|auto|normal|bold|italic|underline|solid|dashed|dotted|block|inline|flex|grid|absolute|relative|fixed|sticky|hidden|visible|transparent|currentColor|important)\b)",
         tk::keyword::constant, state_action::none()},
        {R"(!important)", tk::keyword::self, state_action::none()},
        {R"([0-9]+(\.[0-9]+)?(px|em|rem|%|vh|vw|vmin|vmax|ch|ex|cm|mm|in|pt|pc|deg|rad|grad|turn|s|ms|Hz|kHz|dpi|dpcm|dppx|fr)?)",
         tk::literal::number::self, state_action::none()},
        {R"(#[0-9a-fA-F]{3,8})", tk::literal::number::hex, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::push_state("dstring")},
        {R"(')", tk::literal::string::single, state_action::push_state("sstring")},
        {R"(url\()", tk::name::builtin, state_action::none()},
        {R"(\b(rgb|rgba|hsl|hsla|calc|var|min|max|clamp|env|attr)\b)", tk::name::builtin, state_action::none()},
        {R"([a-zA-Z-]+(?=\s*:))", tk::name::self, state_action::none()},
        {R"([{}();:,>+~*])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_][\w-]*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };
    rules["dstring"] = {
        {R"(\\.)", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
    };
    rules["sstring"] = {
        {R"(\\.)", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^'\\]+)", tk::literal::string::single, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_css_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<css_lexer>(); }, css_info);
}

} // namespace spearmint::lexers

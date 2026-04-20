#include "spearmint/lexers/ini.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"ini", "cfg", "dosini"};
constexpr const char* filenames[] = {"*.ini", "*.cfg", "*.conf", ".editorconfig", ".gitconfig"};
constexpr const char* mimes[] = {"text/x-ini"};
const lexer_info ini_info = {
    "ini", "INI",
    {aliases}, {filenames}, {mimes},
    "", 10,
};
}

const lexer_info& ini_lexer::info() const noexcept { return ini_info; }

float ini_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("[") != src.npos && src.find("]") != src.npos) score += 0.1f;
    if (src.find("=") != src.npos) score += 0.05f;
    return score > 1.0f ? 1.0f : score;
}

state_map ini_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"([;#][^\n]*)", tk::comment::single, state_action::none()},
        {R"(\[[^\]]+\])", tk::name::label, state_action::none()},
        {R"([a-zA-Z_][\w.-]*(?=\s*[=:]))", tk::name::attribute, state_action::none()},
        {R"([=:])", tk::operator_::self, state_action::none()},
        {R"(\b(true|false|yes|no|on|off)\b)", tk::keyword::constant, state_action::none()},
        {R"([0-9]+\.[0-9]+)", tk::literal::number::float_, state_action::none()},
        {R"([0-9]+)", tk::literal::number::integer, state_action::none()},
        {R"("[^"]*")", tk::literal::string::double_, state_action::none()},
        {R"('[^']*')", tk::literal::string::single, state_action::none()},
        {R"([^\s;#\[\]=:"']+)", tk::literal::string::self, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_ini_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<ini_lexer>();
    }, ini_info);
}

}

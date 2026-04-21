#include "spearmint/lexers/ini.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"ini", "cfg", "dosini"};
constexpr const char *filenames[] = {"*.ini", "*.cfg", "*.conf", ".editorconfig", ".gitconfig"};
constexpr const char *mimes[] = {"text/x-ini"};
const lexer_info ini_info = {
    .name = "ini",
    .display_name = "INI",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "",
    .priority = 10,
};
} // namespace

const lexer_info &ini_lexer::info() const noexcept {
    return ini_info;
}

float ini_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("[") != src.npos && src.find("]") != src.npos) {
        score += 0.1f;
    }
    if (src.find("=") != src.npos) {
        score += 0.05f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map ini_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([;#][^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\[[^\]]+\])", .token = tk::name::label, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([a-zA-Z_][\w.-]*(?=\s*[=:]))",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([=:])", .token = tk::operator_::self, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\b(true|false|yes|no|on|off)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9]+\.[0-9]+)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9]+)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"("[^"]*")",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"('[^']*')",
         .token = tk::literal::string::single,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([^\s;#\[\]=:"']+)",
         .token = tk::literal::string::self,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    return rules;
}

SPEARMINT_API void register_ini_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<ini_lexer>(); }, ini_info);
}

} // namespace spearmint::lexers

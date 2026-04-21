#include "spearmint/lexers/yaml.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"yaml", "yml"};
constexpr const char *filenames[] = {"*.yaml", "*.yml"};
constexpr const char *mimes[] = {"text/x-yaml"};
const lexer_info yaml_info = {
    .name = "yaml",
    .display_name = "YAML",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://yaml.org",
    .priority = 10,
};
} // namespace

const lexer_info &yaml_lexer::info() const noexcept {
    return yaml_info;
}

float yaml_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("---") != src.npos) score += 0.2f;
    if (src.find(": ") != src.npos) score += 0.1f;
    if (src.find("- ") != src.npos) score += 0.05f;
    return score > 1.0f ? 1.0f : score;
}

state_map yaml_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(---)", .token = tk::punctuation::self, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\.\.\.)", .token = tk::punctuation::self, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\b(true|false|yes|no|on|off|null|~)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(&[a-zA-Z_]\w*)", .token = tk::name::label, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\*[a-zA-Z_]\w*)",
         .token = tk::name::variable,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(!!?[a-zA-Z_][\w/.-]*)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([a-zA-Z_][\w ]*(?=\s*:))",
         .token = tk::name::attribute,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("dstring"),
         .group_tokens = {}},
        {.pattern = R"(')",
         .token = tk::literal::string::single,
         .action = state_action::push_state("sstring"),
         .group_tokens = {}},
        {.pattern = R"([+-]?[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9]+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([+-]?\.[0-9_]+([eE][+-]?[0-9]+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0x[0-9a-fA-F]+)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0o[0-7]+)",
         .token = tk::literal::number::oct,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([+-]?[0-9][0-9_]*)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\[\]{}:,\-?|>])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([^\s#"'\[\]{}:,]+)",
         .token = tk::literal::string::self,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    rules["dstring"] = {
        {.pattern = R"(\\[\\"/bfnrt])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\u[0-9a-fA-F]{4})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^"\\]+)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\.)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    rules["sstring"] = {
        {.pattern = R"('')", .token = tk::literal::string::escape, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(')",
         .token = tk::literal::string::single,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^']+)",
         .token = tk::literal::string::single,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    return rules;
}

SPEARMINT_API void register_yaml_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<yaml_lexer>(); }, yaml_info);
}

} // namespace spearmint::lexers

#include "spearmint/lexers/yaml.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"yaml", "yml"};
constexpr const char* filenames[] = {"*.yaml", "*.yml"};
constexpr const char* mimes[] = {"text/x-yaml"};
const lexer_info yaml_info = {
    "yaml", "YAML",
    {aliases}, {filenames}, {mimes},
    "https://yaml.org", 10,
};
}

const lexer_info& yaml_lexer::info() const noexcept { return yaml_info; }

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
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},
        {R"(---)", tk::punctuation::self, state_action::none()},
        {R"(\.\.\.)", tk::punctuation::self, state_action::none()},
        {R"(\b(true|false|yes|no|on|off|null|~)\b)", tk::keyword::constant, state_action::none()},
        {R"(&[a-zA-Z_]\w*)", tk::name::label, state_action::none()},
        {R"(\*[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(!!?[a-zA-Z_][\w/.-]*)", tk::keyword::type, state_action::none()},
        {R"([a-zA-Z_][\w ]*(?=\s*:))", tk::name::attribute, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::push_state("dstring")},
        {R"(')", tk::literal::string::single, state_action::push_state("sstring")},
        {R"([+-]?[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9]+)?)", tk::literal::number::float_, state_action::none()},
        {R"([+-]?\.[0-9_]+([eE][+-]?[0-9]+)?)", tk::literal::number::float_, state_action::none()},
        {R"(0x[0-9a-fA-F]+)", tk::literal::number::hex, state_action::none()},
        {R"(0o[0-7]+)", tk::literal::number::oct, state_action::none()},
        {R"([+-]?[0-9][0-9_]*)", tk::literal::number::integer, state_action::none()},
        {R"([\[\]{}:,\-?|>])", tk::punctuation::self, state_action::none()},
        {R"([^\s#"'\[\]{}:,]+)", tk::literal::string::self, state_action::none()},
    };
    rules["dstring"] = {
        {R"(\\[\\"/bfnrt])", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["sstring"] = {
        {R"('')", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^']+)", tk::literal::string::single, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_yaml_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<yaml_lexer>();
    }, yaml_info);
}

}

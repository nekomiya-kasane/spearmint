#include "spearmint/lexers/toml.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"toml"};
constexpr const char* filenames[] = {"*.toml", "Cargo.toml", "pyproject.toml"};
constexpr const char* mimes[] = {"application/toml"};
const lexer_info toml_info = {
    "toml", "TOML",
    {aliases}, {filenames}, {mimes},
    "https://toml.io", 10,
};
}

const lexer_info& toml_lexer::info() const noexcept { return toml_info; }

float toml_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("[package]") != src.npos || src.find("[dependencies]") != src.npos) score += 0.5f;
    if (src.find("[[") != src.npos) score += 0.2f;
    if (src.find(" = ") != src.npos) score += 0.05f;
    return score > 1.0f ? 1.0f : score;
}

state_map toml_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},
        {R"(\[\[[a-zA-Z_][\w.-]*\]\])", tk::name::label, state_action::none()},
        {R"(\[[a-zA-Z_][\w.-]*\])", tk::name::label, state_action::none()},
        {R"([a-zA-Z_][\w-]*(?=\s*=))", tk::name::attribute, state_action::none()},
        {R"(=)", tk::operator_::self, state_action::none()},
        {R"(\b(true|false)\b)", tk::keyword::constant, state_action::none()},
        {R"(\d{4}-\d{2}-\d{2}[T ]\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[+-]\d{2}:\d{2})?)", tk::literal::string::self, state_action::none()},
        {R"(\d{4}-\d{2}-\d{2})", tk::literal::string::self, state_action::none()},
        {R"(\d{2}:\d{2}:\d{2}(\.\d+)?)", tk::literal::string::self, state_action::none()},
        {R"(0x[0-9a-fA-F_]+)", tk::literal::number::hex, state_action::none()},
        {R"(0o[0-7_]+)", tk::literal::number::oct, state_action::none()},
        {R"(0b[01_]+)", tk::literal::number::bin, state_action::none()},
        {R"([+-]?[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)", tk::literal::number::float_, state_action::none()},
        {R"([+-]?(inf|nan))", tk::literal::number::float_, state_action::none()},
        {R"([+-]?[0-9][0-9_]*)", tk::literal::number::integer, state_action::none()},
        {R"(""")", tk::literal::string::double_, state_action::push_state("ml_basic")},
        {R"(")", tk::literal::string::double_, state_action::push_state("basic")},
        {R"(''')", tk::literal::string::single, state_action::push_state("ml_literal")},
        {R"(')", tk::literal::string::single, state_action::push_state("literal")},
        {R"([{}\[\],.])", tk::punctuation::self, state_action::none()},
    };
    rules["basic"] = {
        {R"(\\[\\"/bfnrt])", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\U[0-9a-fA-F]{8})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["literal"] = {
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^']+)", tk::literal::string::single, state_action::none()},
    };
    rules["ml_basic"] = {
        {R"(\\[\\"/bfnrt])", tk::literal::string::escape, state_action::none()},
        {R"(""")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["ml_literal"] = {
        {R"(''')", tk::literal::string::single, state_action::pop_state()},
        {R"([^']+)", tk::literal::string::single, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_toml_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<toml_lexer>();
    }, toml_info);
}

}

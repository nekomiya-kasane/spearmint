#include "spearmint/lexers/toml.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"toml"};
constexpr const char *filenames[] = {"*.toml", "Cargo.toml", "pyproject.toml"};
constexpr const char *mimes[] = {"application/toml"};
const lexer_info toml_info = {
    "toml", "TOML", {aliases}, {filenames}, {mimes}, "https://toml.io", 10,
};
} // namespace

const lexer_info &toml_lexer::info() const noexcept {
    return toml_info;
}

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
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none()},
        {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none()},
        {.pattern = R"(\[\[[a-zA-Z_][\w.-]*\]\])", .token = tk::name::label, .action = state_action::none()},
        {.pattern = R"(\[[a-zA-Z_][\w.-]*\])", .token = tk::name::label, .action = state_action::none()},
        {.pattern = R"([a-zA-Z_][\w-]*(?=\s*=))", .token = tk::name::attribute, .action = state_action::none()},
        {.pattern = R"(=)", .token = tk::operator_::self, .action = state_action::none()},
        {.pattern = R"(\b(true|false)\b)", .token = tk::keyword::constant, .action = state_action::none()},
        {.pattern = R"(\d{4}-\d{2}-\d{2}[T ]\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[+-]\d{2}:\d{2})?)",
         .token = tk::literal::string::self,
         .action = state_action::none()},
        {.pattern = R"(\d{4}-\d{2}-\d{2})", .token = tk::literal::string::self, .action = state_action::none()},
        {.pattern = R"(\d{2}:\d{2}:\d{2}(\.\d+)?)", .token = tk::literal::string::self, .action = state_action::none()},
        {.pattern = R"(0x[0-9a-fA-F_]+)", .token = tk::literal::number::hex, .action = state_action::none()},
        {.pattern = R"(0o[0-7_]+)", .token = tk::literal::number::oct, .action = state_action::none()},
        {.pattern = R"(0b[01_]+)", .token = tk::literal::number::bin, .action = state_action::none()},
        {.pattern = R"([+-]?[0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none()},
        {.pattern = R"([+-]?(inf|nan))", .token = tk::literal::number::float_, .action = state_action::none()},
        {.pattern = R"([+-]?[0-9][0-9_]*)", .token = tk::literal::number::integer, .action = state_action::none()},
        {.pattern = R"(""")", .token = tk::literal::string::double_, .action = state_action::push_state("ml_basic")},
        {.pattern = R"(")", .token = tk::literal::string::double_, .action = state_action::push_state("basic")},
        {.pattern = R"(''')", .token = tk::literal::string::single, .action = state_action::push_state("ml_literal")},
        {.pattern = R"(')", .token = tk::literal::string::single, .action = state_action::push_state("literal")},
        {.pattern = R"([{}\[\],.])", .token = tk::punctuation::self, .action = state_action::none()},
    };
    rules["basic"] = {
        {.pattern = R"(\\[\\"/bfnrt])", .token = tk::literal::string::escape, .action = state_action::none()},
        {.pattern = R"(\\u[0-9a-fA-F]{4})", .token = tk::literal::string::escape, .action = state_action::none()},
        {.pattern = R"(\\U[0-9a-fA-F]{8})", .token = tk::literal::string::escape, .action = state_action::none()},
        {.pattern = R"(")", .token = tk::literal::string::double_, .action = state_action::pop_state()},
        {.pattern = R"([^"\\]+)", .token = tk::literal::string::double_, .action = state_action::none()},
        {.pattern = R"(\\.)", .token = tk::literal::string::double_, .action = state_action::none()},
    };
    rules["literal"] = {
        {.pattern = R"(')", .token = tk::literal::string::single, .action = state_action::pop_state()},
        {.pattern = R"([^']+)", .token = tk::literal::string::single, .action = state_action::none()},
    };
    rules["ml_basic"] = {
        {.pattern = R"(\\[\\"/bfnrt])", .token = tk::literal::string::escape, .action = state_action::none()},
        {.pattern = R"(""")", .token = tk::literal::string::double_, .action = state_action::pop_state()},
        {.pattern = R"([^"\\]+)", .token = tk::literal::string::double_, .action = state_action::none()},
        {.pattern = R"(")", .token = tk::literal::string::double_, .action = state_action::none()},
        {.pattern = R"(\\.)", .token = tk::literal::string::double_, .action = state_action::none()},
    };
    rules["ml_literal"] = {
        {.pattern = R"(''')", .token = tk::literal::string::single, .action = state_action::pop_state()},
        {.pattern = R"([^']+)", .token = tk::literal::string::single, .action = state_action::none()},
        {.pattern = R"(')", .token = tk::literal::string::single, .action = state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_toml_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<toml_lexer>(); }, toml_info);
}

} // namespace spearmint::lexers

#include "spearmint/lexers/go.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"go", "golang"};
constexpr const char* filenames[] = {"*.go"};
constexpr const char* mimes[] = {"text/x-gosrc"};
const lexer_info go_info = {
    "go", "Go",
    {aliases}, {filenames}, {mimes},
    "https://go.dev", 10,
};
}

const lexer_info& go_lexer::info() const noexcept { return go_info; }

float go_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("package ") != src.npos) score += 0.3f;
    if (src.find("func ") != src.npos) score += 0.2f;
    if (src.find("import ") != src.npos) score += 0.1f;
    if (src.find(":=") != src.npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map go_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(\b(break|case|chan|const|continue|default|defer|else|fallthrough|for|func|go|goto|if|import|interface|map|package|range|return|select|struct|switch|type|var)\b)", tk::keyword::self, state_action::none()},
        {R"(\b(bool|byte|complex64|complex128|error|float32|float64|int|int8|int16|int32|int64|rune|string|uint|uint8|uint16|uint32|uint64|uintptr)\b)", tk::keyword::type, state_action::none()},
        {R"(\b(true|false|nil|iota)\b)", tk::keyword::constant, state_action::none()},
        {R"(\b(append|cap|close|complex|copy|delete|imag|len|make|new|panic|print|println|real|recover)\b)", tk::name::builtin, state_action::none()},
        {R"(0x[0-9a-fA-F_]+)", tk::literal::number::hex, state_action::none()},
        {R"(0o[0-7_]+)", tk::literal::number::oct, state_action::none()},
        {R"(0b[01_]+)", tk::literal::number::bin, state_action::none()},
        {R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*)", tk::literal::number::integer, state_action::none()},
        {R"(`)", tk::literal::string::backtick, state_action::push_state("rawstring")},
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},
        {R"('[^'\\]')", tk::literal::string::char_, state_action::none()},
        {R"('\\.')", tk::literal::string::char_, state_action::none()},
        {R"(:=|<-|&&|\|\||<<|>>|[+\-*/%&|^!<>=]=?)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;:])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_]\w*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };
    rules["string"] = {
        {R"(\\[\\'"abfnrtv])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\U[0-9a-fA-F]{8})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["rawstring"] = {
        {R"(`)", tk::literal::string::backtick, state_action::pop_state()},
        {R"([^`]+)", tk::literal::string::backtick, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_go_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<go_lexer>();
    }, go_info);
}

}

#include "spearmint/lexers/rust.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"rust", "rs"};
constexpr const char* filenames[] = {"*.rs"};
constexpr const char* mimes[] = {"text/x-rust"};
const lexer_info rust_info = {
    "rust", "Rust",
    {aliases}, {filenames}, {mimes},
    "https://www.rust-lang.org", 10,
};
}

const lexer_info& rust_lexer::info() const noexcept { return rust_info; }

float rust_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("fn ") != src.npos) score += 0.2f;
    if (src.find("let ") != src.npos) score += 0.1f;
    if (src.find("use ") != src.npos) score += 0.1f;
    if (src.find("impl ") != src.npos) score += 0.2f;
    if (src.find("->") != src.npos) score += 0.05f;
    if (src.find("::") != src.npos) score += 0.05f;
    return score > 1.0f ? 1.0f : score;
}

state_map rust_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(#!\[)", tk::comment::preproc, state_action::none()},
        {R"(#\[)", tk::name::decorator, state_action::none()},
        {R"(\b(as|async|await|break|const|continue|crate|dyn|else|enum|extern|false|fn|for|if|impl|in|let|loop|match|mod|move|mut|pub|ref|return|self|Self|static|struct|super|trait|true|type|unsafe|use|where|while|yield)\b)", tk::keyword::self, state_action::none()},
        {R"(\b(bool|char|f32|f64|i8|i16|i32|i64|i128|isize|str|u8|u16|u32|u64|u128|usize)\b)", tk::keyword::type, state_action::none()},
        {R"(\b(Box|Option|Result|String|Vec|HashMap|HashSet|Rc|Arc|Cell|RefCell|Mutex|Some|None|Ok|Err)\b)", tk::name::builtin, state_action::none()},
        {R"(\b(println|print|eprintln|eprint|format|vec|panic|assert|assert_eq|assert_ne|todo|unimplemented|unreachable|cfg|derive|test|allow|warn|deny|forbid)!?)", tk::name::builtin, state_action::none()},
        {R"(0x[0-9a-fA-F_]+)", tk::literal::number::hex, state_action::none()},
        {R"(0o[0-7_]+)", tk::literal::number::oct, state_action::none()},
        {R"(0b[01_]+)", tk::literal::number::bin, state_action::none()},
        {R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?f?(32|64)?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*([eE][+-]?[0-9_]+))", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*(u8|u16|u32|u64|u128|usize|i8|i16|i32|i64|i128|isize)?)", tk::literal::number::integer, state_action::none()},
        {R"(b")", tk::literal::string::double_, state_action::push_state("bytestring")},
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},
        {R"(r#*")", tk::literal::string::self, state_action::push_state("rawstring")},
        {R"(b'[^'\\]')", tk::literal::string::char_, state_action::none()},
        {R"(b'\\.')", tk::literal::string::char_, state_action::none()},
        {R"('[^'\\]')", tk::literal::string::char_, state_action::none()},
        {R"('\\.')", tk::literal::string::char_, state_action::none()},
        {R"('[a-zA-Z_]\w*)", tk::name::label, state_action::none()},
        {R"(=>|->|\.\.\.|\.\.|&&|\|\||<<|>>|[+\-*/%&|^!<>=]=?)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;:@#?])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_]\w*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"([^/*]+)", tk::comment::multiline, state_action::none()},
        {R"([/*])", tk::comment::multiline, state_action::none()},
    };
    rules["string"] = {
        {R"(\\[\\'"0nrt])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(\\u\{[0-9a-fA-F]+\})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["bytestring"] = {
        {R"(\\[\\'"0nrt])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["rawstring"] = {
        {R"("[#]*)", tk::literal::string::self, state_action::pop_state()},
        {R"([^"]+)", tk::literal::string::self, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_rust_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<rust_lexer>();
    }, rust_info);
}

}

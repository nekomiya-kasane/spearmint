#include "spearmint/lexers/rust.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"rust", "rs"};
constexpr const char *filenames[] = {"*.rs"};
constexpr const char *mimes[] = {"text/x-rust"};
const lexer_info rust_info = {
    .name = "rust",
    .display_name = "Rust",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://www.rust-lang.org",
    .priority = 10,
};
} // namespace

const lexer_info &rust_lexer::info() const noexcept {
    return rust_info;
}

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
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(//[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern = R"(#!\[)", .token = tk::comment::preproc, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(#\[)", .token = tk::name::decorator, .action = state_action::none(), .group_tokens = {}},
        {.pattern =
             R"(\b(as|async|await|break|const|continue|crate|dyn|else|enum|extern|false|fn|for|if|impl|in|let|loop|match|mod|move|mut|pub|ref|return|self|Self|static|struct|super|trait|true|type|unsafe|use|where|while|yield)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(bool|char|f32|f64|i8|i16|i32|i64|i128|isize|str|u8|u16|u32|u64|u128|usize)\b)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(Box|Option|Result|String|Vec|HashMap|HashSet|Rc|Arc|Cell|RefCell|Mutex|Some|None|Ok|Err)\b)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(println|print|eprintln|eprint|format|vec|panic|assert|assert_eq|assert_ne|todo|unimplemented|unreachable|cfg|derive|test|allow|warn|deny|forbid)!?)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0x[0-9a-fA-F_]+)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0o[0-7_]+)",
         .token = tk::literal::number::oct,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0b[01_]+)",
         .token = tk::literal::number::bin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?f?(32|64)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*([eE][+-]?[0-9_]+))",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*(u8|u16|u32|u64|u128|usize|i8|i16|i32|i64|i128|isize)?)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(b")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("bytestring"),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("string"),
         .group_tokens = {}},
        {.pattern = R"(r#*")",
         .token = tk::literal::string::self,
         .action = state_action::push_state("rawstring"),
         .group_tokens = {}},
        {.pattern = R"(b'[^'\\]')",
         .token = tk::literal::string::char_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(b'\\.')",
         .token = tk::literal::string::char_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"('[^'\\]')",
         .token = tk::literal::string::char_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"('\\.')",
         .token = tk::literal::string::char_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"('[a-zA-Z_]\w*)", .token = tk::name::label, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(=>|->|\.\.\.|\.\.|&&|\|\||<<|>>|[+\-*/%&|^!<>=]=?)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\[\](){}.,;:@#?])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([a-zA-Z_]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
    };
    rules["comment"] = {
        {.pattern = R"(\*/)", .token = tk::comment::multiline, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern = R"([^/*]+)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([/*])", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
    };
    rules["string"] = {
        {.pattern = R"(\\[\\'"0nrt])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\x[0-9a-fA-F]{2})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\u\{[0-9a-fA-F]+\})",
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
    rules["bytestring"] = {
        {.pattern = R"(\\[\\'"0nrt])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\x[0-9a-fA-F]{2})",
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
    rules["rawstring"] = {
        {.pattern = R"("[#]*)",
         .token = tk::literal::string::self,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^"]+)", .token = tk::literal::string::self, .action = state_action::none(), .group_tokens = {}},
    };
    return rules;
}

SPEARMINT_API void register_rust_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<rust_lexer>(); }, rust_info);
}

} // namespace spearmint::lexers

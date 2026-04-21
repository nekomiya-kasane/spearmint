#include "spearmint/lexers/go.h"

namespace spearmint::lexers {

    namespace {
        constexpr const char *aliases[] = {"go", "golang"};
        constexpr const char *filenames[] = {"*.go"};
        constexpr const char *mimes[] = {"text/x-gosrc"};
        const lexer_info go_info = {
            .name = "go",
            .display_name = "Go",
            .aliases = {aliases},
            .filenames = {filenames},
            .mime_types = {mimes},
            .url = "https://go.dev",
            .priority = 10,
        };
    } // namespace

    const lexer_info &go_lexer::info() const noexcept {
        return go_info;
    }

    float go_lexer::analyse_text(std::string_view src) const noexcept {
        float score = 0.0f;
        if (src.find("package ") != src.npos) {
            score += 0.3f;
        }
        if (src.find("func ") != src.npos) {
            score += 0.2f;
        }
        if (src.find("import ") != src.npos) {
            score += 0.1f;
        }
        if (src.find(":=") != src.npos) {
            score += 0.1f;
        }
        return score > 1.0f ? 1.0f : score;
    }

    state_map go_lexer::get_rules() const {
        namespace tk = token;
        state_map rules;
        rules["root"] = {
            {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(//[^\n]*)",
             .token = tk::comment::single,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(/\*)",
             .token = tk::comment::multiline,
             .action = state_action::push_state("comment"),
             .group_tokens = {}},
            {.pattern =
                 R"(\b(break|case|chan|const|continue|default|defer|else|fallthrough|for|func|go|goto|if|import|interface|map|package|range|return|select|struct|switch|type|var)\b)",
             .token = tk::keyword::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern =
                 R"(\b(bool|byte|complex64|complex128|error|float32|float64|int|int8|int16|int32|int64|rune|string|uint|uint8|uint16|uint32|uint64|uintptr)\b)",
             .token = tk::keyword::type,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\b(true|false|nil|iota)\b)",
             .token = tk::keyword::constant,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern =
                 R"(\b(append|cap|close|complex|copy|delete|imag|len|make|new|panic|print|println|real|recover)\b)",
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
            {.pattern = R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)",
             .token = tk::literal::number::float_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([0-9][0-9_]*)",
             .token = tk::literal::number::integer,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(`)",
             .token = tk::literal::string::backtick,
             .action = state_action::push_state("rawstring"),
             .group_tokens = {}},
            {.pattern = R"(")",
             .token = tk::literal::string::double_,
             .action = state_action::push_state("string"),
             .group_tokens = {}},
            {.pattern = R"('[^'\\]')",
             .token = tk::literal::string::char_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"('\\.')",
             .token = tk::literal::string::char_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(:=|<-|&&|\|\||<<|>>|[+\-*/%&|^!<>=]=?)",
             .token = tk::operator_::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([\[\](){}.,;:])",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([a-zA-Z_]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
        };
        rules["comment"] = {
            {.pattern = R"(\*/)",
             .token = tk::comment::multiline,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"([^*]+)",
             .token = tk::comment::multiline,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\*)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        };
        rules["string"] = {
            {.pattern = R"(\\[\\'"abfnrtv])",
             .token = tk::literal::string::escape,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\\x[0-9a-fA-F]{2})",
             .token = tk::literal::string::escape,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\\u[0-9a-fA-F]{4})",
             .token = tk::literal::string::escape,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\\U[0-9a-fA-F]{8})",
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
            {.pattern = R"(`)",
             .token = tk::literal::string::backtick,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"([^`]+)",
             .token = tk::literal::string::backtick,
             .action = state_action::none(),
             .group_tokens = {}},
        };
        return rules;
    }

    SPEARMINT_API void register_go_lexer() {
        register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<go_lexer>(); }, go_info);
    }

} // namespace spearmint::lexers

#include "spearmint/lexers/typescript.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"typescript", "ts"};
constexpr const char *filenames[] = {"*.ts", "*.tsx", "*.mts", "*.cts"};
constexpr const char *mimes[] = {"text/typescript", "application/typescript"};
const lexer_info ts_info = {
    .name = "typescript",
    .display_name = "TypeScript",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://www.typescriptlang.org",
    .priority = 10,
};
} // namespace

const lexer_info &typescript_lexer::info() const noexcept {
    return ts_info;
}

float typescript_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("interface ") != src.npos) {
        score += 0.2f;
    }
    if (src.find(": string") != src.npos || src.find(": number") != src.npos) {
        score += 0.2f;
    }
    if (src.find("import ") != src.npos) {
        score += 0.1f;
    }
    if (src.find("export ") != src.npos) {
        score += 0.1f;
    }
    if (src.find("=>") != src.npos) {
        score += 0.05f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map typescript_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(//[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern =
             R"(\b(abstract|as|async|await|break|case|catch|class|const|continue|debugger|declare|default|delete|do|else|enum|export|extends|finally|for|from|function|get|if|implements|import|in|instanceof|interface|keyof|let|module|namespace|new|of|override|package|private|protected|public|readonly|return|satisfies|set|static|super|switch|this|throw|try|type|typeof|var|void|while|with|yield)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(true|false|null|undefined|NaN|Infinity)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(any|bigint|boolean|never|number|object|string|symbol|unknown|void)\b)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(Array|Boolean|Date|Error|Function|JSON|Map|Math|Number|Object|Promise|Proxy|RegExp|Set|String|Symbol|WeakMap|WeakSet|console|document|window|globalThis)\b)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0x[0-9a-fA-F_]+n?)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0o[0-7_]+n?)",
         .token = tk::literal::number::oct,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0b[01_]+n?)",
         .token = tk::literal::number::bin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*n?)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(`)",
         .token = tk::literal::string::backtick,
         .action = state_action::push_state("template"),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("dstring"),
         .group_tokens = {}},
        {.pattern = R"(')",
         .token = tk::literal::string::single,
         .action = state_action::push_state("sstring"),
         .group_tokens = {}},
        {.pattern = R"(/(?![/*]))",
         .token = tk::literal::string::regex,
         .action = state_action::push_state("regex"),
         .group_tokens = {}},
        {.pattern = R"(=>|\.\.\.|\?\?|&&|\|\||<<|>>>?|[+\-*/%&|^~!<>=]=?|\?\.?)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(@[a-zA-Z_]\w*)",
         .token = tk::name::decorator,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\[\](){}.,;:])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([a-zA-Z_$]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
    };
    rules["comment"] = {
        {.pattern = R"(\*/)", .token = tk::comment::multiline, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"([^*]+)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\*)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
    };
    rules["dstring"] = {
        {.pattern = R"(\\[\\'"bfnrtv0])",
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
    rules["sstring"] = {
        {.pattern = R"(\\[\\'"bfnrtv0])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(')",
         .token = tk::literal::string::single,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^'\\]+)",
         .token = tk::literal::string::single,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\.)", .token = tk::literal::string::single, .action = state_action::none(), .group_tokens = {}},
    };
    rules["template"] = {
        {.pattern = R"(\\[\\`$])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\$\{)",
         .token = tk::literal::string::interpol,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(`)",
         .token = tk::literal::string::backtick,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^`\\$]+)",
         .token = tk::literal::string::backtick,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([$\\])",
         .token = tk::literal::string::backtick,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    rules["regex"] = {
        {.pattern = R"(\\.)", .token = tk::literal::string::regex, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(/[gimsuy]*)",
         .token = tk::literal::string::regex,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^/\\]+)",
         .token = tk::literal::string::regex,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    return rules;
}

SPEARMINT_API void register_typescript_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<typescript_lexer>(); }, ts_info);
}

} // namespace spearmint::lexers

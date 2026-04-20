#include "spearmint/lexers/typescript.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"typescript", "ts"};
constexpr const char* filenames[] = {"*.ts", "*.tsx", "*.mts", "*.cts"};
constexpr const char* mimes[] = {"text/typescript", "application/typescript"};
const lexer_info ts_info = {
    "typescript", "TypeScript",
    {aliases}, {filenames}, {mimes},
    "https://www.typescriptlang.org", 10,
};
}

const lexer_info& typescript_lexer::info() const noexcept { return ts_info; }

float typescript_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("interface ") != src.npos) score += 0.2f;
    if (src.find(": string") != src.npos || src.find(": number") != src.npos) score += 0.2f;
    if (src.find("import ") != src.npos) score += 0.1f;
    if (src.find("export ") != src.npos) score += 0.1f;
    if (src.find("=>") != src.npos) score += 0.05f;
    return score > 1.0f ? 1.0f : score;
}

state_map typescript_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(\b(abstract|as|async|await|break|case|catch|class|const|continue|debugger|declare|default|delete|do|else|enum|export|extends|finally|for|from|function|get|if|implements|import|in|instanceof|interface|keyof|let|module|namespace|new|of|override|package|private|protected|public|readonly|return|satisfies|set|static|super|switch|this|throw|try|type|typeof|var|void|while|with|yield)\b)", tk::keyword::self, state_action::none()},
        {R"(\b(true|false|null|undefined|NaN|Infinity)\b)", tk::keyword::constant, state_action::none()},
        {R"(\b(any|bigint|boolean|never|number|object|string|symbol|unknown|void)\b)", tk::keyword::type, state_action::none()},
        {R"(\b(Array|Boolean|Date|Error|Function|JSON|Map|Math|Number|Object|Promise|Proxy|RegExp|Set|String|Symbol|WeakMap|WeakSet|console|document|window|globalThis)\b)", tk::name::builtin, state_action::none()},
        {R"(0x[0-9a-fA-F_]+n?)", tk::literal::number::hex, state_action::none()},
        {R"(0o[0-7_]+n?)", tk::literal::number::oct, state_action::none()},
        {R"(0b[01_]+n?)", tk::literal::number::bin, state_action::none()},
        {R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*n?)", tk::literal::number::integer, state_action::none()},
        {R"(`)", tk::literal::string::backtick, state_action::push_state("template")},
        {R"(")", tk::literal::string::double_, state_action::push_state("dstring")},
        {R"(')", tk::literal::string::single, state_action::push_state("sstring")},
        {R"(/(?![/*]))", tk::literal::string::regex, state_action::push_state("regex")},
        {R"(=>|\.\.\.|\?\?|&&|\|\||<<|>>>?|[+\-*/%&|^~!<>=]=?|\?\.?)", tk::operator_::self, state_action::none()},
        {R"(@[a-zA-Z_]\w*)", tk::name::decorator, state_action::none()},
        {R"([\[\](){}.,;:])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_$]\w*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };
    rules["dstring"] = {
        {R"(\\[\\'"bfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\u\{[0-9a-fA-F]+\})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["sstring"] = {
        {R"(\\[\\'"bfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^'\\]+)", tk::literal::string::single, state_action::none()},
        {R"(\\.)", tk::literal::string::single, state_action::none()},
    };
    rules["template"] = {
        {R"(\\[\\`$])", tk::literal::string::escape, state_action::none()},
        {R"(\$\{)", tk::literal::string::interpol, state_action::none()},
        {R"(`)", tk::literal::string::backtick, state_action::pop_state()},
        {R"([^`\\$]+)", tk::literal::string::backtick, state_action::none()},
        {R"([$\\])", tk::literal::string::backtick, state_action::none()},
    };
    rules["regex"] = {
        {R"(\\.)", tk::literal::string::regex, state_action::none()},
        {R"(/[gimsuy]*)", tk::literal::string::regex, state_action::pop_state()},
        {R"([^/\\]+)", tk::literal::string::regex, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_typescript_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<typescript_lexer>();
    }, ts_info);
}

}

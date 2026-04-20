/**
 * @file javascript.cpp
 * @brief JavaScript / ECMAScript lexer implementation.
 */

#include "spearmint/lexers/javascript.h"

namespace spearmint::lexers {

namespace {

constexpr const char *aliases[] = {"javascript", "js", "ecmascript"};
constexpr const char *filenames[] = {"*.js", "*.mjs", "*.cjs", "*.jsx"};
constexpr const char *mimes[] = {"text/javascript", "application/javascript"};

const lexer_info js_info = {
    "javascript", "JavaScript", {aliases},
    {filenames},  {mimes},      "https://developer.mozilla.org/en-US/docs/Web/JavaScript",
    10,
};

} // namespace

const lexer_info &javascript_lexer::info() const noexcept {
    return js_info;
}

float javascript_lexer::analyse_text(std::string_view source) const noexcept {
    float score = 0.0f;
    if (source.find("function ") != std::string_view::npos) score += 0.1f;
    if (source.find("const ") != std::string_view::npos) score += 0.05f;
    if (source.find("let ") != std::string_view::npos) score += 0.05f;
    if (source.find("var ") != std::string_view::npos) score += 0.05f;
    if (source.find("=>") != std::string_view::npos) score += 0.1f;
    if (source.find("console.log") != std::string_view::npos) score += 0.15f;
    if (source.find("require(") != std::string_view::npos) score += 0.1f;
    if (source.find("import ") != std::string_view::npos) score += 0.05f;
    return score > 1.0f ? 1.0f : score;
}

state_map javascript_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;

    rules["root"] = {
        {R"(\n)", tk::whitespace, state_action::none()},
        {R"([ \t\r]+)", tk::whitespace, state_action::none()},

        // Single-line comments
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},

        // Multi-line comments
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},

        // Template literals
        {R"(`)", tk::literal::string::backtick, state_action::push_state("template")},

        // Strings
        {R"(")", tk::literal::string::double_, state_action::push_state("dqs")},
        {R"(')", tk::literal::string::single, state_action::push_state("sqs")},

        // Regex literals (simplified)
        {R"(/(?![/*])[^/\n]+/[gimsuy]*)", tk::literal::string::regex, state_action::none()},

        // Keywords
        {R"(\b(?:async|await|break|case|catch|class|const|continue|debugger|default|delete|do|else|export|extends|finally|for|from|function|if|import|in|instanceof|let|new|of|return|static|super|switch|this|throw|try|typeof|var|void|while|with|yield)\b)",
         tk::keyword::self, state_action::none()},

        // Boolean / null / undefined
        {R"(\b(?:true|false|null|undefined|NaN|Infinity)\b)", tk::keyword::constant, state_action::none()},

        // Builtin objects
        {R"(\b(?:Array|Boolean|Date|Error|Function|JSON|Map|Math|Number|Object|Promise|Proxy|Reflect|RegExp|Set|String|Symbol|WeakMap|WeakSet|console|document|window|globalThis)\b)",
         tk::name::builtin, state_action::none()},

        // Floating point
        {R"(\b\d+\.\d*(?:[eE][+-]?\d+)?\b)", tk::literal::number::float_, state_action::none()},
        {R"(\b\d+[eE][+-]?\d+\b)", tk::literal::number::float_, state_action::none()},

        // Hex
        {R"(\b0[xX][0-9a-fA-F_]+n?\b)", tk::literal::number::hex, state_action::none()},

        // Oct
        {R"(\b0[oO][0-7_]+n?\b)", tk::literal::number::oct, state_action::none()},

        // Bin
        {R"(\b0[bB][01_]+n?\b)", tk::literal::number::bin, state_action::none()},

        // Integer (including BigInt)
        {R"(\b[0-9][0-9_]*n?\b)", tk::literal::number::integer, state_action::none()},

        // Arrow / operators
        {R"(=>)", tk::operator_::self, state_action::none()},
        {R"([+\-*/%&|^~<>=!?:;]+)", tk::operator_::self, state_action::none()},

        // Punctuation
        {R"([\[\](){}.,])", tk::punctuation::self, state_action::none()},

        // Decorators
        {R"(@[a-zA-Z_]\w*)", tk::name::decorator, state_action::none()},

        // Identifiers
        {R"([a-zA-Z_$]\w*)", tk::name::self, state_action::none()},
    };

    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };

    rules["dqs"] = {
        {R"(\\[\\'"abfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\u\{[0-9a-fA-F]+\})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };

    rules["sqs"] = {
        {R"(\\[\\'"abfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\u\{[0-9a-fA-F]+\})", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^'\\]+)", tk::literal::string::single, state_action::none()},
        {R"(\\.)", tk::literal::string::single, state_action::none()},
    };

    rules["template"] = {
        {R"(\\[\\`])", tk::literal::string::escape, state_action::none()},
        {R"(`)", tk::literal::string::backtick, state_action::pop_state()},
        {R"(\$\{)", tk::literal::string::interpol, state_action::none()},
        {R"([^`\\$]+)", tk::literal::string::backtick, state_action::none()},
        {R"(\$)", tk::literal::string::backtick, state_action::none()},
        {R"(\\.)", tk::literal::string::backtick, state_action::none()},
    };

    return rules;
}

SPEARMINT_API void register_javascript_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<javascript_lexer>(); }, js_info);
}

} // namespace spearmint::lexers

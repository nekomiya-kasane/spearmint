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
            .name = "javascript",
            .display_name = "JavaScript",
            .aliases = {aliases},
            .filenames = {filenames},
            .mime_types = {mimes},
            .url = "https://developer.mozilla.org/en-US/docs/Web/JavaScript",
            .priority = 10,
        };

    } // namespace

    const lexer_info &javascript_lexer::info() const noexcept {
        return js_info;
    }

    float javascript_lexer::analyse_text(std::string_view source) const noexcept {
        float score = 0.0f;
        if (source.find("function ") != std::string_view::npos) {
            score += 0.1f;
        }
        if (source.find("const ") != std::string_view::npos) {
            score += 0.05f;
        }
        if (source.find("let ") != std::string_view::npos) {
            score += 0.05f;
        }
        if (source.find("var ") != std::string_view::npos) {
            score += 0.05f;
        }
        if (source.find("=>") != std::string_view::npos) {
            score += 0.1f;
        }
        if (source.find("console.log") != std::string_view::npos) {
            score += 0.15f;
        }
        if (source.find("require(") != std::string_view::npos) {
            score += 0.1f;
        }
        if (source.find("import ") != std::string_view::npos) {
            score += 0.05f;
        }
        return score > 1.0f ? 1.0f : score;
    }

    state_map javascript_lexer::get_rules() const {
        namespace tk = token;
        state_map rules;

        rules["root"] = {
            {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"([ \t\r]+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},

            // Single-line comments
            {.pattern = R"(//[^\n]*)",
             .token = tk::comment::single,
             .action = state_action::none(),
             .group_tokens = {}},

            // Multi-line comments
            {.pattern = R"(/\*)",
             .token = tk::comment::multiline,
             .action = state_action::push_state("comment"),
             .group_tokens = {}},

            // Template literals
            {.pattern = R"(`)",
             .token = tk::literal::string::backtick,
             .action = state_action::push_state("template"),
             .group_tokens = {}},

            // Strings
            {.pattern = R"(")",
             .token = tk::literal::string::double_,
             .action = state_action::push_state("dqs"),
             .group_tokens = {}},
            {.pattern = R"(')",
             .token = tk::literal::string::single,
             .action = state_action::push_state("sqs"),
             .group_tokens = {}},

            // Regex literals (simplified)
            {.pattern = R"(/(?![/*])[^/\n]+/[gimsuy]*)",
             .token = tk::literal::string::regex,
             .action = state_action::none(),
             .group_tokens = {}},

            // Keywords
            {.pattern =
                 R"(\b(?:async|await|break|case|catch|class|const|continue|debugger|default|delete|do|else|export|extends|finally|for|from|function|if|import|in|instanceof|let|new|of|return|static|super|switch|this|throw|try|typeof|var|void|while|with|yield)\b)",
             .token = tk::keyword::self,
             .action = state_action::none(),
             .group_tokens = {}},

            // Boolean / null / undefined
            {.pattern = R"(\b(?:true|false|null|undefined|NaN|Infinity)\b)",
             .token = tk::keyword::constant,
             .action = state_action::none(),
             .group_tokens = {}},

            // Builtin objects
            {.pattern =
                 R"(\b(?:Array|Boolean|Date|Error|Function|JSON|Map|Math|Number|Object|Promise|Proxy|Reflect|RegExp|Set|String|Symbol|WeakMap|WeakSet|console|document|window|globalThis)\b)",
             .token = tk::name::builtin,
             .action = state_action::none(),
             .group_tokens = {}},

            // Floating point
            {.pattern = R"(\b\d+\.\d*(?:[eE][+-]?\d+)?\b)",
             .token = tk::literal::number::float_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\b\d+[eE][+-]?\d+\b)",
             .token = tk::literal::number::float_,
             .action = state_action::none(),
             .group_tokens = {}},

            // Hex
            {.pattern = R"(\b0[xX][0-9a-fA-F_]+n?\b)",
             .token = tk::literal::number::hex,
             .action = state_action::none(),
             .group_tokens = {}},

            // Oct
            {.pattern = R"(\b0[oO][0-7_]+n?\b)",
             .token = tk::literal::number::oct,
             .action = state_action::none(),
             .group_tokens = {}},

            // Bin
            {.pattern = R"(\b0[bB][01_]+n?\b)",
             .token = tk::literal::number::bin,
             .action = state_action::none(),
             .group_tokens = {}},

            // Integer (including BigInt)
            {.pattern = R"(\b[0-9][0-9_]*n?\b)",
             .token = tk::literal::number::integer,
             .action = state_action::none(),
             .group_tokens = {}},

            // Arrow / operators
            {.pattern = R"(=>)", .token = tk::operator_::self, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"([+\-*/%&|^~<>=!?:;]+)",
             .token = tk::operator_::self,
             .action = state_action::none(),
             .group_tokens = {}},

            // Punctuation
            {.pattern = R"([\[\](){}.,])",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},

            // Decorators
            {.pattern = R"(@[a-zA-Z_]\w*)",
             .token = tk::name::decorator,
             .action = state_action::none(),
             .group_tokens = {}},

            // Identifiers
            {.pattern = R"([a-zA-Z_$]\w*)",
             .token = tk::name::self,
             .action = state_action::none(),
             .group_tokens = {}},
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

        rules["dqs"] = {
            {.pattern = R"(\\[\\'"abfnrtv0])",
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

        rules["sqs"] = {
            {.pattern = R"(\\[\\'"abfnrtv0])",
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
            {.pattern = R"(')",
             .token = tk::literal::string::single,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"([^'\\]+)",
             .token = tk::literal::string::single,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\\.)",
             .token = tk::literal::string::single,
             .action = state_action::none(),
             .group_tokens = {}},
        };

        rules["template"] = {
            {.pattern = R"(\\[\\`])",
             .token = tk::literal::string::escape,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(`)",
             .token = tk::literal::string::backtick,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"(\$\{)",
             .token = tk::literal::string::interpol,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([^`\\$]+)",
             .token = tk::literal::string::backtick,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$)",
             .token = tk::literal::string::backtick,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\\.)",
             .token = tk::literal::string::backtick,
             .action = state_action::none(),
             .group_tokens = {}},
        };

        return rules;
    }

    SPEARMINT_API void register_javascript_lexer() {
        register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<javascript_lexer>(); }, js_info);
    }

} // namespace spearmint::lexers

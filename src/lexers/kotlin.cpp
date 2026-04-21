#include "spearmint/lexers/kotlin.h"

namespace spearmint::lexers {

    namespace {
        constexpr const char *aliases[] = {"kotlin", "kt"};
        constexpr const char *filenames[] = {"*.kt", "*.kts"};
        constexpr const char *mimes[] = {"text/x-kotlin"};
        const lexer_info kt_info = {
            .name = "kotlin",
            .display_name = "Kotlin",
            .aliases = {aliases},
            .filenames = {filenames},
            .mime_types = {mimes},
            .url = "https://kotlinlang.org",
            .priority = 10,
        };
    } // namespace

    const lexer_info &kotlin_lexer::info() const noexcept {
        return kt_info;
    }

    float kotlin_lexer::analyse_text(std::string_view src) const noexcept {
        float score = 0.0f;
        if (src.find("fun ") != src.npos) {
            score += 0.3f;
        }
        if (src.find("val ") != src.npos || src.find("var ") != src.npos) {
            score += 0.1f;
        }
        if (src.find("package ") != src.npos) {
            score += 0.1f;
        }
        if (src.find("println(") != src.npos) {
            score += 0.2f;
        }
        return score > 1.0f ? 1.0f : score;
    }

    state_map kotlin_lexer::get_rules() const {
        namespace tk = token;
        state_map rules;
        rules["root"] = {
            {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none()},
            {.pattern = R"(//[^\n]*)", .token = tk::comment::single, .action = state_action::none()},
            {.pattern = R"(/\*)", .token = tk::comment::multiline, .action = state_action::push_state("comment")},
            {.pattern = R"(@[a-zA-Z_]\w*)", .token = tk::name::decorator, .action = state_action::none()},
            {.pattern =
                 R"(\b(abstract|actual|annotation|as|break|by|catch|class|companion|const|constructor|continue|crossinline|data|delegate|do|else|enum|expect|external|final|finally|for|fun|get|if|import|in|infix|init|inline|inner|interface|internal|is|lateinit|noinline|object|open|operator|out|override|package|private|protected|public|reified|return|sealed|set|super|suspend|tailrec|this|throw|try|typealias|val|var|vararg|when|where|while|yield)\b)",
             .token = tk::keyword::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\b(true|false|null)\b)",
             .token = tk::keyword::constant,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern =
                 R"(\b(Boolean|Byte|Char|Double|Float|Int|Long|Short|String|Unit|Any|Nothing|Array|List|Map|Set|Pair|Triple|Sequence)\b)",
             .token = tk::keyword::type,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern =
                 R"(\b(println|print|readLine|require|check|error|TODO|listOf|mapOf|setOf|arrayOf|mutableListOf|mutableMapOf|mutableSetOf|lazy|run|let|also|apply|with|repeat|buildString|buildList)\b)",
             .token = tk::name::builtin,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(0x[0-9a-fA-F_]+[uUlL]*)",
             .token = tk::literal::number::hex,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(0b[01_]+[uUlL]*)",
             .token = tk::literal::number::bin,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[fF]?)",
             .token = tk::literal::number::float_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([0-9][0-9_]*[uUlL]*)",
             .token = tk::literal::number::integer,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(""")",
             .token = tk::literal::string::double_,
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
            {.pattern = R"(=>|->|\.\.|\?\.|!!|&&|\|\||[+\-*/%&|^~!<>=]=?|\?:?|::)",
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
            {.pattern = R"(/\*)",
             .token = tk::comment::multiline,
             .action = state_action::push_state("comment"),
             .group_tokens = {}},
            {.pattern = R"([^/*]+)",
             .token = tk::comment::multiline,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([/*])", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        };
        rules["string"] = {
            {.pattern = R"(\\[\\'"$bfnrt])",
             .token = tk::literal::string::escape,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\\u[0-9a-fA-F]{4})",
             .token = tk::literal::string::escape,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$\{)",
             .token = tk::literal::string::interpol,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$[a-zA-Z_]\w*)",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(")",
             .token = tk::literal::string::double_,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"([^"\\$]+)",
             .token = tk::literal::string::double_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([$\\])",
             .token = tk::literal::string::double_,
             .action = state_action::none(),
             .group_tokens = {}},
        };
        rules["rawstring"] = {
            {.pattern = R"(\$\{)",
             .token = tk::literal::string::interpol,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$[a-zA-Z_]\w*)",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(""")",
             .token = tk::literal::string::double_,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"([^"$]+)",
             .token = tk::literal::string::double_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(["$])",
             .token = tk::literal::string::double_,
             .action = state_action::none(),
             .group_tokens = {}},
        };
        return rules;
    }

    SPEARMINT_API void register_kotlin_lexer() {
        register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<kotlin_lexer>(); }, kt_info);
    }

} // namespace spearmint::lexers

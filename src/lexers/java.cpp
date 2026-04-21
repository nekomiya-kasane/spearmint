#include "spearmint/lexers/java.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"java"};
constexpr const char *filenames[] = {"*.java"};
constexpr const char *mimes[] = {"text/x-java"};
const lexer_info java_info = {
    .name = "java",
    .display_name = "Java",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://www.java.com",
    .priority = 10,
};
} // namespace

const lexer_info &java_lexer::info() const noexcept {
    return java_info;
}

float java_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("public class ") != src.npos) score += 0.4f;
    if (src.find("import java.") != src.npos) score += 0.3f;
    if (src.find("System.out.") != src.npos) score += 0.2f;
    return score > 1.0f ? 1.0f : score;
}

state_map java_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(//[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern = R"(@[a-zA-Z_]\w*)",
         .token = tk::name::decorator,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(abstract|assert|break|case|catch|class|const|continue|default|do|else|enum|extends|final|finally|for|goto|if|implements|import|instanceof|interface|native|new|package|private|protected|public|return|static|strictfp|super|switch|synchronized|this|throw|throws|transient|try|volatile|while|yield|var|record|sealed|permits|non-sealed)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(boolean|byte|char|double|float|int|long|short|void)\b)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(true|false|null)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(String|Integer|Long|Double|Float|Boolean|Character|Byte|Short|Object|Class|System|Math|Thread|Runnable|Exception|RuntimeException|ArrayList|HashMap|List|Map|Set|Optional|Stream|Collections|Arrays)\b)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0x[0-9a-fA-F_]+[lL]?)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0b[01_]+[lL]?)",
         .token = tk::literal::number::bin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0[0-7_]+[lL]?)",
         .token = tk::literal::number::oct,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*\.[0-9_]*([eE][+-]?[0-9_]+)?[fFdD]?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*[lL]?)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
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
        {.pattern = R"(&&|\|\||<<|>>>?|[+\-*/%&|^~!<>=]=?|\?|::)",
         .token = tk::operator_::self,
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
    rules["string"] = {
        {.pattern = R"(\\[\\'"bfnrt])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\u[0-9a-fA-F]{4})",
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
    return rules;
}

SPEARMINT_API void register_java_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<java_lexer>(); }, java_info);
}

} // namespace spearmint::lexers

#include "spearmint/lexers/java.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"java"};
constexpr const char* filenames[] = {"*.java"};
constexpr const char* mimes[] = {"text/x-java"};
const lexer_info java_info = {
    "java", "Java",
    {aliases}, {filenames}, {mimes},
    "https://www.java.com", 10,
};
}

const lexer_info& java_lexer::info() const noexcept { return java_info; }

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
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(@[a-zA-Z_]\w*)", tk::name::decorator, state_action::none()},
        {R"(\b(abstract|assert|break|case|catch|class|const|continue|default|do|else|enum|extends|final|finally|for|goto|if|implements|import|instanceof|interface|native|new|package|private|protected|public|return|static|strictfp|super|switch|synchronized|this|throw|throws|transient|try|volatile|while|yield|var|record|sealed|permits|non-sealed)\b)", tk::keyword::self, state_action::none()},
        {R"(\b(boolean|byte|char|double|float|int|long|short|void)\b)", tk::keyword::type, state_action::none()},
        {R"(\b(true|false|null)\b)", tk::keyword::constant, state_action::none()},
        {R"(\b(String|Integer|Long|Double|Float|Boolean|Character|Byte|Short|Object|Class|System|Math|Thread|Runnable|Exception|RuntimeException|ArrayList|HashMap|List|Map|Set|Optional|Stream|Collections|Arrays)\b)", tk::name::builtin, state_action::none()},
        {R"(0x[0-9a-fA-F_]+[lL]?)", tk::literal::number::hex, state_action::none()},
        {R"(0b[01_]+[lL]?)", tk::literal::number::bin, state_action::none()},
        {R"(0[0-7_]+[lL]?)", tk::literal::number::oct, state_action::none()},
        {R"([0-9][0-9_]*\.[0-9_]*([eE][+-]?[0-9_]+)?[fFdD]?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*[lL]?)", tk::literal::number::integer, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},
        {R"('[^'\\]')", tk::literal::string::char_, state_action::none()},
        {R"('\\.')", tk::literal::string::char_, state_action::none()},
        {R"(&&|\|\||<<|>>>?|[+\-*/%&|^~!<>=]=?|\?|::)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;:])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_$]\w*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };
    rules["string"] = {
        {R"(\\[\\'"bfnrt])", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_java_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<java_lexer>();
    }, java_info);
}

}

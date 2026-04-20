#include "spearmint/lexers/kotlin.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"kotlin", "kt"};
constexpr const char* filenames[] = {"*.kt", "*.kts"};
constexpr const char* mimes[] = {"text/x-kotlin"};
const lexer_info kt_info = {
    "kotlin", "Kotlin",
    {aliases}, {filenames}, {mimes},
    "https://kotlinlang.org", 10,
};
}

const lexer_info& kotlin_lexer::info() const noexcept { return kt_info; }

float kotlin_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("fun ") != src.npos) score += 0.3f;
    if (src.find("val ") != src.npos || src.find("var ") != src.npos) score += 0.1f;
    if (src.find("package ") != src.npos) score += 0.1f;
    if (src.find("println(") != src.npos) score += 0.2f;
    return score > 1.0f ? 1.0f : score;
}

state_map kotlin_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(@[a-zA-Z_]\w*)", tk::name::decorator, state_action::none()},
        {R"(\b(abstract|actual|annotation|as|break|by|catch|class|companion|const|constructor|continue|crossinline|data|delegate|do|else|enum|expect|external|final|finally|for|fun|get|if|import|in|infix|init|inline|inner|interface|internal|is|lateinit|noinline|object|open|operator|out|override|package|private|protected|public|reified|return|sealed|set|super|suspend|tailrec|this|throw|try|typealias|val|var|vararg|when|where|while|yield)\b)", tk::keyword::self, state_action::none()},
        {R"(\b(true|false|null)\b)", tk::keyword::constant, state_action::none()},
        {R"(\b(Boolean|Byte|Char|Double|Float|Int|Long|Short|String|Unit|Any|Nothing|Array|List|Map|Set|Pair|Triple|Sequence)\b)", tk::keyword::type, state_action::none()},
        {R"(\b(println|print|readLine|require|check|error|TODO|listOf|mapOf|setOf|arrayOf|mutableListOf|mutableMapOf|mutableSetOf|lazy|run|let|also|apply|with|repeat|buildString|buildList)\b)", tk::name::builtin, state_action::none()},
        {R"(0x[0-9a-fA-F_]+[uUlL]*)", tk::literal::number::hex, state_action::none()},
        {R"(0b[01_]+[uUlL]*)", tk::literal::number::bin, state_action::none()},
        {R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[fF]?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*[uUlL]*)", tk::literal::number::integer, state_action::none()},
        {R"(""")", tk::literal::string::double_, state_action::push_state("rawstring")},
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},
        {R"('[^'\\]')", tk::literal::string::char_, state_action::none()},
        {R"('\\.')", tk::literal::string::char_, state_action::none()},
        {R"(=>|->|\.\.|\?\.|!!|&&|\|\||[+\-*/%&|^~!<>=]=?|\?:?|::)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;:])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_]\w*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"([^/*]+)", tk::comment::multiline, state_action::none()},
        {R"([/*])", tk::comment::multiline, state_action::none()},
    };
    rules["string"] = {
        {R"(\\[\\'"$bfnrt])", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\$\{)", tk::literal::string::interpol, state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\$]+)", tk::literal::string::double_, state_action::none()},
        {R"([$\\])", tk::literal::string::double_, state_action::none()},
    };
    rules["rawstring"] = {
        {R"(\$\{)", tk::literal::string::interpol, state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(""")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"$]+)", tk::literal::string::double_, state_action::none()},
        {R"(["$])", tk::literal::string::double_, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_kotlin_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<kotlin_lexer>();
    }, kt_info);
}

}

#include "spearmint/lexers/csharp.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"csharp", "c#", "cs"};
constexpr const char* filenames[] = {"*.cs", "*.csx"};
constexpr const char* mimes[] = {"text/x-csharp"};
const lexer_info cs_info = {
    "csharp", "C#",
    {aliases}, {filenames}, {mimes},
    "https://learn.microsoft.com/dotnet/csharp", 10,
};
}

const lexer_info& csharp_lexer::info() const noexcept { return cs_info; }

float csharp_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("using System") != src.npos) score += 0.4f;
    if (src.find("namespace ") != src.npos) score += 0.1f;
    if (src.find("Console.") != src.npos) score += 0.2f;
    return score > 1.0f ? 1.0f : score;
}

state_map csharp_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(\[)", tk::punctuation::self, state_action::none()},
        {R"(\b(abstract|as|base|break|case|catch|checked|class|const|continue|default|delegate|do|else|enum|event|explicit|extern|finally|fixed|for|foreach|goto|if|implicit|in|interface|internal|is|lock|namespace|new|operator|out|override|params|partial|private|protected|public|readonly|record|ref|return|sealed|sizeof|stackalloc|static|struct|switch|this|throw|try|typeof|unchecked|unsafe|using|var|virtual|void|volatile|when|while|yield|async|await|get|set|init|required|global|where|select|from|orderby|group|into|join|let|on|equals|ascending|descending)\b)", tk::keyword::self, state_action::none()},
        {R"(\b(bool|byte|char|decimal|double|float|int|long|nint|nuint|object|sbyte|short|string|uint|ulong|ushort|dynamic)\b)", tk::keyword::type, state_action::none()},
        {R"(\b(true|false|null)\b)", tk::keyword::constant, state_action::none()},
        {R"(\b(Console|Math|String|Array|List|Dictionary|Task|Func|Action|Tuple|Nullable|Exception|Object|Type|Enum|Convert|Environment|GC|DateTime|TimeSpan|Guid)\b)", tk::name::builtin, state_action::none()},
        {R"(0x[0-9a-fA-F_]+[uUlL]*)", tk::literal::number::hex, state_action::none()},
        {R"(0b[01_]+[uUlL]*)", tk::literal::number::bin, state_action::none()},
        {R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[fFdDmM]?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*[uUlL]*)", tk::literal::number::integer, state_action::none()},
        {R"(@")", tk::literal::string::double_, state_action::push_state("verbatim_string")},
        {R"(\$")", tk::literal::string::double_, state_action::push_state("interp_string")},
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},
        {R"('[^'\\]')", tk::literal::string::char_, state_action::none()},
        {R"('\\.')", tk::literal::string::char_, state_action::none()},
        {R"(=>|&&|\|\||<<|>>|[+\-*/%&|^~!<>=]=?|\?\?=?|\?\.?|::)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;:])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_@]\w*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };
    rules["string"] = {
        {R"(\\[\\'"0abfnrtv])", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\U[0-9a-fA-F]{8})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["verbatim_string"] = {
        {R"("")", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"]+)", tk::literal::string::double_, state_action::none()},
    };
    rules["interp_string"] = {
        {R"(\\[\\'"0abfnrtv])", tk::literal::string::escape, state_action::none()},
        {R"(\{)", tk::literal::string::interpol, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\{]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_csharp_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<csharp_lexer>();
    }, cs_info);
}

}

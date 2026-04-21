#include "spearmint/lexers/csharp.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"csharp", "c#", "cs"};
constexpr const char *filenames[] = {"*.cs", "*.csx"};
constexpr const char *mimes[] = {"text/x-csharp"};
const lexer_info cs_info = {
    .name = "csharp",
    .display_name = "C#",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://learn.microsoft.com/dotnet/csharp",
    .priority = 10,
};
} // namespace

const lexer_info &csharp_lexer::info() const noexcept {
    return cs_info;
}

float csharp_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("using System") != src.npos) {
        score += 0.4f;
    }
    if (src.find("namespace ") != src.npos) {
        score += 0.1f;
    }
    if (src.find("Console.") != src.npos) {
        score += 0.2f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map csharp_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(//[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern = R"(\[)", .token = tk::punctuation::self, .action = state_action::none(), .group_tokens = {}},
        {.pattern =
             R"(\b(abstract|as|base|break|case|catch|checked|class|const|continue|default|delegate|do|else|enum|event|explicit|extern|finally|fixed|for|foreach|goto|if|implicit|in|interface|internal|is|lock|namespace|new|operator|out|override|params|partial|private|protected|public|readonly|record|ref|return|sealed|sizeof|stackalloc|static|struct|switch|this|throw|try|typeof|unchecked|unsafe|using|var|virtual|void|volatile|when|while|yield|async|await|get|set|init|required|global|where|select|from|orderby|group|into|join|let|on|equals|ascending|descending)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(bool|byte|char|decimal|double|float|int|long|nint|nuint|object|sbyte|short|string|uint|ulong|ushort|dynamic)\b)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(true|false|null)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(Console|Math|String|Array|List|Dictionary|Task|Func|Action|Tuple|Nullable|Exception|Object|Type|Enum|Convert|Environment|GC|DateTime|TimeSpan|Guid)\b)",
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
        {.pattern = R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?[fFdDmM]?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*[uUlL]*)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(@")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("verbatim_string"),
         .group_tokens = {}},
        {.pattern = R"(\$")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("interp_string"),
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
        {.pattern = R"(=>|&&|\|\||<<|>>|[+\-*/%&|^~!<>=]=?|\?\?=?|\?\.?|::)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\[\](){}.,;:])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([a-zA-Z_@]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
    };
    rules["comment"] = {
        {.pattern = R"(\*/)", .token = tk::comment::multiline, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"([^*]+)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\*)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
    };
    rules["string"] = {
        {.pattern = R"(\\[\\'"0abfnrtv])",
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
    rules["verbatim_string"] = {
        {.pattern = R"("")", .token = tk::literal::string::escape, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^"]+)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    rules["interp_string"] = {
        {.pattern = R"(\\[\\'"0abfnrtv])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\{)",
         .token = tk::literal::string::interpol,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^"\\{]+)",
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

SPEARMINT_API void register_csharp_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<csharp_lexer>(); }, cs_info);
}

} // namespace spearmint::lexers

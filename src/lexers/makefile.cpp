#include "spearmint/lexers/makefile.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"makefile", "make", "mf"};
constexpr const char *filenames[] = {"Makefile", "makefile", "GNUmakefile", "*.mk", "*.mak"};
constexpr const char *mimes[] = {"text/x-makefile"};
const lexer_info mf_info = {
    "makefile", "Makefile", {aliases}, {filenames}, {mimes}, "https://www.gnu.org/software/make/", 10,
};
} // namespace

const lexer_info &makefile_lexer::info() const noexcept {
    return mf_info;
}

float makefile_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("\t") != src.npos && src.find(":") != src.npos) score += 0.2f;
    if (src.find("$(") != src.npos) score += 0.2f;
    if (src.find(".PHONY") != src.npos) score += 0.5f;
    return score > 1.0f ? 1.0f : score;
}

state_map makefile_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},
        {R"(\b(ifeq|ifneq|ifdef|ifndef|else|endif|include|-include|sinclude|override|export|unexport|define|endef|undefine|vpath)\b)",
         tk::keyword::self, state_action::none()},
        {R"(\$\([^)]+\))", tk::name::variable, state_action::none()},
        {R"(\$\{[^}]+\})", tk::name::variable, state_action::none()},
        {R"(\$[@<^+?*%])", tk::name::variable, state_action::none()},
        {R"(\$\$)", tk::name::variable, state_action::none()},
        {R"(\b(subst|patsubst|strip|findstring|filter|filter-out|sort|word|wordlist|words|firstword|lastword|dir|notdir|suffix|basename|addsuffix|addprefix|join|wildcard|realpath|abspath|error|warning|info|shell|origin|flavor|foreach|if|or|and|call|eval|file|value)\b)",
         tk::name::builtin, state_action::none()},
        {R"(^[a-zA-Z_][\w.-]*\s*(?=[:+?]?=))", tk::name::variable, state_action::none()},
        {R"(^[^\s:=#]+(?=\s*:))", tk::name::label, state_action::none()},
        {R"([:+?]?=)", tk::operator_::self, state_action::none()},
        {R"([:;|@\-])", tk::punctuation::self, state_action::none()},
        {R"("[^"]*")", tk::literal::string::double_, state_action::none()},
        {R"('[^']*')", tk::literal::string::single, state_action::none()},
        {R"(\\$)", tk::punctuation::self, state_action::none()},
        {R"([^\s#$"'\\:;|@=]+)", tk::text, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_makefile_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<makefile_lexer>(); }, mf_info);
}

} // namespace spearmint::lexers

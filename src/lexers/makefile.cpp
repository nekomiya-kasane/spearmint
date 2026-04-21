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
        if (src.find("\t") != src.npos && src.find(":") != src.npos) {
            score += 0.2f;
        }
        if (src.find("$(") != src.npos) {
            score += 0.2f;
        }
        if (src.find(".PHONY") != src.npos) {
            score += 0.5f;
        }
        return score > 1.0f ? 1.0f : score;
    }

    state_map makefile_lexer::get_rules() const {
        namespace tk = token;
        state_map rules;
        rules["root"] = {
            {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
            {.pattern =
                 R"(\b(ifeq|ifneq|ifdef|ifndef|else|endif|include|-include|sinclude|override|export|unexport|define|endef|undefine|vpath)\b)",
             .token = tk::keyword::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$\([^)]+\))",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$\{[^}]+\})",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$[@<^+?*%])",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$\$)", .token = tk::name::variable, .action = state_action::none(), .group_tokens = {}},
            {.pattern =
                 R"(\b(subst|patsubst|strip|findstring|filter|filter-out|sort|word|wordlist|words|firstword|lastword|dir|notdir|suffix|basename|addsuffix|addprefix|join|wildcard|realpath|abspath|error|warning|info|shell|origin|flavor|foreach|if|or|and|call|eval|file|value)\b)",
             .token = tk::name::builtin,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(^[a-zA-Z_][\w.-]*\s*(?=[:+?]?=))",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(^[^\s:=#]+(?=\s*:))",
             .token = tk::name::label,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([:+?]?=)", .token = tk::operator_::self, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"([:;|@\-])",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"("[^"]*")",
             .token = tk::literal::string::double_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"('[^']*')",
             .token = tk::literal::string::single,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\\$)", .token = tk::punctuation::self, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"([^\s#$"'\\:;|@=]+)", .token = tk::text, .action = state_action::none(), .group_tokens = {}},
        };
        return rules;
    }

    SPEARMINT_API void register_makefile_lexer() {
        register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<makefile_lexer>(); }, mf_info);
    }

} // namespace spearmint::lexers

#include "spearmint/lexers/dockerfile.h"

namespace spearmint::lexers {

    namespace {
        constexpr const char *aliases[] = {"dockerfile", "docker"};
        constexpr const char *filenames[] = {"Dockerfile", "Dockerfile.*", "*.dockerfile"};
        constexpr const char *mimes[] = {"text/x-dockerfile-config"};
        const lexer_info df_info = {
            "dockerfile", "Dockerfile", {aliases},
            {filenames},  {mimes},      "https://docs.docker.com/reference/dockerfile/",
            10,
        };
    } // namespace

    const lexer_info &dockerfile_lexer::info() const noexcept {
        return df_info;
    }

    float dockerfile_lexer::analyse_text(std::string_view src) const noexcept {
        float score = 0.0f;
        if (src.find("FROM ") != src.npos) {
            score += 0.4f;
        }
        if (src.find("RUN ") != src.npos) {
            score += 0.2f;
        }
        if (src.find("COPY ") != src.npos || src.find("ADD ") != src.npos) {
            score += 0.1f;
        }
        if (src.find("ENTRYPOINT") != src.npos || src.find("CMD") != src.npos) {
            score += 0.1f;
        }
        return score > 1.0f ? 1.0f : score;
    }

    state_map dockerfile_lexer::get_rules() const {
        namespace tk = token;
        state_map rules;
        rules["root"] = {
            {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
            {.pattern =
                 R"(\b(?i)(FROM|AS|MAINTAINER|RUN|CMD|LABEL|EXPOSE|ENV|ADD|COPY|ENTRYPOINT|VOLUME|USER|WORKDIR|ARG|ONBUILD|STOPSIGNAL|HEALTHCHECK|SHELL)\b)",
             .token = tk::keyword::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$\{[^}]+\})",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$[a-zA-Z_]\w*)",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(")",
             .token = tk::literal::string::double_,
             .action = state_action::push_state("string"),
             .group_tokens = {}},
            {.pattern = R"('[^']*')",
             .token = tk::literal::string::single,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([0-9]+)",
             .token = tk::literal::number::integer,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\\$)", .token = tk::punctuation::self, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(&&|\|\||[|;])",
             .token = tk::operator_::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([\[\]=:])",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([^\s#"'$\\\[\]=:;|&]+)",
             .token = tk::text,
             .action = state_action::none(),
             .group_tokens = {}},
        };
        rules["string"] = {
            {.pattern = R"(\\[\\"])",
             .token = tk::literal::string::escape,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$\{[^}]+\})",
             .token = tk::name::variable,
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
        return rules;
    }

    SPEARMINT_API void register_dockerfile_lexer() {
        register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<dockerfile_lexer>(); }, df_info);
    }

} // namespace spearmint::lexers

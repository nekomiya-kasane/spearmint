#include "spearmint/lexers/dockerfile.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"dockerfile", "docker"};
constexpr const char* filenames[] = {"Dockerfile", "Dockerfile.*", "*.dockerfile"};
constexpr const char* mimes[] = {"text/x-dockerfile-config"};
const lexer_info df_info = {
    "dockerfile", "Dockerfile",
    {aliases}, {filenames}, {mimes},
    "https://docs.docker.com/reference/dockerfile/", 10,
};
}

const lexer_info& dockerfile_lexer::info() const noexcept { return df_info; }

float dockerfile_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("FROM ") != src.npos) score += 0.4f;
    if (src.find("RUN ") != src.npos) score += 0.2f;
    if (src.find("COPY ") != src.npos || src.find("ADD ") != src.npos) score += 0.1f;
    if (src.find("ENTRYPOINT") != src.npos || src.find("CMD") != src.npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map dockerfile_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},
        {R"(\b(?i)(FROM|AS|MAINTAINER|RUN|CMD|LABEL|EXPOSE|ENV|ADD|COPY|ENTRYPOINT|VOLUME|USER|WORKDIR|ARG|ONBUILD|STOPSIGNAL|HEALTHCHECK|SHELL)\b)", tk::keyword::self, state_action::none()},
        {R"(\$\{[^}]+\})", tk::name::variable, state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},
        {R"('[^']*')", tk::literal::string::single, state_action::none()},
        {R"([0-9]+)", tk::literal::number::integer, state_action::none()},
        {R"(\\$)", tk::punctuation::self, state_action::none()},
        {R"(&&|\|\||[|;])", tk::operator_::self, state_action::none()},
        {R"([\[\]=:])", tk::punctuation::self, state_action::none()},
        {R"([^\s#"'$\\\[\]=:;|&]+)", tk::text, state_action::none()},
    };
    rules["string"] = {
        {R"(\\[\\"])", tk::literal::string::escape, state_action::none()},
        {R"(\$\{[^}]+\})", tk::name::variable, state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\$]+)", tk::literal::string::double_, state_action::none()},
        {R"([$\\])", tk::literal::string::double_, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_dockerfile_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<dockerfile_lexer>();
    }, df_info);
}

}

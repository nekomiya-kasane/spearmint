#include "spearmint/lexers/bash.h"

namespace spearmint::lexers {

namespace {
constexpr const char* aliases[] = {"bash", "sh", "shell", "zsh"};
constexpr const char* filenames[] = {"*.sh", "*.bash", "*.zsh", ".bashrc", ".zshrc", ".profile"};
constexpr const char* mimes[] = {"application/x-sh", "text/x-shellscript"};
const lexer_info bash_info = {
    "bash", "Bash",
    {aliases}, {filenames}, {mimes},
    "https://www.gnu.org/software/bash/", 10,
};
}

const lexer_info& bash_lexer::info() const noexcept { return bash_info; }

float bash_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.starts_with("#!/bin/bash") || src.starts_with("#!/bin/sh") || src.starts_with("#!/usr/bin/env bash")) score += 0.8f;
    if (src.find("echo ") != src.npos) score += 0.1f;
    if (src.find("fi\n") != src.npos || src.find("done\n") != src.npos) score += 0.2f;
    return score > 1.0f ? 1.0f : score;
}

state_map bash_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(#![^\n]*)", tk::comment::hashbang, state_action::none()},
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},
        {R"(\b(if|then|else|elif|fi|for|while|until|do|done|in|case|esac|function|select|time|coproc)\b)", tk::keyword::self, state_action::none()},
        {R"(\b(echo|printf|read|cd|pwd|ls|cp|mv|rm|mkdir|rmdir|cat|grep|sed|awk|find|sort|uniq|wc|head|tail|cut|tr|tee|xargs|chmod|chown|kill|ps|export|source|alias|unalias|set|unset|shift|exit|return|local|declare|typeset|readonly|eval|exec|trap|wait|test)\b)", tk::name::builtin, state_action::none()},
        {R"(\$\{[^}]+\})", tk::name::variable, state_action::none()},
        {R"(\$\([^)]+\))", tk::literal::string::interpol, state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(\$[0-9#?$!@*-])", tk::name::variable, state_action::none()},
        {R"([0-9]+)", tk::literal::number::integer, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::push_state("dstring")},
        {R"(')", tk::literal::string::single, state_action::push_state("sstring")},
        {R"(&&|\|\||[|&;()<>]|<<|>>|[0-9]*[<>]&?[0-9]*)", tk::operator_::self, state_action::none()},
        {R"([\[\]])", tk::punctuation::self, state_action::none()},
        {R"([^\s'"$#|&;()<>\[\]]+)", tk::text, state_action::none()},
    };
    rules["dstring"] = {
        {R"(\\[\\$"`\n])", tk::literal::string::escape, state_action::none()},
        {R"(\$\{[^}]+\})", tk::name::variable, state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\$]+)", tk::literal::string::double_, state_action::none()},
        {R"([$\\])", tk::literal::string::double_, state_action::none()},
    };
    rules["sstring"] = {
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^']+)", tk::literal::string::single, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_bash_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> {
        return std::make_unique<bash_lexer>();
    }, bash_info);
}

}

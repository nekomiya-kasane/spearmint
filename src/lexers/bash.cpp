#include "spearmint/lexers/bash.h"

#include "spearmint/core/lexer.h"
#include "spearmint/core/lexer_registry.h"
#include "spearmint/core/regex_lexer.h"
#include "spearmint/core/token.h"

#include <array>

namespace spearmint::lexers {

    namespace {
        constexpr std::array<const char *, 4> aliases = {"bash", "sh", "shell", "zsh"};
        constexpr std::array<const char *, 6> filenames = {"*.sh", "*.bash", "*.zsh", ".bashrc", ".zshrc", ".profile"};
        constexpr std::array<const char *, 2> mimes = {"application/x-sh", "text/x-shellscript"};
        const lexer_info bash_info = {
            .name = "bash",
            .display_name = "Bash",
            .aliases = {aliases},
            .filenames = {filenames},
            .mime_types = {mimes},
            .url = "https://www.gnu.org/software/bash/",
            .priority = 10,
        };
    } // namespace

    const lexer_info &bash_lexer::info() const noexcept {
        return bash_info;
    }

    float bash_lexer::analyse_text(std::string_view src) const noexcept {
        float score = 0.0f;
        if (src.starts_with("#!/bin/bash") || src.starts_with("#!/bin/sh") || src.starts_with("#!/usr/bin/env bash")) {
            score += 0.8f;
        }
        if (src.find("echo ") != src.npos) {
            score += 0.1f;
        }
        if (src.find("fi\n") != src.npos || src.find("done\n") != src.npos) {
            score += 0.2f;
        }
        return score > 1.0f ? 1.0f : score;
    }

    state_map bash_lexer::get_rules() const {
        namespace tk = token;
        state_map rules;
        rules["root"] = {
            {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(#![^\n]*)",
             .token = tk::comment::hashbang,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
            {.pattern =
                 R"(\b(if|then|else|elif|fi|for|while|until|do|done|in|case|esac|function|select|time|coproc)\b)",
             .token = tk::keyword::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern =
                 R"(\b(echo|printf|read|cd|pwd|ls|cp|mv|rm|mkdir|rmdir|cat|grep|sed|awk|find|sort|uniq|wc|head|tail|cut|tr|tee|xargs|chmod|chown|kill|ps|export|source|alias|unalias|set|unset|shift|exit|return|local|declare|typeset|readonly|eval|exec|trap|wait|test)\b)",
             .token = tk::name::builtin,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$\{[^}]+\})",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$\([^)]+\))",
             .token = tk::literal::string::interpol,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$[a-zA-Z_]\w*)",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\$[0-9#?$!@*-])",
             .token = tk::name::variable,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([0-9]+)",
             .token = tk::literal::number::integer,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(")",
             .token = tk::literal::string::double_,
             .action = state_action::push_state("dstring"),
             .group_tokens = {}},
            {.pattern = R"(')",
             .token = tk::literal::string::single,
             .action = state_action::push_state("sstring"),
             .group_tokens = {}},
            {.pattern = R"(&&|\|\||[|&;()<>]|<<|>>|[0-9]*[<>]&?[0-9]*)",
             .token = tk::operator_::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([\[\]])",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([^\s'"$#|&;()<>\[\]]+)",
             .token = tk::text,
             .action = state_action::none(),
             .group_tokens = {}},
        };
        rules["dstring"] = {
            {.pattern = R"(\\[\\$"`\n])",
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
        rules["sstring"] = {
            {.pattern = R"(')",
             .token = tk::literal::string::single,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"([^']+)",
             .token = tk::literal::string::single,
             .action = state_action::none(),
             .group_tokens = {}},
        };
        return rules;
    }

    SPEARMINT_API void register_bash_lexer() {
        register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<bash_lexer>(); }, bash_info);
    }

} // namespace spearmint::lexers

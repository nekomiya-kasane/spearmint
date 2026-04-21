#include "spearmint/lexers/php.h"

#include "spearmint/core/token.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"php"};
constexpr const char *filenames[] = {"*.php", "*.phtml", "*.php3", "*.php4", "*.php5"};
constexpr const char *mimes[] = {"text/x-php", "application/x-php"};
const lexer_info php_info = {
    .name = "php",
    .display_name = "PHP",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://www.php.net",
    .priority = 10,
};
} // namespace

const lexer_info &php_lexer::info() const noexcept {
    return php_info;
}

float php_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("<?php") != src.npos) {
        score += 0.5f;
    }
    if (src.find("$") != src.npos) {
        score += 0.05f;
    }
    if (src.find("->") != src.npos) {
        score += 0.05f;
    }
    if (src.find("echo ") != src.npos) {
        score += 0.1f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map php_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(<\?php\b)", .token = tk::comment::preproc, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(<\?=)", .token = tk::comment::preproc, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\?>)", .token = tk::comment::preproc, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(//[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern =
             R"(\b(abstract|and|as|break|callable|case|catch|class|clone|const|continue|declare|default|do|else|elseif|enddeclare|endfor|endforeach|endif|endswitch|endwhile|extends|final|finally|fn|for|foreach|function|global|goto|if|implements|include|include_once|instanceof|insteadof|interface|match|namespace|new|or|private|protected|public|readonly|require|require_once|return|static|switch|throw|trait|try|use|var|while|xor|yield|yield from|enum)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(true|false|null|TRUE|FALSE|NULL)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(int|float|bool|string|array|object|void|never|mixed|self|parent|static|iterable|callable)\b)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(echo|print|die|exit|isset|unset|empty|list|array|new|clone)\b)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\$[a-zA-Z_]\w*)",
         .token = tk::name::variable,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0x[0-9a-fA-F_]+)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0b[01_]+)",
         .token = tk::literal::number::bin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0[0-7_]+)",
         .token = tk::literal::number::oct,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9][0-9_]*)",
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
        {.pattern = R"(=>|->|::|&&|\|\||[+\-*/%&|^~!<>=]=?|\?\?=?|\.\.\.|\?->)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\[\](){}.,;:@])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([a-zA-Z_]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
    };
    rules["comment"] = {
        {.pattern = R"(\*/)", .token = tk::comment::multiline, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"([^*]+)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\*)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
    };
    rules["dstring"] = {
        {.pattern = R"(\\[\\$"nrtv0efx])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\$[a-zA-Z_]\w*)",
         .token = tk::name::variable,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\{\$)",
         .token = tk::literal::string::interpol,
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
        {.pattern = R"(\\')", .token = tk::literal::string::escape, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\\\\)",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(')",
         .token = tk::literal::string::single,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^'\\]+)",
         .token = tk::literal::string::single,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\)", .token = tk::literal::string::single, .action = state_action::none(), .group_tokens = {}},
    };
    return rules;
}

SPEARMINT_API void register_php_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<php_lexer>(); }, php_info);
}

} // namespace spearmint::lexers

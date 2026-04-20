#include "spearmint/lexers/php.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"php"};
constexpr const char *filenames[] = {"*.php", "*.phtml", "*.php3", "*.php4", "*.php5"};
constexpr const char *mimes[] = {"text/x-php", "application/x-php"};
const lexer_info php_info = {
    "php", "PHP", {aliases}, {filenames}, {mimes}, "https://www.php.net", 10,
};
} // namespace

const lexer_info &php_lexer::info() const noexcept {
    return php_info;
}

float php_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("<?php") != src.npos) score += 0.5f;
    if (src.find("$") != src.npos) score += 0.05f;
    if (src.find("->") != src.npos) score += 0.05f;
    if (src.find("echo ") != src.npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map php_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(<\?php\b)", tk::comment::preproc, state_action::none()},
        {R"(<\?=)", tk::comment::preproc, state_action::none()},
        {R"(\?>)", tk::comment::preproc, state_action::none()},
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(\b(abstract|and|as|break|callable|case|catch|class|clone|const|continue|declare|default|do|else|elseif|enddeclare|endfor|endforeach|endif|endswitch|endwhile|extends|final|finally|fn|for|foreach|function|global|goto|if|implements|include|include_once|instanceof|insteadof|interface|match|namespace|new|or|private|protected|public|readonly|require|require_once|return|static|switch|throw|trait|try|use|var|while|xor|yield|yield from|enum)\b)",
         tk::keyword::self, state_action::none()},
        {R"(\b(true|false|null|TRUE|FALSE|NULL)\b)", tk::keyword::constant, state_action::none()},
        {R"(\b(int|float|bool|string|array|object|void|never|mixed|self|parent|static|iterable|callable)\b)",
         tk::keyword::type, state_action::none()},
        {R"(\b(echo|print|die|exit|isset|unset|empty|list|array|new|clone)\b)", tk::name::builtin,
         state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(0x[0-9a-fA-F_]+)", tk::literal::number::hex, state_action::none()},
        {R"(0b[01_]+)", tk::literal::number::bin, state_action::none()},
        {R"(0[0-7_]+)", tk::literal::number::oct, state_action::none()},
        {R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*)", tk::literal::number::integer, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::push_state("dstring")},
        {R"(')", tk::literal::string::single, state_action::push_state("sstring")},
        {R"(=>|->|::|&&|\|\||[+\-*/%&|^~!<>=]=?|\?\?=?|\.\.\.|\?->)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;:@])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_]\w*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };
    rules["dstring"] = {
        {R"(\\[\\$"nrtv0efx])", tk::literal::string::escape, state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(\{\$)", tk::literal::string::interpol, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\$]+)", tk::literal::string::double_, state_action::none()},
        {R"([$\\])", tk::literal::string::double_, state_action::none()},
    };
    rules["sstring"] = {
        {R"(\\')", tk::literal::string::escape, state_action::none()},
        {R"(\\\\)", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^'\\]+)", tk::literal::string::single, state_action::none()},
        {R"(\\)", tk::literal::string::single, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_php_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<php_lexer>(); }, php_info);
}

} // namespace spearmint::lexers

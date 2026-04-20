#include "spearmint/lexers/ruby.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"ruby", "rb"};
constexpr const char *filenames[] = {"*.rb", "*.gemspec", "*.rake", "Rakefile", "Gemfile"};
constexpr const char *mimes[] = {"text/x-ruby", "application/x-ruby"};
const lexer_info ruby_info = {
    "ruby", "Ruby", {aliases}, {filenames}, {mimes}, "https://www.ruby-lang.org", 10,
};
} // namespace

const lexer_info &ruby_lexer::info() const noexcept {
    return ruby_info;
}

float ruby_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("def ") != src.npos) score += 0.2f;
    if (src.find("end\n") != src.npos || src.find("end ") != src.npos) score += 0.1f;
    if (src.find("require ") != src.npos) score += 0.2f;
    if (src.find("puts ") != src.npos) score += 0.1f;
    if (src.find("attr_") != src.npos) score += 0.2f;
    return score > 1.0f ? 1.0f : score;
}

state_map ruby_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},
        {R"(=begin\b.*?=end\b)", tk::comment::multiline, state_action::none()},
        {R"(\b(alias|and|begin|break|case|class|def|defined\?|do|else|elsif|end|ensure|false|for|if|in|module|next|nil|not|or|redo|rescue|retry|return|self|super|then|true|undef|unless|until|when|while|yield|__FILE__|__LINE__|__ENCODING__)\b)",
         tk::keyword::self, state_action::none()},
        {R"(\b(require|require_relative|include|extend|prepend|attr_reader|attr_writer|attr_accessor|puts|print|p|gets|raise|lambda|proc)\b)",
         tk::name::builtin, state_action::none()},
        {R"(:[a-zA-Z_]\w*[!?]?)", tk::literal::string::symbol, state_action::none()},
        {R"(@{1,2}[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(\$[a-zA-Z_]\w*)", tk::name::variable, state_action::none()},
        {R"(0x[0-9a-fA-F_]+)", tk::literal::number::hex, state_action::none()},
        {R"(0b[01_]+)", tk::literal::number::bin, state_action::none()},
        {R"(0o[0-7_]+)", tk::literal::number::oct, state_action::none()},
        {R"([0-9][0-9_]*\.[0-9_]+([eE][+-]?[0-9_]+)?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9][0-9_]*)", tk::literal::number::integer, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::push_state("dstring")},
        {R"(')", tk::literal::string::single, state_action::push_state("sstring")},
        {R"(=>|<=>|&&|\|\||<<|>>|[+\-*/%&|^~!<>=]=?|\.\.|\.\.\.|\?|::)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_]\w*[!?]?)", tk::name::self, state_action::none()},
    };
    rules["dstring"] = {
        {R"(\\[\\'"abfnrstv#])", tk::literal::string::escape, state_action::none()},
        {R"(#\{)", tk::literal::string::interpol, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\#]+)", tk::literal::string::double_, state_action::none()},
        {R"([\\#])", tk::literal::string::double_, state_action::none()},
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

SPEARMINT_API void register_ruby_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<ruby_lexer>(); }, ruby_info);
}

} // namespace spearmint::lexers

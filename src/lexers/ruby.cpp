#include "spearmint/lexers/ruby.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"ruby", "rb"};
constexpr const char *filenames[] = {"*.rb", "*.gemspec", "*.rake", "Rakefile", "Gemfile"};
constexpr const char *mimes[] = {"text/x-ruby", "application/x-ruby"};
const lexer_info ruby_info = {
    .name = "ruby",
    .display_name = "Ruby",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://www.ruby-lang.org",
    .priority = 10,
};
} // namespace

const lexer_info &ruby_lexer::info() const noexcept {
    return ruby_info;
}

float ruby_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("def ") != src.npos) {
        score += 0.2f;
    }
    if (src.find("end\n") != src.npos || src.find("end ") != src.npos) {
        score += 0.1f;
    }
    if (src.find("require ") != src.npos) {
        score += 0.2f;
    }
    if (src.find("puts ") != src.npos) {
        score += 0.1f;
    }
    if (src.find("attr_") != src.npos) {
        score += 0.2f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map ruby_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(=begin\b.*?=end\b)",
         .token = tk::comment::multiline,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(alias|and|begin|break|case|class|def|defined\?|do|else|elsif|end|ensure|false|for|if|in|module|next|nil|not|or|redo|rescue|retry|return|self|super|then|true|undef|unless|until|when|while|yield|__FILE__|__LINE__|__ENCODING__)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(require|require_relative|include|extend|prepend|attr_reader|attr_writer|attr_accessor|puts|print|p|gets|raise|lambda|proc)\b)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(:[a-zA-Z_]\w*[!?]?)",
         .token = tk::literal::string::symbol,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(@{1,2}[a-zA-Z_]\w*)",
         .token = tk::name::variable,
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
        {.pattern = R"(0o[0-7_]+)",
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
        {.pattern = R"(=>|<=>|&&|\|\||<<|>>|[+\-*/%&|^~!<>=]=?|\.\.|\.\.\.|\?|::)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\[\](){}.,;])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([a-zA-Z_]\w*[!?]?)",
         .token = tk::name::self,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    rules["dstring"] = {
        {.pattern = R"(\\[\\'"abfnrstv#])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(#\{)",
         .token = tk::literal::string::interpol,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^"\\#]+)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\\#])",
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

SPEARMINT_API void register_ruby_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<ruby_lexer>(); }, ruby_info);
}

} // namespace spearmint::lexers

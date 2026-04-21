#include "spearmint/lexers/lua.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"lua"};
constexpr const char *filenames[] = {"*.lua"};
constexpr const char *mimes[] = {"text/x-lua", "application/x-lua"};
const lexer_info lua_info = {
    .name = "lua",
    .display_name = "Lua",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://www.lua.org",
    .priority = 10,
};
} // namespace

const lexer_info &lua_lexer::info() const noexcept {
    return lua_info;
}

float lua_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("local ") != src.npos) {
        score += 0.2f;
    }
    if (src.find("function ") != src.npos) {
        score += 0.1f;
    }
    if (src.find("end\n") != src.npos) {
        score += 0.1f;
    }
    if (src.find("then\n") != src.npos || src.find("then ") != src.npos) {
        score += 0.2f;
    }
    if (src.find("require(") != src.npos || src.find("require \"") != src.npos) {
        score += 0.2f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map lua_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(--\[\[)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("mlcomment"),
         .group_tokens = {}},
        {.pattern = R"(--[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern =
             R"(\b(and|break|do|else|elseif|end|false|for|function|goto|if|in|local|nil|not|or|repeat|return|then|true|until|while)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(assert|collectgarbage|dofile|error|getmetatable|ipairs|load|loadfile|next|pairs|pcall|print|rawequal|rawget|rawlen|rawset|require|select|setmetatable|tonumber|tostring|type|xpcall|coroutine|debug|io|math|os|package|string|table|utf8)\b)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0x[0-9a-fA-F]+(\.[0-9a-fA-F]+)?([pP][+-]?[0-9]+)?)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9]+\.[0-9]*([eE][+-]?[0-9]+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\.[0-9]+([eE][+-]?[0-9]+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9]+([eE][+-]?[0-9]+))",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([0-9]+)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\[\[)",
         .token = tk::literal::string::self,
         .action = state_action::push_state("mlstring"),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("dstring"),
         .group_tokens = {}},
        {.pattern = R"(')",
         .token = tk::literal::string::single,
         .action = state_action::push_state("sstring"),
         .group_tokens = {}},
        {.pattern = R"(\.\.\.|\.\.|[+\-*/%^#<>=~]=?)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\[\](){}.,;:])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([a-zA-Z_]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
    };
    rules["mlcomment"] = {
        {.pattern = R"(\]\])",
         .token = tk::comment::multiline,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^\]]+)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\])", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
    };
    rules["mlstring"] = {
        {.pattern = R"(\]\])",
         .token = tk::literal::string::self,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^\]]+)",
         .token = tk::literal::string::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\])", .token = tk::literal::string::self, .action = state_action::none(), .group_tokens = {}},
    };
    rules["dstring"] = {
        {.pattern = R"(\\[\\'"abfnrtv\n0])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\x[0-9a-fA-F]{2})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\u\{[0-9a-fA-F]+\})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\[0-9]{1,3})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^"\\]+)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\.)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    rules["sstring"] = {
        {.pattern = R"(\\[\\'"abfnrtv\n0])",
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
        {.pattern = R"(\\.)", .token = tk::literal::string::single, .action = state_action::none(), .group_tokens = {}},
    };
    return rules;
}

SPEARMINT_API void register_lua_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<lua_lexer>(); }, lua_info);
}

} // namespace spearmint::lexers

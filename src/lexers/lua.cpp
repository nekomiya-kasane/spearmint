#include "spearmint/lexers/lua.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"lua"};
constexpr const char *filenames[] = {"*.lua"};
constexpr const char *mimes[] = {"text/x-lua", "application/x-lua"};
const lexer_info lua_info = {
    "lua", "Lua", {aliases}, {filenames}, {mimes}, "https://www.lua.org", 10,
};
} // namespace

const lexer_info &lua_lexer::info() const noexcept {
    return lua_info;
}

float lua_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("local ") != src.npos) score += 0.2f;
    if (src.find("function ") != src.npos) score += 0.1f;
    if (src.find("end\n") != src.npos) score += 0.1f;
    if (src.find("then\n") != src.npos || src.find("then ") != src.npos) score += 0.2f;
    if (src.find("require(") != src.npos || src.find("require \"") != src.npos) score += 0.2f;
    return score > 1.0f ? 1.0f : score;
}

state_map lua_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(--\[\[)", tk::comment::multiline, state_action::push_state("mlcomment")},
        {R"(--[^\n]*)", tk::comment::single, state_action::none()},
        {R"(\b(and|break|do|else|elseif|end|false|for|function|goto|if|in|local|nil|not|or|repeat|return|then|true|until|while)\b)",
         tk::keyword::self, state_action::none()},
        {R"(\b(assert|collectgarbage|dofile|error|getmetatable|ipairs|load|loadfile|next|pairs|pcall|print|rawequal|rawget|rawlen|rawset|require|select|setmetatable|tonumber|tostring|type|xpcall|coroutine|debug|io|math|os|package|string|table|utf8)\b)",
         tk::name::builtin, state_action::none()},
        {R"(0x[0-9a-fA-F]+(\.[0-9a-fA-F]+)?([pP][+-]?[0-9]+)?)", tk::literal::number::hex, state_action::none()},
        {R"([0-9]+\.[0-9]*([eE][+-]?[0-9]+)?)", tk::literal::number::float_, state_action::none()},
        {R"(\.[0-9]+([eE][+-]?[0-9]+)?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9]+([eE][+-]?[0-9]+))", tk::literal::number::float_, state_action::none()},
        {R"([0-9]+)", tk::literal::number::integer, state_action::none()},
        {R"(\[\[)", tk::literal::string::self, state_action::push_state("mlstring")},
        {R"(")", tk::literal::string::double_, state_action::push_state("dstring")},
        {R"(')", tk::literal::string::single, state_action::push_state("sstring")},
        {R"(\.\.\.|\.\.|[+\-*/%^#<>=~]=?)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;:])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_]\w*)", tk::name::self, state_action::none()},
    };
    rules["mlcomment"] = {
        {R"(\]\])", tk::comment::multiline, state_action::pop_state()},
        {R"([^\]]+)", tk::comment::multiline, state_action::none()},
        {R"(\])", tk::comment::multiline, state_action::none()},
    };
    rules["mlstring"] = {
        {R"(\]\])", tk::literal::string::self, state_action::pop_state()},
        {R"([^\]]+)", tk::literal::string::self, state_action::none()},
        {R"(\])", tk::literal::string::self, state_action::none()},
    };
    rules["dstring"] = {
        {R"(\\[\\'"abfnrtv\n0])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(\\u\{[0-9a-fA-F]+\})", tk::literal::string::escape, state_action::none()},
        {R"(\\[0-9]{1,3})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };
    rules["sstring"] = {
        {R"(\\[\\'"abfnrtv\n0])", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^'\\]+)", tk::literal::string::single, state_action::none()},
        {R"(\\.)", tk::literal::string::single, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_lua_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<lua_lexer>(); }, lua_info);
}

} // namespace spearmint::lexers

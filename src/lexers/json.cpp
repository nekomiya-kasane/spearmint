/**
 * @file json.cpp
 * @brief JSON lexer implementation.
 */

#include "spearmint/lexers/json.h"

namespace spearmint::lexers {

namespace {

constexpr const char *aliases[] = {"json"};
constexpr const char *filenames[] = {"*.json", "*.jsonl", "*.geojson"};
constexpr const char *mimes[] = {"application/json", "application/ld+json"};

const lexer_info json_info = {
    "json", "JSON", {aliases}, {filenames}, {mimes}, "https://json.org", 10,
};

} // namespace

const lexer_info &json_lexer::info() const noexcept {
    return json_info;
}

state_map json_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;

    rules["root"] = {
        {R"(\n)", tk::whitespace, state_action::none()},
        {R"([ \t\r]+)", tk::whitespace, state_action::none()},

        // Strings (keys and values)
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},

        // Boolean / null
        {R"(\b(?:true|false)\b)", tk::keyword::constant, state_action::none()},
        {R"(\bnull\b)", tk::keyword::constant, state_action::none()},

        // Numbers
        {R"(-?\d+\.\d+(?:[eE][+-]?\d+)?)", tk::literal::number::float_, state_action::none()},
        {R"(-?\d+[eE][+-]?\d+)", tk::literal::number::float_, state_action::none()},
        {R"(-?\d+)", tk::literal::number::integer, state_action::none()},

        // Punctuation
        {R"([\[\]{}:,])", tk::punctuation::self, state_action::none()},
    };

    rules["string"] = {
        {R"(\\[\\"/bfnrt])", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
    };

    return rules;
}

SPEARMINT_API void register_json_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<json_lexer>(); }, json_info);
}

} // namespace spearmint::lexers

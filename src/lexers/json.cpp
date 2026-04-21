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
        {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([ \t\r]+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},

        // Strings (keys and values)
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("string"),
         .group_tokens = {}},

        // Boolean / null
        {.pattern = R"(\b(?:true|false)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\bnull\b)", .token = tk::keyword::constant, .action = state_action::none(), .group_tokens = {}},

        // Numbers
        {.pattern = R"(-?\d+\.\d+(?:[eE][+-]?\d+)?)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(-?\d+[eE][+-]?\d+)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(-?\d+)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},

        // Punctuation
        {.pattern = R"([\[\]{}:,])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
    };

    rules["string"] = {
        {.pattern = R"(\\[\\"/bfnrt])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\u[0-9a-fA-F]{4})",
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
    };

    return rules;
}

SPEARMINT_API void register_json_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<json_lexer>(); }, json_info);
}

} // namespace spearmint::lexers

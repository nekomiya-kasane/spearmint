#include "spearmint/lexers/css.h"

namespace spearmint::lexers {

    namespace {
        constexpr const char *aliases[] = {"css"};
        constexpr const char *filenames[] = {"*.css"};
        constexpr const char *mimes[] = {"text/css"};
        const lexer_info css_info = {
            .name = "css",
            .display_name = "CSS",
            .aliases = {aliases},
            .filenames = {filenames},
            .mime_types = {mimes},
            .url = "https://www.w3.org/Style/CSS/",
            .priority = 10,
        };
    } // namespace

    const lexer_info &css_lexer::info() const noexcept {
        return css_info;
    }

    float css_lexer::analyse_text(std::string_view src) const noexcept {
        float score = 0.0f;
        if (src.find("{") != src.npos && src.find("}") != src.npos) {
            score += 0.1f;
        }
        if (src.find("color:") != src.npos || src.find("margin:") != src.npos) {
            score += 0.2f;
        }
        if (src.find("@media") != src.npos) {
            score += 0.2f;
        }
        if (src.find("display:") != src.npos) {
            score += 0.1f;
        }
        return score > 1.0f ? 1.0f : score;
    }

    state_map css_lexer::get_rules() const {
        namespace tk = token;
        state_map rules;
        rules["root"] = {
            {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(/\*)",
             .token = tk::comment::multiline,
             .action = state_action::push_state("comment"),
             .group_tokens = {}},
            {.pattern = R"(@[a-zA-Z-]+)",
             .token = tk::keyword::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\.[a-zA-Z_][\w-]*)",
             .token = tk::name::class_,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(#[a-zA-Z_][\w-]*)",
             .token = tk::name::label,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(::[a-zA-Z-]+)",
             .token = tk::name::decorator,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(:[a-zA-Z-]+)",
             .token = tk::name::decorator,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern =
                 R"(\b(inherit|initial|unset|revert|none|auto|normal|bold|italic|underline|solid|dashed|dotted|block|inline|flex|grid|absolute|relative|fixed|sticky|hidden|visible|transparent|currentColor|important)\b)",
             .token = tk::keyword::constant,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(!important)",
             .token = tk::keyword::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern =
                 R"([0-9]+(\.[0-9]+)?(px|em|rem|%|vh|vw|vmin|vmax|ch|ex|cm|mm|in|pt|pc|deg|rad|grad|turn|s|ms|Hz|kHz|dpi|dpcm|dppx|fr)?)",
             .token = tk::literal::number::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(#[0-9a-fA-F]{3,8})",
             .token = tk::literal::number::hex,
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
            {.pattern = R"(url\()", .token = tk::name::builtin, .action = state_action::none(), .group_tokens = {}},
            {.pattern = R"(\b(rgb|rgba|hsl|hsla|calc|var|min|max|clamp|env|attr)\b)",
             .token = tk::name::builtin,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([a-zA-Z-]+(?=\s*:))",
             .token = tk::name::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([{}();:,>+~*])",
             .token = tk::punctuation::self,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"([a-zA-Z_][\w-]*)",
             .token = tk::name::self,
             .action = state_action::none(),
             .group_tokens = {}},
        };
        rules["comment"] = {
            {.pattern = R"(\*/)",
             .token = tk::comment::multiline,
             .action = state_action::pop_state(),
             .group_tokens = {}},
            {.pattern = R"([^*]+)",
             .token = tk::comment::multiline,
             .action = state_action::none(),
             .group_tokens = {}},
            {.pattern = R"(\*)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        };
        rules["dstring"] = {
            {.pattern = R"(\\.)",
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
        rules["sstring"] = {
            {.pattern = R"(\\.)",
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
        };
        return rules;
    }

    SPEARMINT_API void register_css_lexer() {
        register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<css_lexer>(); }, css_info);
    }

} // namespace spearmint::lexers

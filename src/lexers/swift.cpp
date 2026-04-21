#include "spearmint/lexers/swift.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"swift"};
constexpr const char *filenames[] = {"*.swift"};
constexpr const char *mimes[] = {"text/x-swift"};
const lexer_info swift_info = {
    .name = "swift",
    .display_name = "Swift",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://swift.org",
    .priority = 10,
};
} // namespace

const lexer_info &swift_lexer::info() const noexcept {
    return swift_info;
}

float swift_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("import Foundation") != src.npos || src.find("import UIKit") != src.npos ||
        src.find("import SwiftUI") != src.npos) {
        score += 0.5f;
    }
    if (src.find("func ") != src.npos) {
        score += 0.1f;
    }
    if (src.find("let ") != src.npos || src.find("var ") != src.npos) {
        score += 0.05f;
    }
    if (src.find("guard ") != src.npos) {
        score += 0.3f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map swift_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {.pattern = R"(\s+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(//[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern = R"(@[a-zA-Z_]\w*)",
         .token = tk::name::decorator,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(#[a-zA-Z_]\w*)",
         .token = tk::comment::preproc,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(actor|associatedtype|async|await|break|case|catch|class|continue|convenience|default|defer|deinit|didSet|do|else|enum|extension|fallthrough|fileprivate|final|for|func|get|guard|if|import|in|indirect|infix|init|inout|internal|is|lazy|let|mutating|nonmutating|open|operator|optional|override|postfix|precedencegroup|prefix|private|protocol|public|repeat|required|rethrows|return|self|Self|set|some|static|struct|subscript|super|switch|throw|throws|try|typealias|unowned|var|weak|where|while|willSet)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b(true|false|nil)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(Any|AnyObject|Bool|Character|Double|Float|Int|Int8|Int16|Int32|Int64|Optional|String|UInt|UInt8|UInt16|UInt32|UInt64|Void|Array|Dictionary|Set|Result|Never)\b)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern =
             R"(\b(print|debugPrint|dump|fatalError|precondition|preconditionFailure|assert|assertionFailure|type|min|max|abs|zip|stride|sequence|repeatElement|swap)\b)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0x[0-9a-fA-F_]+)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0o[0-7_]+)",
         .token = tk::literal::number::oct,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(0b[01_]+)",
         .token = tk::literal::number::bin,
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
        {.pattern = R"(""")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("mlstring"),
         .group_tokens = {}},
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("string"),
         .group_tokens = {}},
        {.pattern = R"(->|\.\.\.|\.\.<|&&|\|\||[+\-*/%&|^~!<>=]=?|\?\?|\?\.?|::)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([\[\](){}.,;:])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"([a-zA-Z_]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
    };
    rules["comment"] = {
        {.pattern = R"(\*/)", .token = tk::comment::multiline, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},
        {.pattern = R"([^/*]+)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([/*])", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
    };
    rules["string"] = {
        {.pattern = R"(\\[\\'"0nrt])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\u\{[0-9a-fA-F]+\})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\\()",
         .token = tk::literal::string::interpol,
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
    rules["mlstring"] = {
        {.pattern = R"(\\[\\'"0nrt])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\\()",
         .token = tk::literal::string::interpol,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(""")",
         .token = tk::literal::string::double_,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^"\\]+)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(")", .token = tk::literal::string::double_, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\\.)",
         .token = tk::literal::string::double_,
         .action = state_action::none(),
         .group_tokens = {}},
    };
    return rules;
}

SPEARMINT_API void register_swift_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<swift_lexer>(); }, swift_info);
}

} // namespace spearmint::lexers

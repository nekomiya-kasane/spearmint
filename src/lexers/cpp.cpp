/**
 * @file cpp.cpp
 * @brief C/C++ lexer implementation.
 */

#include "spearmint/lexers/cpp.h"

namespace spearmint::lexers {

namespace {

constexpr const char *aliases[] = {"cpp", "c++", "c", "cxx"};
constexpr const char *filenames[] = {"*.cpp", "*.cxx", "*.cc", "*.c", "*.h", "*.hpp", "*.hxx", "*.hh", "*.ipp"};
constexpr const char *mimes[] = {"text/x-c++src", "text/x-csrc", "text/x-c++hdr", "text/x-chdr"};

const lexer_info cpp_info = {
    "cpp", "C++", {aliases}, {filenames}, {mimes}, "https://isocpp.org", 10,
};

} // namespace

const lexer_info &cpp_lexer::info() const noexcept {
    return cpp_info;
}

float cpp_lexer::analyse_text(std::string_view source) const noexcept {
    float score = 0.0f;
    if (source.find("#include") != std::string_view::npos) {
        score += 0.2f;
    }
    if (source.find("int main") != std::string_view::npos) {
        score += 0.2f;
    }
    if (source.find("std::") != std::string_view::npos) {
        score += 0.15f;
    }
    if (source.find("namespace") != std::string_view::npos) {
        score += 0.1f;
    }
    if (source.find("template") != std::string_view::npos) {
        score += 0.1f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map cpp_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;

    rules["root"] = {
        {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"([ \t\r]+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},

        // Preprocessor
        {.pattern = R"(#[^\n]*)", .token = tk::comment::preproc, .action = state_action::none(), .group_tokens = {}},

        // Single-line comments
        {.pattern = R"(//[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},

        // Multi-line comments
        {.pattern = R"(/\*)",
         .token = tk::comment::multiline,
         .action = state_action::push_state("comment"),
         .group_tokens = {}},

        // Raw strings R"(...)"
        {.pattern = R"(R"[^(]*\()",
         .token = tk::literal::string::self,
         .action = state_action::push_state("rawstring"),
         .group_tokens = {}},

        // Strings
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("string"),
         .group_tokens = {}},

        // Char literals
        {.pattern = R"(')",
         .token = tk::literal::string::char_,
         .action = state_action::push_state("char"),
         .group_tokens = {}},

        // Keywords
        {.pattern =
             R"(\b(?:alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|co_await|co_return|co_yield|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|float|for|friend|goto|if|inline|int|long|module|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|template|this|thread_local|throw|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},

        // Builtin types
        {.pattern =
             R"(\b(?:size_t|ptrdiff_t|intptr_t|uintptr_t|int8_t|int16_t|int32_t|int64_t|uint8_t|uint16_t|uint32_t|uint64_t|nullptr_t|string|string_view|vector|map|set|array|span|optional|variant|tuple|pair|unique_ptr|shared_ptr|weak_ptr)\b)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},

        // Boolean/null
        {.pattern = R"(\b(?:true|false|nullptr|NULL)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},

        // Floating point
        {.pattern = R"(\b\d+\.\d*(?:[eE][+-]?\d+)?[fFlL]?\b)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b\d+[eE][+-]?\d+[fFlL]?\b)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b\.\d+(?:[eE][+-]?\d+)?[fFlL]?\b)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},

        // Hex
        {.pattern = R"(\b0[xX][0-9a-fA-F']+[uUlL]*\b)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},

        // Oct
        {.pattern = R"(\b0[0-7']+[uUlL]*\b)",
         .token = tk::literal::number::oct,
         .action = state_action::none(),
         .group_tokens = {}},

        // Bin
        {.pattern = R"(\b0[bB][01']+[uUlL]*\b)",
         .token = tk::literal::number::bin,
         .action = state_action::none(),
         .group_tokens = {}},

        // Integer
        {.pattern = R"(\b[0-9][0-9']*[uUlL]*\b)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},

        // Operators
        {.pattern = R"([+\-*/%&|^~<>=!?:;]+)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},

        // Punctuation
        {.pattern = R"([\[\](){}.,])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},

        // Identifiers
        {.pattern = R"([a-zA-Z_]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
    };

    rules["comment"] = {
        {.pattern = R"(\*/)", .token = tk::comment::multiline, .action = state_action::pop_state(), .group_tokens = {}},
        {.pattern = R"([^*]+)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\*)", .token = tk::comment::multiline, .action = state_action::none(), .group_tokens = {}},
    };

    rules["string"] = {
        {.pattern = R"(\\[\\'"abfnrtv0])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\x[0-9a-fA-F]+)",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\u[0-9a-fA-F]{4})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\U[0-9a-fA-F]{8})",
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

    rules["char"] = {
        {.pattern = R"(\\[\\'"abfnrtv0])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\x[0-9a-fA-F]+)",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(')",
         .token = tk::literal::string::char_,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^'\\]+)",
         .token = tk::literal::string::char_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\.)", .token = tk::literal::string::char_, .action = state_action::none(), .group_tokens = {}},
    };

    rules["rawstring"] = {
        {.pattern = R"(\)[^"]*")",
         .token = tk::literal::string::self,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^)]+)", .token = tk::literal::string::self, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\))", .token = tk::literal::string::self, .action = state_action::none(), .group_tokens = {}},
    };

    return rules;
}

SPEARMINT_API void register_cpp_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<cpp_lexer>(); }, cpp_info);
}

} // namespace spearmint::lexers

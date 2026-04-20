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
    if (source.find("#include") != std::string_view::npos) score += 0.2f;
    if (source.find("int main") != std::string_view::npos) score += 0.2f;
    if (source.find("std::") != std::string_view::npos) score += 0.15f;
    if (source.find("namespace") != std::string_view::npos) score += 0.1f;
    if (source.find("template") != std::string_view::npos) score += 0.1f;
    return score > 1.0f ? 1.0f : score;
}

state_map cpp_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;

    rules["root"] = {
        {R"(\n)", tk::whitespace, state_action::none()},
        {R"([ \t\r]+)", tk::whitespace, state_action::none()},

        // Preprocessor
        {R"(#[^\n]*)", tk::comment::preproc, state_action::none()},

        // Single-line comments
        {R"(//[^\n]*)", tk::comment::single, state_action::none()},

        // Multi-line comments
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},

        // Raw strings R"(...)"
        {R"(R"[^(]*\()", tk::literal::string::self, state_action::push_state("rawstring")},

        // Strings
        {R"(")", tk::literal::string::double_, state_action::push_state("string")},

        // Char literals
        {R"(')", tk::literal::string::char_, state_action::push_state("char")},

        // Keywords
        {R"(\b(?:alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|co_await|co_return|co_yield|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|float|for|friend|goto|if|inline|int|long|module|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|template|this|thread_local|throw|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq)\b)",
         tk::keyword::self, state_action::none()},

        // Builtin types
        {R"(\b(?:size_t|ptrdiff_t|intptr_t|uintptr_t|int8_t|int16_t|int32_t|int64_t|uint8_t|uint16_t|uint32_t|uint64_t|nullptr_t|string|string_view|vector|map|set|array|span|optional|variant|tuple|pair|unique_ptr|shared_ptr|weak_ptr)\b)",
         tk::keyword::type, state_action::none()},

        // Boolean/null
        {R"(\b(?:true|false|nullptr|NULL)\b)", tk::keyword::constant, state_action::none()},

        // Floating point
        {R"(\b\d+\.\d*(?:[eE][+-]?\d+)?[fFlL]?\b)", tk::literal::number::float_, state_action::none()},
        {R"(\b\d+[eE][+-]?\d+[fFlL]?\b)", tk::literal::number::float_, state_action::none()},
        {R"(\b\.\d+(?:[eE][+-]?\d+)?[fFlL]?\b)", tk::literal::number::float_, state_action::none()},

        // Hex
        {R"(\b0[xX][0-9a-fA-F']+[uUlL]*\b)", tk::literal::number::hex, state_action::none()},

        // Oct
        {R"(\b0[0-7']+[uUlL]*\b)", tk::literal::number::oct, state_action::none()},

        // Bin
        {R"(\b0[bB][01']+[uUlL]*\b)", tk::literal::number::bin, state_action::none()},

        // Integer
        {R"(\b[0-9][0-9']*[uUlL]*\b)", tk::literal::number::integer, state_action::none()},

        // Operators
        {R"([+\-*/%&|^~<>=!?:;]+)", tk::operator_::self, state_action::none()},

        // Punctuation
        {R"([\[\](){}.,])", tk::punctuation::self, state_action::none()},

        // Identifiers
        {R"([a-zA-Z_]\w*)", tk::name::self, state_action::none()},
    };

    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };

    rules["string"] = {
        {R"(\\[\\'"abfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]+)", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\U[0-9a-fA-F]{8})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };

    rules["char"] = {
        {R"(\\[\\'"abfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]+)", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::char_, state_action::pop_state()},
        {R"([^'\\]+)", tk::literal::string::char_, state_action::none()},
        {R"(\\.)", tk::literal::string::char_, state_action::none()},
    };

    rules["rawstring"] = {
        {R"(\)[^"]*")", tk::literal::string::self, state_action::pop_state()},
        {R"([^)]+)", tk::literal::string::self, state_action::none()},
        {R"(\))", tk::literal::string::self, state_action::none()},
    };

    return rules;
}

SPEARMINT_API void register_cpp_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<cpp_lexer>(); }, cpp_info);
}

} // namespace spearmint::lexers

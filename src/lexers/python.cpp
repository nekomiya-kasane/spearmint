/**
 * @file python.cpp
 * @brief Python 3 lexer implementation.
 */

#include "spearmint/lexers/python.h"

namespace spearmint::lexers {

namespace {

constexpr const char *aliases[] = {"python", "py", "python3", "py3"};
constexpr const char *filenames[] = {"*.py", "*.pyw", "*.pyi", "*.jy", "*.sage"};
constexpr const char *mimes[] = {"text/x-python", "application/x-python"};

const lexer_info python_info = {
    "python", "Python", {aliases}, {filenames}, {mimes}, "https://python.org", 10,
};

} // namespace

const lexer_info &python_lexer::info() const noexcept {
    return python_info;
}

float python_lexer::analyse_text(std::string_view source) const noexcept {
    float score = 0.0f;
    if (source.find("import ") != std::string_view::npos) score += 0.1f;
    if (source.find("def ") != std::string_view::npos) score += 0.1f;
    if (source.find("class ") != std::string_view::npos) score += 0.1f;
    if (source.starts_with("#!/usr/bin/env python") || source.starts_with("#!/usr/bin/python")) score += 0.5f;
    if (source.find("print(") != std::string_view::npos) score += 0.05f;
    return score > 1.0f ? 1.0f : score;
}

state_map python_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;

    // ── root state ─────────────────────────────────────────────────────
    rules["root"] = {
        // Newlines
        {R"(\n)", tk::whitespace, state_action::none()},

        // Whitespace
        {R"([ \t\r]+)", tk::whitespace, state_action::none()},

        // Comments
        {R"(#[^\n]*)", tk::comment::single, state_action::none()},

        // Triple-quoted strings (must come before single-quoted)
        {R"(""")", tk::literal::string::doc, state_action::push_state("tdqs")},
        {R"(''')", tk::literal::string::doc, state_action::push_state("tsqs")},

        // Decorators
        {R"(@[a-zA-Z_]\w*(?:\.[a-zA-Z_]\w*)*)", tk::name::decorator, state_action::none()},

        // Keywords
        {R"(\b(?:False|None|True|and|as|assert|async|await|break|class|continue|def|del|elif|else|except|finally|for|from|global|if|import|in|is|lambda|nonlocal|not|or|pass|raise|return|try|while|with|yield)\b)",
         tk::keyword::self, state_action::none()},

        // Builtin types
        {R"(\b(?:bool|bytearray|bytes|complex|dict|float|frozenset|int|list|memoryview|object|range|set|str|tuple|type)\b)",
         tk::keyword::type, state_action::none()},

        // Builtin functions
        {R"(\b(?:abs|all|any|ascii|bin|breakpoint|callable|chr|classmethod|compile|delattr|dir|divmod|enumerate|eval|exec|filter|format|getattr|globals|hasattr|hash|help|hex|id|input|isinstance|issubclass|iter|len|locals|map|max|min|next|oct|open|ord|pow|print|property|repr|reversed|round|setattr|slice|sorted|staticmethod|sum|super|vars|zip|__import__)\b)",
         tk::name::builtin, state_action::none()},

        // Builtin constants
        {R"(\b(?:NotImplemented|Ellipsis|__debug__)\b)", tk::keyword::constant, state_action::none()},

        // Builtin exceptions
        {R"(\b(?:ArithmeticError|AssertionError|AttributeError|BaseException|BlockingIOError|BrokenPipeError|BufferError|BytesWarning|ChildProcessError|ConnectionAbortedError|ConnectionError|ConnectionRefusedError|ConnectionResetError|DeprecationWarning|EOFError|EnvironmentError|Exception|FileExistsError|FileNotFoundError|FloatingPointError|FutureWarning|GeneratorExit|IOError|ImportError|ImportWarning|IndentationError|IndexError|InterruptedError|IsADirectoryError|KeyError|KeyboardInterrupt|LookupError|MemoryError|ModuleNotFoundError|NameError|NotADirectoryError|NotImplementedError|OSError|OverflowError|PendingDeprecationWarning|PermissionError|ProcessLookupError|RecursionError|ReferenceError|ResourceWarning|RuntimeError|RuntimeWarning|StopAsyncIteration|StopIteration|SyntaxError|SyntaxWarning|SystemError|SystemExit|TabError|TimeoutError|TypeError|UnboundLocalError|UnicodeDecodeError|UnicodeEncodeError|UnicodeError|UnicodeTranslationError|UnicodeWarning|UserWarning|ValueError|Warning|WindowsError|ZeroDivisionError)\b)",
         tk::name::exception, state_action::none()},

        // Magic names (__xxx__)
        {R"(__[a-zA-Z_]\w*__)", tk::name::function_magic, state_action::none()},

        // Double-quoted strings
        {R"(")", tk::literal::string::double_, state_action::push_state("dqs")},

        // Single-quoted strings
        {R"(')", tk::literal::string::single, state_action::push_state("sqs")},

        // Floating point numbers
        {R"(\b\d+\.\d*(?:[eE][+-]?\d+)?j?\b)", tk::literal::number::float_, state_action::none()},
        {R"(\b\d+[eE][+-]?\d+j?\b)", tk::literal::number::float_, state_action::none()},
        {R"(\b\.\d+(?:[eE][+-]?\d+)?j?\b)", tk::literal::number::float_, state_action::none()},

        // Hex numbers
        {R"(\b0[xX][0-9a-fA-F_]+\b)", tk::literal::number::hex, state_action::none()},

        // Oct numbers
        {R"(\b0[oO][0-7_]+\b)", tk::literal::number::oct, state_action::none()},

        // Bin numbers
        {R"(\b0[bB][01_]+\b)", tk::literal::number::bin, state_action::none()},

        // Integer numbers
        {R"(\b[0-9][0-9_]*\b)", tk::literal::number::integer, state_action::none()},

        // Operators
        {R"([+\-*/%&|^~<>=!@:;]+)", tk::operator_::self, state_action::none()},

        // Punctuation
        {R"([\[\](){}.,])", tk::punctuation::self, state_action::none()},

        // Identifiers (names)
        {R"([a-zA-Z_]\w*)", tk::name::self, state_action::none()},
    };

    // ── Double-quoted string state ─────────────────────────────────────
    rules["dqs"] = {
        {R"(\\[\\'"abfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\U[0-9a-fA-F]{8})", tk::literal::string::escape, state_action::none()},
        {R"(\\N\{[^}]+\})", tk::literal::string::escape, state_action::none()},
        {R"(")", tk::literal::string::double_, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::double_, state_action::none()},
        {R"(\\.)", tk::literal::string::double_, state_action::none()},
    };

    // ── Single-quoted string state ─────────────────────────────────────
    rules["sqs"] = {
        {R"(\\[\\'"abfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(\\x[0-9a-fA-F]{2})", tk::literal::string::escape, state_action::none()},
        {R"(\\u[0-9a-fA-F]{4})", tk::literal::string::escape, state_action::none()},
        {R"(\\U[0-9a-fA-F]{8})", tk::literal::string::escape, state_action::none()},
        {R"(\\N\{[^}]+\})", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^'\\]+)", tk::literal::string::single, state_action::none()},
        {R"(\\.)", tk::literal::string::single, state_action::none()},
    };

    // ── Triple double-quoted string state ──────────────────────────────
    rules["tdqs"] = {
        {R"(\\[\\'"abfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(""")", tk::literal::string::doc, state_action::pop_state()},
        {R"([^"\\]+)", tk::literal::string::doc, state_action::none()},
        {R"(")", tk::literal::string::doc, state_action::none()},
        {R"(\\.)", tk::literal::string::doc, state_action::none()},
    };

    // ── Triple single-quoted string state ──────────────────────────────
    rules["tsqs"] = {
        {R"(\\[\\'"abfnrtv0])", tk::literal::string::escape, state_action::none()},
        {R"(''')", tk::literal::string::doc, state_action::pop_state()},
        {R"([^'\\]+)", tk::literal::string::doc, state_action::none()},
        {R"(')", tk::literal::string::doc, state_action::none()},
        {R"(\\.)", tk::literal::string::doc, state_action::none()},
    };

    return rules;
}

SPEARMINT_API void register_python_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<python_lexer>(); }, python_info);
}

} // namespace spearmint::lexers

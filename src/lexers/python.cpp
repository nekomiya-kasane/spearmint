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
    .name = "python",
    .display_name = "Python",
    .aliases = {aliases},
    .filenames = {filenames},
    .mime_types = {mimes},
    .url = "https://python.org",
    .priority = 10,
};

} // namespace

const lexer_info &python_lexer::info() const noexcept {
    return python_info;
}

float python_lexer::analyse_text(std::string_view source) const noexcept {
    float score = 0.0f;
    if (source.find("import ") != std::string_view::npos) {
        score += 0.1f;
    }
    if (source.find("def ") != std::string_view::npos) {
        score += 0.1f;
    }
    if (source.find("class ") != std::string_view::npos) {
        score += 0.1f;
    }
    if (source.starts_with("#!/usr/bin/env python") || source.starts_with("#!/usr/bin/python")) {
        score += 0.5f;
    }
    if (source.find("print(") != std::string_view::npos) {
        score += 0.05f;
    }
    return score > 1.0f ? 1.0f : score;
}

state_map python_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;

    // ── root state ─────────────────────────────────────────────────────
    rules["root"] = {
        // Newlines
        {.pattern = R"(\n)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},

        // Whitespace
        {.pattern = R"([ \t\r]+)", .token = tk::whitespace, .action = state_action::none(), .group_tokens = {}},

        // Comments
        {.pattern = R"(#[^\n]*)", .token = tk::comment::single, .action = state_action::none(), .group_tokens = {}},

        // Triple-quoted strings (must come before single-quoted)
        {.pattern = R"(""")",
         .token = tk::literal::string::doc,
         .action = state_action::push_state("tdqs"),
         .group_tokens = {}},
        {.pattern = R"(''')",
         .token = tk::literal::string::doc,
         .action = state_action::push_state("tsqs"),
         .group_tokens = {}},

        // Decorators
        {.pattern = R"(@[a-zA-Z_]\w*(?:\.[a-zA-Z_]\w*)*)",
         .token = tk::name::decorator,
         .action = state_action::none(),
         .group_tokens = {}},

        // Keywords
        {.pattern =
             R"(\b(?:False|None|True|and|as|assert|async|await|break|class|continue|def|del|elif|else|except|finally|for|from|global|if|import|in|is|lambda|nonlocal|not|or|pass|raise|return|try|while|with|yield)\b)",
         .token = tk::keyword::self,
         .action = state_action::none(),
         .group_tokens = {}},

        // Builtin types
        {.pattern =
             R"(\b(?:bool|bytearray|bytes|complex|dict|float|frozenset|int|list|memoryview|object|range|set|str|tuple|type)\b)",
         .token = tk::keyword::type,
         .action = state_action::none(),
         .group_tokens = {}},

        // Builtin functions
        {.pattern =
             R"(\b(?:abs|all|any|ascii|bin|breakpoint|callable|chr|classmethod|compile|delattr|dir|divmod|enumerate|eval|exec|filter|format|getattr|globals|hasattr|hash|help|hex|id|input|isinstance|issubclass|iter|len|locals|map|max|min|next|oct|open|ord|pow|print|property|repr|reversed|round|setattr|slice|sorted|staticmethod|sum|super|vars|zip|__import__)\b)",
         .token = tk::name::builtin,
         .action = state_action::none(),
         .group_tokens = {}},

        // Builtin constants
        {.pattern = R"(\b(?:NotImplemented|Ellipsis|__debug__)\b)",
         .token = tk::keyword::constant,
         .action = state_action::none(),
         .group_tokens = {}},

        // Builtin exceptions
        {.pattern =
             R"(\b(?:ArithmeticError|AssertionError|AttributeError|BaseException|BlockingIOError|BrokenPipeError|BufferError|BytesWarning|ChildProcessError|ConnectionAbortedError|ConnectionError|ConnectionRefusedError|ConnectionResetError|DeprecationWarning|EOFError|EnvironmentError|Exception|FileExistsError|FileNotFoundError|FloatingPointError|FutureWarning|GeneratorExit|IOError|ImportError|ImportWarning|IndentationError|IndexError|InterruptedError|IsADirectoryError|KeyError|KeyboardInterrupt|LookupError|MemoryError|ModuleNotFoundError|NameError|NotADirectoryError|NotImplementedError|OSError|OverflowError|PendingDeprecationWarning|PermissionError|ProcessLookupError|RecursionError|ReferenceError|ResourceWarning|RuntimeError|RuntimeWarning|StopAsyncIteration|StopIteration|SyntaxError|SyntaxWarning|SystemError|SystemExit|TabError|TimeoutError|TypeError|UnboundLocalError|UnicodeDecodeError|UnicodeEncodeError|UnicodeError|UnicodeTranslationError|UnicodeWarning|UserWarning|ValueError|Warning|WindowsError|ZeroDivisionError)\b)",
         .token = tk::name::exception,
         .action = state_action::none(),
         .group_tokens = {}},

        // Magic names (__xxx__)
        {.pattern = R"(__[a-zA-Z_]\w*__)",
         .token = tk::name::function_magic,
         .action = state_action::none(),
         .group_tokens = {}},

        // Double-quoted strings
        {.pattern = R"(")",
         .token = tk::literal::string::double_,
         .action = state_action::push_state("dqs"),
         .group_tokens = {}},

        // Single-quoted strings
        {.pattern = R"(')",
         .token = tk::literal::string::single,
         .action = state_action::push_state("sqs"),
         .group_tokens = {}},

        // Floating point numbers
        {.pattern = R"(\b\d+\.\d*(?:[eE][+-]?\d+)?j?\b)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b\d+[eE][+-]?\d+j?\b)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\b\.\d+(?:[eE][+-]?\d+)?j?\b)",
         .token = tk::literal::number::float_,
         .action = state_action::none(),
         .group_tokens = {}},

        // Hex numbers
        {.pattern = R"(\b0[xX][0-9a-fA-F_]+\b)",
         .token = tk::literal::number::hex,
         .action = state_action::none(),
         .group_tokens = {}},

        // Oct numbers
        {.pattern = R"(\b0[oO][0-7_]+\b)",
         .token = tk::literal::number::oct,
         .action = state_action::none(),
         .group_tokens = {}},

        // Bin numbers
        {.pattern = R"(\b0[bB][01_]+\b)",
         .token = tk::literal::number::bin,
         .action = state_action::none(),
         .group_tokens = {}},

        // Integer numbers
        {.pattern = R"(\b[0-9][0-9_]*\b)",
         .token = tk::literal::number::integer,
         .action = state_action::none(),
         .group_tokens = {}},

        // Operators
        {.pattern = R"([+\-*/%&|^~<>=!@:;]+)",
         .token = tk::operator_::self,
         .action = state_action::none(),
         .group_tokens = {}},

        // Punctuation
        {.pattern = R"([\[\](){}.,])",
         .token = tk::punctuation::self,
         .action = state_action::none(),
         .group_tokens = {}},

        // Identifiers (names)
        {.pattern = R"([a-zA-Z_]\w*)", .token = tk::name::self, .action = state_action::none(), .group_tokens = {}},
    };

    // ── Double-quoted string state ─────────────────────────────────────
    rules["dqs"] = {
        {.pattern = R"(\\[\\'"abfnrtv0])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\x[0-9a-fA-F]{2})",
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
        {.pattern = R"(\\N\{[^}]+\})",
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

    // ── Single-quoted string state ─────────────────────────────────────
    rules["sqs"] = {
        {.pattern = R"(\\[\\'"abfnrtv0])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(\\x[0-9a-fA-F]{2})",
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
        {.pattern = R"(\\N\{[^}]+\})",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(')",
         .token = tk::literal::string::single,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^'\\]+)", .token = tk::literal::string::single, .action = state_action::none()},
        {.pattern = R"(\\.)", .token = tk::literal::string::single, .action = state_action::none()},
    };

    // ── Triple double-quoted string state ──────────────────────────────
    rules["tdqs"] = {
        {.pattern = R"(\\[\\'"abfnrtv0])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(""")",
         .token = tk::literal::string::doc,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^"\\]+)",
         .token = tk::literal::string::doc,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(")", .token = tk::literal::string::doc, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\\.)", .token = tk::literal::string::doc, .action = state_action::none(), .group_tokens = {}},
    };

    // ── Triple single-quoted string state ──────────────────────────────
    rules["tsqs"] = {
        {.pattern = R"(\\[\\'"abfnrtv0])",
         .token = tk::literal::string::escape,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(''')",
         .token = tk::literal::string::doc,
         .action = state_action::pop_state(),
         .group_tokens = {}},
        {.pattern = R"([^'\\]+)",
         .token = tk::literal::string::doc,
         .action = state_action::none(),
         .group_tokens = {}},
        {.pattern = R"(')", .token = tk::literal::string::doc, .action = state_action::none(), .group_tokens = {}},
        {.pattern = R"(\\.)", .token = tk::literal::string::doc, .action = state_action::none(), .group_tokens = {}},
    };

    return rules;
}

SPEARMINT_API void register_python_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<python_lexer>(); }, python_info);
}

} // namespace spearmint::lexers

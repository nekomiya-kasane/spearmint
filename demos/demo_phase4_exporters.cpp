/**
 * @file demo_phase4_exporters.cpp
 * @brief Phase 4 demo: tokenize Python and export to HTML, ANSI, SVG, LaTeX.
 */

#include <cstdio>
#include <string>

#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/core/style.h"
#include "spearmint/core/style_registry.h"
#include "spearmint/core/lexer_registry.h"
#include "spearmint/lexers/python.h"
#include "spearmint/styles/builtin.h"
#include "spearmint/exporters/html.h"
#include "spearmint/exporters/ansi.h"
#include "spearmint/exporters/svg.h"
#include "spearmint/exporters/latex.h"

using namespace spearmint;

static const char* python_source = R"(def fibonacci(n):
    """Return first n Fibonacci numbers."""
    result = [0, 1]
    while len(result) < n:
        result.append(result[-1] + result[-2])
    return result[:n]

# Print first 10
for i, num in enumerate(fibonacci(10)):
    print(f"F({i}) = {num}")
)";

int main() {
    lexers::register_python_lexer();

    auto lex = get_lexer_by_name("python");
    if (!lex) { std::puts("ERROR: no python lexer"); return 1; }

    auto result = lex->tokenize(python_source);
    style_def_view style(styles::monokai);

    std::puts("=== spearmint Phase 4: Exporters demo ===\n");
    std::printf("Tokens: %zu\n\n", result.size());

    // ── ANSI ───────────────────────────────────────────────────────────
    std::puts("--- ANSI (terminal) output ---\n");
    exporters::ansi_exporter ansi_exp;
    auto ansi = ansi_exp.format(result.tokens, style);
    std::fputs(ansi.c_str(), stdout);
    std::puts("\n");

    // ── HTML ───────────────────────────────────────────────────────────
    std::puts("--- HTML output (first 500 chars) ---\n");
    exporters::html_exporter html_exp({.full_page = true});
    auto html = html_exp.format(result.tokens, style);
    std::printf("Total HTML size: %zu bytes\n", html.size());
    if (html.size() > 500)
        std::printf("%.500s...\n\n", html.c_str());
    else
        std::printf("%s\n\n", html.c_str());

    // ── SVG ────────────────────────────────────────────────────────────
    std::puts("--- SVG output (first 500 chars) ---\n");
    exporters::svg_exporter svg_exp;
    auto svg = svg_exp.format(result.tokens, style);
    std::printf("Total SVG size: %zu bytes\n", svg.size());
    if (svg.size() > 500)
        std::printf("%.500s...\n\n", svg.c_str());
    else
        std::printf("%s\n\n", svg.c_str());

    // ── LaTeX ──────────────────────────────────────────────────────────
    std::puts("--- LaTeX output (first 500 chars) ---\n");
    exporters::latex_exporter latex_exp;
    auto tex = latex_exp.format(result.tokens, style);
    std::printf("Total LaTeX size: %zu bytes\n", tex.size());
    if (tex.size() > 500)
        std::printf("%.500s...\n\n", tex.c_str());
    else
        std::printf("%s\n\n", tex.c_str());

    return 0;
}

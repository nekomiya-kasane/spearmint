/**
 * @file demo_phase3_lexer.cpp
 * @brief Phase 3 demo: tokenize Python source and display with style colors.
 */

#include "spearmint/core/lexer_registry.h"
#include "spearmint/core/style.h"
#include "spearmint/core/style_registry.h"
#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/lexers/python.h"
#include "spearmint/styles/builtin.h"

#include <cstdio>

using namespace spearmint;

static const char *python_source = R"(#!/usr/bin/env python3
"""Fibonacci sequence generator."""

import sys

def fibonacci(n: int) -> list[int]:
    """Return the first n Fibonacci numbers."""
    if n <= 0:
        return []
    result = [0, 1]
    while len(result) < n:
        result.append(result[-1] + result[-2])
    return result[:n]

class FibonacciIterator:
    """Lazy Fibonacci iterator."""

    def __init__(self, limit=None):
        self._a, self._b = 0, 1
        self._limit = limit
        self._count = 0

    def __iter__(self):
        return self

    def __next__(self):
        if self._limit is not None and self._count >= self._limit:
            raise StopIteration
        val = self._a
        self._a, self._b = self._b, self._a + self._b
        self._count += 1
        return val

@staticmethod
def main():
    # Print first 10 Fibonacci numbers
    for i, num in enumerate(fibonacci(10)):
        print(f"F({i}) = {num}")

if __name__ == "__main__":
    main()
)";

int main() {
    // Register the Python lexer
    lexers::register_python_lexer();

    std::puts("=== spearmint Phase 3: Python lexer demo ===\n");

    // Get lexer by name
    auto lex = get_lexer_by_name("python");
    if (!lex) {
        std::puts("ERROR: Python lexer not found");
        return 1;
    }

    std::printf("Lexer: %s (%s)\n", lex->info().display_name, lex->info().name);
    std::printf("Analyse score: %.2f\n\n", lex->analyse_text(python_source));

    // Tokenize
    auto ts = lex->tokenize(python_source);
    std::printf("Total tokens: %zu\n\n", ts.size());

    // Display with monokai colors
    const auto *style = get_style("monokai");
    if (!style) {
        std::puts("ERROR: monokai style not found");
        return 1;
    }

    std::puts("=== Tokenized output (with monokai colors) ===\n");

    for (const auto &entry : ts) {
        // Skip pure whitespace for display
        bool all_ws = true;
        for (char c : entry.text) {
            if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
                all_ws = false;
                break;
            }
        }
        if (all_ws) {
            continue;
        }

        const auto *rule = style->lookup(entry.type);
        std::printf("%-35s ", entry.type.name);
        if (rule && rule->has_fg) {
            std::printf("#%06X ", rule->fg);
        } else {
            std::printf("        ");
        }
        if (rule && rule->bold) {
            std::printf("B ");
        } else {
            std::printf("  ");
        }
        if (rule && rule->italic) {
            std::printf("I ");
        } else {
            std::printf("  ");
        }

        // Print text, replacing newlines
        std::printf("| ");
        for (char c : entry.text) {
            if (c == '\n') {
                std::printf("\\n");
            } else if (c == '\t') {
                std::printf("\\t");
            } else {
                std::putchar(c);
            }
        }
        std::putchar('\n');
    }

    std::puts("\n=== Source reconstruction ===\n");

    // Verify lossless tokenization
    std::string reconstructed;
    for (const auto &e : ts) {
        reconstructed.append(e.text);
    }
    std::printf("Original length:      %zu\n", std::string_view(python_source).size());
    std::printf("Reconstructed length: %zu\n", reconstructed.size());
    std::printf("Lossless: %s\n", reconstructed == python_source ? "YES" : "NO");

    return 0;
}

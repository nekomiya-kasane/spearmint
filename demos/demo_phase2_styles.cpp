/**
 * @file demo_phase2_styles.cpp
 * @brief Phase 2 demo: lists all builtin styles and shows token coloring for a sample.
 */

#include "spearmint/core/style.h"
#include "spearmint/core/style_registry.h"
#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/styles/builtin.h"

#include <cstdio>

using namespace spearmint;

static void print_hex(uint32_t color) {
    std::printf("#%06X", color);
}

int main() {
    std::puts("=== spearmint builtin styles ===\n");

    auto names = get_all_styles();
    std::printf("Total styles: %zu\n\n", names.size());

    for (auto name : names) {
        const auto *s = get_style(name);
        if (!s) continue;

        std::printf("%-20.*s  bg=", static_cast<int>(s->display_name.size()), s->display_name.data());
        if (s->has_background)
            print_hex(s->background_color);
        else
            std::printf("(none)");
        std::printf("  entries=%zu\n", s->entries.size());
    }

    std::puts("\n=== monokai token coloring ===\n");

    token_stream ts;
    ts.push_back({token::keyword::self, "def"});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::name::function, "fibonacci"});
    ts.push_back({token::punctuation::self, "("});
    ts.push_back({token::name::self, "n"});
    ts.push_back({token::punctuation::self, ")"});
    ts.push_back({token::punctuation::self, ":"});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::keyword::self, "return"});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::name::self, "n"});

    const auto *monokai = get_style("monokai");
    if (!monokai) {
        std::puts("ERROR: monokai style not found");
        return 1;
    }

    for (const auto &entry : ts) {
        const auto *rule = monokai->lookup(entry.type);
        std::printf("%-35s | \"%.*s\"", entry.type.name, static_cast<int>(entry.text.size()), entry.text.data());
        if (rule) {
            std::printf("  → fg=");
            if (rule->has_fg)
                print_hex(rule->fg);
            else
                std::printf("(inherit)");
            if (rule->bold) std::printf(" bold");
            if (rule->italic) std::printf(" italic");
        } else {
            std::printf("  → (no rule)");
        }
        std::putchar('\n');
    }

    std::puts("\n=== compile-time style verification ===\n");

    static_assert(styles::monokai.lookup(token::keyword::self)->fg == 0xf92672);
    std::printf("monokai keyword fg = #%06X (verified at compile time)\n",
                styles::monokai.lookup(token::keyword::self)->fg);

    return 0;
}

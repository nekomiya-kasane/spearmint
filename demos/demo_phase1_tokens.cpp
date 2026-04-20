/**
 * @file demo_phase1_tokens.cpp
 * @brief Phase 1 demo: prints the full spearmint token hierarchy as a tree.
 */

#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"

#include <cstdio>
#include <cstring>

using namespace spearmint;

static int depth_of(token_type t) {
    int d = 0;
    const char *p = t.name;
    while (*p) {
        if (*p == '.') ++d;
        ++p;
    }
    return d;
}

int main() {
    std::puts("=== spearmint token hierarchy ===\n");

    for (uint32_t i = 0; i < detail::valid_token_count; ++i) {
        const auto &t = detail::all_tokens[i];
        int d = depth_of(t);

        for (int j = 0; j < d; ++j) std::fputs("  ", stdout);

        std::printf("[%3u] %s", t.id, t.short_name);

        if (d > 0) {
            // Show parent
            const auto &parent = detail::token_table[t.parent_id];
            std::printf("  (parent: %s)", parent.short_name);
        }
        std::putchar('\n');
    }

    std::puts("\n=== token_stream demo ===\n");

    token_stream ts;
    ts.push_back({token::keyword::self, "def"});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::name::function, "fibonacci"});
    ts.push_back({token::punctuation::self, "("});
    ts.push_back({token::name::self, "n"});
    ts.push_back({token::punctuation::self, ")"});
    ts.push_back({token::punctuation::self, ":"});
    ts.push_back({token::whitespace, "\n    "});
    ts.push_back({token::keyword::self, "if"});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::name::self, "n"});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::operator_::self, "<="});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::literal::number::integer, "1"});
    ts.push_back({token::punctuation::self, ":"});
    ts.push_back({token::whitespace, "\n        "});
    ts.push_back({token::keyword::self, "return"});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::name::self, "n"});

    for (const auto &entry : ts) {
        std::printf("%-35s | \"%.*s\"\n", entry.type.name, static_cast<int>(entry.text.size()), entry.text.data());
    }

    std::puts("\n=== compile-time verification ===\n");

    static_assert(token::keyword::constant.is_child_of(token::keyword::self));
    static_assert(token_from_string("Token.Keyword.Constant") == token::keyword::constant);

    std::printf("token_from_string(\"Token.Keyword.Constant\") = %s [id=%u]\n",
                token_from_string("Token.Keyword.Constant").name, token_from_string("Token.Keyword.Constant").id);

    std::printf("Total token types: %u\n", detail::valid_token_count);

    return 0;
}

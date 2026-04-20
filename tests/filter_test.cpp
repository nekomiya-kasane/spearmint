#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/filters/builtin_filters.h"
#include "spearmint/filters/filter.h"

#include <gtest/gtest.h>

using namespace spearmint;

static token_stream make_sample() {
    return {
        {token::keyword::self, "def"},   {token::whitespace, " "},        {token::name::function, "foo"},
        {token::punctuation::self, "("}, {token::punctuation::self, ")"}, {token::punctuation::self, ":"},
        {token::whitespace, "\n    "},   {token::keyword::self, "pass"},  {token::whitespace, "\n"},
    };
}

// ── strip_whitespace ───────────────────────────────────────────────────

TEST(FilterTest, StripWhitespace) {
    auto ts = make_sample();
    auto result = ts | filters::strip_whitespace();
    for (const auto &e : result) {
        EXPECT_NE(e.type, token::whitespace);
    }
    EXPECT_EQ(result.size(), 6u); // def, foo, (, ), :, pass
}

// ── remove_token ───────────────────────────────────────────────────────

TEST(FilterTest, RemoveToken) {
    auto ts = make_sample();
    auto result = ts | filters::remove_token(token::punctuation::self);
    for (const auto &e : result) {
        EXPECT_NE(e.type, token::punctuation::self);
    }
}

// ── keep_only ──────────────────────────────────────────────────────────

TEST(FilterTest, KeepOnly) {
    auto ts = make_sample();
    auto result = ts | filters::keep_only({token::keyword::self});
    EXPECT_EQ(result.size(), 2u); // def, pass
    for (const auto &e : result) {
        EXPECT_EQ(e.type, token::keyword::self);
    }
}

// ── remap_token ────────────────────────────────────────────────────────

TEST(FilterTest, RemapToken) {
    auto ts = make_sample();
    auto result = ts | filters::remap_token(token::keyword::self, token::name::self);
    for (const auto &e : result) {
        if (e.text == "def" || e.text == "pass") {
            EXPECT_EQ(e.type, token::name::self);
        }
    }
}

// ── remap_if ───────────────────────────────────────────────────────────

TEST(FilterTest, RemapIf) {
    auto ts = make_sample();
    auto result = ts | filters::remap_if([](const token_entry &e) { return e.text == "foo"; }, token::name::builtin);
    for (const auto &e : result) {
        if (e.text == "foo") {
            EXPECT_EQ(e.type, token::name::builtin);
        }
    }
}

// ── trim ───────────────────────────────────────────────────────────────

TEST(FilterTest, Trim) {
    token_stream ts = {
        {token::whitespace, "\n"}, {token::whitespace, "  "}, {token::keyword::self, "def"},
        {token::whitespace, " "},  {token::name::self, "x"},  {token::whitespace, "\n"},
    };
    auto result = ts | filters::trim();
    ASSERT_GE(result.size(), 3u);
    EXPECT_EQ(result.front().type, token::keyword::self);
    EXPECT_EQ(result.back().type, token::name::self);
}

// ── normalize_whitespace ───────────────────────────────────────────────

TEST(FilterTest, NormalizeWhitespace) {
    token_stream ts = {
        {token::keyword::self, "def"},
        {token::whitespace, "   "},
        {token::whitespace, "\n\t"},
        {token::name::self, "x"},
    };
    auto result = ts | filters::normalize_whitespace();
    // Two consecutive ws tokens should collapse to one " "
    int ws_count = 0;
    for (const auto &e : result) {
        if (e.type == token::whitespace) {
            EXPECT_EQ(e.text, " ");
            ++ws_count;
        }
    }
    EXPECT_EQ(ws_count, 1);
}

// ── merge_consecutive ──────────────────────────────────────────────────

TEST(FilterTest, MergeConsecutiveContiguous) {
    // Build contiguous string_views from a single buffer
    static const char buf[] = "defpass";
    token_stream ts = {
        {token::keyword::self, std::string_view(buf, 3)},     // "def"
        {token::keyword::self, std::string_view(buf + 3, 4)}, // "pass"
    };
    auto result = ts | filters::merge_consecutive();
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].text, "defpass");
    EXPECT_EQ(result[0].type, token::keyword::self);
}

TEST(FilterTest, MergeConsecutiveNonContiguous) {
    token_stream ts = {
        {token::keyword::self, "def"},
        {token::keyword::self, "pass"},
    };
    auto result = ts | filters::merge_consecutive();
    // Non-contiguous views can't merge, should stay separate
    EXPECT_EQ(result.size(), 2u);
}

// ── Pipeline composition ───────────────────────────────────────────────

TEST(FilterTest, PipelineComposition) {
    auto ts = make_sample();
    auto pipeline = filters::strip_whitespace() | filters::remove_token(token::punctuation::self);
    auto result = pipeline(ts);
    // Should have: def, foo, pass
    EXPECT_EQ(result.size(), 3u);
}

TEST(FilterTest, PipelineWithOperator) {
    auto ts = make_sample();
    auto result = ts | filters::strip_whitespace() | filters::remove_token(token::punctuation::self);
    EXPECT_EQ(result.size(), 3u);
}

// ── Empty stream ───────────────────────────────────────────────────────

TEST(FilterTest, EmptyStream) {
    token_stream ts;
    EXPECT_TRUE((ts | filters::strip_whitespace()).empty());
    EXPECT_TRUE((ts | filters::merge_consecutive()).empty());
    EXPECT_TRUE((ts | filters::trim()).empty());
    EXPECT_TRUE((ts | filters::normalize_whitespace()).empty());
}

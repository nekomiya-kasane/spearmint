#include <gtest/gtest.h>

#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"

using namespace spearmint;

// ── static_assert: token identity ──────────────────────────────────────
// Note: file-scope static_assert with constexpr functions is incompatible with
// -fprofile-instr-generate (coverage instrumentation breaks constexpr eval).
// Guard with __LLVM_INSTR_PROFILE_GENERATE or fall back to runtime checks below.
#ifndef COCA_COVERAGE_INSTRUMENTED

static_assert(token::root == token::root);
static_assert(token::root != token::text);
static_assert(token::keyword::self != token::name::self);

// ── static_assert: is_child_of ─────────────────────────────────────────

static_assert(token::root.is_child_of(token::root));
static_assert(token::text.is_child_of(token::root));
static_assert(token::whitespace.is_child_of(token::text));
static_assert(token::whitespace.is_child_of(token::root));
static_assert(!token::root.is_child_of(token::text));
static_assert(!token::text.is_child_of(token::keyword::self));

// Keyword hierarchy
static_assert(token::keyword::self.is_child_of(token::root));
static_assert(token::keyword::constant.is_child_of(token::keyword::self));
static_assert(token::keyword::constant.is_child_of(token::root));
static_assert(!token::keyword::constant.is_child_of(token::name::self));

// Name hierarchy
static_assert(token::name::function.is_child_of(token::name::self));
static_assert(token::name::function.is_child_of(token::root));
static_assert(token::name::variable_class.is_child_of(token::name::variable));
static_assert(token::name::variable_class.is_child_of(token::name::self));
static_assert(token::name::variable_class.is_child_of(token::root));

// Literal hierarchy
static_assert(token::literal::string::self.is_child_of(token::literal::self));
static_assert(token::literal::string::escape.is_child_of(token::literal::string::self));
static_assert(token::literal::string::escape.is_child_of(token::literal::self));
static_assert(token::literal::string::escape.is_child_of(token::root));
static_assert(token::literal::number::hex.is_child_of(token::literal::number::self));
static_assert(token::literal::number::integer_long.is_child_of(token::literal::number::integer));

// Comment hierarchy
static_assert(token::comment::single.is_child_of(token::comment::self));
static_assert(token::comment::single.is_child_of(token::root));

// Generic hierarchy
static_assert(token::generic::deleted.is_child_of(token::generic::self));

// Cross-branch: not related
static_assert(!token::keyword::self.is_child_of(token::name::self));
static_assert(!token::comment::self.is_child_of(token::literal::self));

// ── static_assert: token_from_string ───────────────────────────────────

static_assert(token_from_string("Token") == token::root);
static_assert(token_from_string("Token.Keyword") == token::keyword::self);
static_assert(token_from_string("Token.Keyword.Constant") == token::keyword::constant);
static_assert(token_from_string("Token.Name.Function") == token::name::function);
static_assert(token_from_string("Token.Literal.String") == token::literal::string::self);
static_assert(token_from_string("Token.Literal.String.Escape") == token::literal::string::escape);
static_assert(token_from_string("Token.Literal.Number.Hex") == token::literal::number::hex);
static_assert(token_from_string("Token.Operator") == token::operator_::self);
static_assert(token_from_string("Token.Operator.Word") == token::operator_::word);
static_assert(token_from_string("Token.Punctuation") == token::punctuation::self);
static_assert(token_from_string("Token.Comment.Single") == token::comment::single);
static_assert(token_from_string("Token.Generic.Deleted") == token::generic::deleted);
static_assert(token_from_string("Token.Generic.StrongEmph") == token::generic::strong_emph);

// Unknown string returns root
static_assert(token_from_string("Token.DoesNotExist") == token::root);

#endif // !coverage instrumentation

// ── Runtime tests ──────────────────────────────────────────────────────

TEST(TokenTest, IsChildOf) {
    EXPECT_TRUE(token::keyword::constant.is_child_of(token::keyword::self));
    EXPECT_TRUE(token::keyword::constant.is_child_of(token::root));
    EXPECT_FALSE(token::keyword::constant.is_child_of(token::name::self));
}

TEST(TokenTest, TokenFromString) {
    EXPECT_EQ(token_from_string("Token.Keyword"), token::keyword::self);
    EXPECT_EQ(token_from_string("Token.Name.Variable.Class"), token::name::variable_class);
    EXPECT_EQ(token_from_string("Token.Literal.Number.Integer.Long"), token::literal::number::integer_long);
}

TEST(TokenTest, TokenNames) {
    EXPECT_STREQ(token::keyword::constant.name, "Token.Keyword.Constant");
    EXPECT_STREQ(token::keyword::constant.short_name, "Constant");
    EXPECT_STREQ(token::literal::string::escape.name, "Token.Literal.String.Escape");
}

TEST(TokenTest, AllTokensCount) {
    EXPECT_EQ(detail::valid_token_count, 79u);
    // Verify no duplicates in all_tokens
    for (uint32_t i = 0; i < detail::valid_token_count; ++i) {
        for (uint32_t j = i + 1; j < detail::valid_token_count; ++j) {
            EXPECT_NE(detail::all_tokens[i].id, detail::all_tokens[j].id)
                << "Duplicate id at indices " << i << " and " << j;
        }
    }
}

TEST(TokenTest, AllTokensHaveNames) {
    for (uint32_t i = 0; i < detail::valid_token_count; ++i) {
        EXPECT_NE(detail::all_tokens[i].name[0], '\0')
            << "Token at index " << i << " has empty name";
    }
}

TEST(TokenTest, TokenStreamBasic) {
    token_stream ts;
    ts.push_back({token::keyword::self, "def"});
    ts.push_back({token::whitespace, " "});
    ts.push_back({token::name::function, "foo"});
    EXPECT_EQ(ts.size(), 3u);
    EXPECT_EQ(ts[0].type, token::keyword::self);
    EXPECT_EQ(ts[0].text, "def");
    EXPECT_EQ(ts[2].type, token::name::function);
    EXPECT_EQ(ts[2].text, "foo");
}

TEST(TokenTest, TokenEntryEquality) {
    token_entry a{token::keyword::self, "def"};
    token_entry b{token::keyword::self, "def"};
    token_entry c{token::keyword::self, "class"};
    token_entry d{token::name::self, "def"};
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(a, d);
}

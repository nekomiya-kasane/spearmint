/**
 * @file ftl_lexer_test.cpp
 * @brief Tests for the Fluent Translation List (.ftl) lexer.
 */

#include "spearmint/lexers/ftl.h"

#include <gtest/gtest.h>

using namespace spearmint;
using namespace spearmint::lexers;

class FtlLexerTest : public ::testing::Test {
  protected:
    ftl_lexer lexer;
};

// ── Info ────────────────────────────────────────────────────────────────

TEST_F(FtlLexerTest, info_name) {
    EXPECT_STREQ(lexer.info().name, "ftl");
    EXPECT_STREQ(lexer.info().display_name, "Fluent (FTL)");
}

TEST_F(FtlLexerTest, info_filenames) {
    ASSERT_GE(lexer.info().filenames.size(), 1u);
    EXPECT_STREQ(lexer.info().filenames[0], "*.ftl");
}

// ── analyse_text ────────────────────────────────────────────────────────

TEST_F(FtlLexerTest, analyse_text_positive) {
    auto score = lexer.analyse_text("hello = Hello { $name }!\n-brand = Firefox\n");
    EXPECT_GT(score, 0.0f);
}

TEST_F(FtlLexerTest, analyse_text_negative) {
    auto score = lexer.analyse_text("int main() { return 0; }");
    EXPECT_LT(score, 0.2f);
}

// ── Simple message ──────────────────────────────────────────────────────

TEST_F(FtlLexerTest, simple_message) {
    auto result = lexer.tokenize("hello = Hello, world!\n");
    EXPECT_FALSE(result.tokens.empty());

    // Should have at least: identifier, operator, string content
    bool has_name = false;
    bool has_operator = false;
    bool has_string = false;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::name::variable) {
            has_name = true;
        }
        if (tok.type == token::operator_::self) {
            has_operator = true;
        }
        if (tok.type == token::literal::string::self) {
            has_string = true;
        }
    }
    EXPECT_TRUE(has_name);
    EXPECT_TRUE(has_operator);
    EXPECT_TRUE(has_string);
}

// ── Comments ────────────────────────────────────────────────────────────

TEST_F(FtlLexerTest, comments) {
    auto result = lexer.tokenize("# Simple comment\n## Group comment\n### Resource comment\n");
    EXPECT_FALSE(result.tokens.empty());

    int comment_count = 0;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::comment::single || tok.type == token::comment::special) {
            ++comment_count;
        }
    }
    EXPECT_EQ(comment_count, 3);
}

// ── Term definition ─────────────────────────────────────────────────────

TEST_F(FtlLexerTest, term_definition) {
    auto result = lexer.tokenize("-brand = Firefox\n");
    EXPECT_FALSE(result.tokens.empty());

    bool has_term = false;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::name::function && tok.text.find("-brand") != std::string_view::npos) {
            has_term = true;
        }
    }
    EXPECT_TRUE(has_term);
}

// ── Placeable with variable ─────────────────────────────────────────────

TEST_F(FtlLexerTest, placeable_variable) {
    auto result = lexer.tokenize("hello = Hello { $name }!\n");
    EXPECT_FALSE(result.tokens.empty());

    bool has_var = false;
    bool has_open_brace = false;
    bool has_close_brace = false;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::name::variable && tok.text.find("$name") != std::string_view::npos) {
            has_var = true;
        }
        if (tok.type == token::punctuation::self && tok.text == "{") {
            has_open_brace = true;
        }
        if (tok.type == token::punctuation::self && tok.text == "}") {
            has_close_brace = true;
        }
    }
    EXPECT_TRUE(has_var);
    EXPECT_TRUE(has_open_brace);
    EXPECT_TRUE(has_close_brace);
}

// ── Placeable with term reference ───────────────────────────────────────

TEST_F(FtlLexerTest, placeable_term_ref) {
    auto result = lexer.tokenize("about = About { -brand }\n");
    EXPECT_FALSE(result.tokens.empty());

    bool has_term_ref = false;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::name::function && tok.text.find("-brand") != std::string_view::npos) {
            has_term_ref = true;
        }
    }
    EXPECT_TRUE(has_term_ref);
}

// ── Attribute ───────────────────────────────────────────────────────────

TEST_F(FtlLexerTest, attribute) {
    auto result = lexer.tokenize("login-button =\n    .label = Log in\n    .title = Click to log in\n");
    EXPECT_FALSE(result.tokens.empty());

    int attr_count = 0;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::name::attribute) {
            ++attr_count;
        }
    }
    EXPECT_GE(attr_count, 2);
}

// ── Selector with variant keys ──────────────────────────────────────────

TEST_F(FtlLexerTest, selector_variants) {
    auto result = lexer.tokenize("emails = { $count ->\n"
                                 "    [one] You have one email.\n"
                                 "    *[other] You have { $count } emails.\n"
                                 "}\n");
    EXPECT_FALSE(result.tokens.empty());

    bool has_arrow = false;
    bool has_keyword = false;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::operator_::self && tok.text == "->") {
            has_arrow = true;
        }
        if (tok.type == token::keyword::self) {
            has_keyword = true;
        }
    }
    EXPECT_TRUE(has_arrow);
    EXPECT_TRUE(has_keyword);
}

// ── Function call ───────────────────────────────────────────────────────

TEST_F(FtlLexerTest, function_call) {
    auto result = lexer.tokenize("today = Today is { DATETIME($date) }\n");
    EXPECT_FALSE(result.tokens.empty());

    bool has_func = false;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::name::builtin && tok.text.find("DATETIME") != std::string_view::npos) {
            has_func = true;
        }
    }
    EXPECT_TRUE(has_func);
}

// ── Number literal in placeable ─────────────────────────────────────────

TEST_F(FtlLexerTest, number_literal) {
    auto result = lexer.tokenize("msg = { NUMBER($val, minimumFractionDigits: 2) }\n");
    EXPECT_FALSE(result.tokens.empty());

    bool has_number = false;
    for (const auto &tok : result.tokens) {
        if (tok.type == token::literal::number::float_) {
            has_number = true;
        }
    }
    EXPECT_TRUE(has_number);
}

// ── Registration ────────────────────────────────────────────────────────

TEST_F(FtlLexerTest, registration) {
    register_ftl_lexer();
    auto lex = get_lexer_by_name("ftl");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "ftl");
}

TEST_F(FtlLexerTest, registration_by_filename) {
    register_ftl_lexer();
    auto lex = get_lexer_by_filename("messages.ftl");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "ftl");
}

TEST_F(FtlLexerTest, registration_by_alias) {
    register_ftl_lexer();
    auto lex = get_lexer_by_name("fluent");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "ftl");
}

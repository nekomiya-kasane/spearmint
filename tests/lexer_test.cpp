#include "spearmint/core/lexer.h"
#include "spearmint/core/lexer_registry.h"
#include "spearmint/core/regex_lexer.h"
#include "spearmint/lexers/python.h"

#include <gtest/gtest.h>

using namespace spearmint;

// ── Python lexer info ──────────────────────────────────────────────────

TEST(PythonLexerTest, Info) {
    lexers::python_lexer lex;
    EXPECT_STREQ(lex.info().name, "python");
    EXPECT_STREQ(lex.info().display_name, "Python");
    EXPECT_GT(lex.info().aliases.size(), 0u);
    EXPECT_GT(lex.info().filenames.size(), 0u);
}

// ── Basic tokenization ─────────────────────────────────────────────────

TEST(PythonLexerTest, SimpleAssignment) {
    lexers::python_lexer lex;
    auto result = lex.tokenize("x = 42\n");

    ASSERT_GE(result.size(), 3u);

    // Find the identifier "x"
    bool found_name = false;
    bool found_number = false;
    for (const auto &e : result) {
        if (e.text == "x") {
            EXPECT_EQ(e.type, token::name::self);
            found_name = true;
        }
        if (e.text == "42") {
            EXPECT_EQ(e.type, token::literal::number::integer);
            found_number = true;
        }
    }
    EXPECT_TRUE(found_name);
    EXPECT_TRUE(found_number);
}

TEST(PythonLexerTest, Keywords) {
    lexers::python_lexer lex;
    auto result = lex.tokenize("if True:\n    pass\n");

    bool found_if = false, found_true = false, found_pass = false;
    for (const auto &e : result) {
        if (e.text == "if") {
            EXPECT_EQ(e.type, token::keyword::self);
            found_if = true;
        }
        if (e.text == "True") {
            EXPECT_EQ(e.type, token::keyword::self);
            found_true = true;
        }
        if (e.text == "pass") {
            EXPECT_EQ(e.type, token::keyword::self);
            found_pass = true;
        }
    }
    EXPECT_TRUE(found_if);
    EXPECT_TRUE(found_true);
    EXPECT_TRUE(found_pass);
}

TEST(PythonLexerTest, Strings) {
    lexers::python_lexer lex;
    auto result = lex.tokenize(R"(s = "hello world")"
                               "\n");

    bool found_string = false;
    for (const auto &e : result) {
        if (e.text == "hello world") {
            EXPECT_EQ(e.type, token::literal::string::double_);
            found_string = true;
        }
    }
    EXPECT_TRUE(found_string);
}

TEST(PythonLexerTest, Comments) {
    lexers::python_lexer lex;
    auto result = lex.tokenize("# this is a comment\nx = 1\n");

    bool found_comment = false;
    for (const auto &e : result) {
        if (e.text.find("this is a comment") != std::string_view::npos) {
            EXPECT_EQ(e.type, token::comment::single);
            found_comment = true;
        }
    }
    EXPECT_TRUE(found_comment);
}

TEST(PythonLexerTest, Decorator) {
    lexers::python_lexer lex;
    auto result = lex.tokenize("@staticmethod\ndef foo(): pass\n");

    bool found_decorator = false;
    for (const auto &e : result) {
        if (e.text == "@staticmethod") {
            EXPECT_EQ(e.type, token::name::decorator);
            found_decorator = true;
        }
    }
    EXPECT_TRUE(found_decorator);
}

TEST(PythonLexerTest, Numbers) {
    lexers::python_lexer lex;
    auto result = lex.tokenize("a = 0xFF\nb = 3.14\nc = 0b1010\n");

    bool found_hex = false, found_float = false, found_bin = false;
    for (const auto &e : result) {
        if (e.text == "0xFF") {
            EXPECT_EQ(e.type, token::literal::number::hex);
            found_hex = true;
        }
        if (e.text == "3.14") {
            EXPECT_EQ(e.type, token::literal::number::float_);
            found_float = true;
        }
        if (e.text == "0b1010") {
            EXPECT_EQ(e.type, token::literal::number::bin);
            found_bin = true;
        }
    }
    EXPECT_TRUE(found_hex);
    EXPECT_TRUE(found_float);
    EXPECT_TRUE(found_bin);
}

TEST(PythonLexerTest, BuiltinFunctions) {
    lexers::python_lexer lex;
    auto result = lex.tokenize("print(len([1,2,3]))\n");

    bool found_print = false, found_len = false;
    for (const auto &e : result) {
        if (e.text == "print") {
            EXPECT_EQ(e.type, token::name::builtin);
            found_print = true;
        }
        if (e.text == "len") {
            EXPECT_EQ(e.type, token::name::builtin);
            found_len = true;
        }
    }
    EXPECT_TRUE(found_print);
    EXPECT_TRUE(found_len);
}

TEST(PythonLexerTest, AnalyseText) {
    lexers::python_lexer lex;
    EXPECT_GT(lex.analyse_text("#!/usr/bin/env python\nimport os\n"), 0.5f);
    EXPECT_GT(lex.analyse_text("def foo():\n    pass\n"), 0.0f);
    EXPECT_EQ(lex.analyse_text("int main() { return 0; }"), 0.0f);
}

TEST(PythonLexerTest, NoTokensLost) {
    lexers::python_lexer lex;
    std::string_view source = "x = 42\n";
    auto result = lex.tokenize(source);

    // Reconstruct source from tokens
    std::string reconstructed;
    for (const auto &e : result) {
        reconstructed.append(e.text);
    }
    EXPECT_EQ(reconstructed, source);
}

// ── Registry tests ─────────────────────────────────────────────────────

class LexerRegistryTest : public ::testing::Test {
  protected:
    void SetUp() override { lexers::register_python_lexer(); }
};

TEST_F(LexerRegistryTest, GetByName) {
    auto lex = get_lexer_by_name("python");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "python");
}

TEST_F(LexerRegistryTest, GetByAlias) {
    auto lex = get_lexer_by_name("py");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "python");
}

TEST_F(LexerRegistryTest, GetByFilename) {
    auto lex = get_lexer_by_filename("test.py");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "python");
}

TEST_F(LexerRegistryTest, GetByMime) {
    auto lex = get_lexer_by_mime("text/x-python");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "python");
}

TEST_F(LexerRegistryTest, GetAllLexers) {
    auto all = get_all_lexers();
    EXPECT_GE(all.size(), 1u);
}

TEST_F(LexerRegistryTest, NotFound) {
    auto lex = get_lexer_by_name("nonexistent_language");
    EXPECT_EQ(lex, nullptr);
}

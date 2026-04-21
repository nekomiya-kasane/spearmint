#include "spearmint/core/lexer_registry.h"
#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/lexers/cpp.h"
#include "spearmint/lexers/javascript.h"
#include "spearmint/lexers/json.h"

#include <gtest/gtest.h>

using namespace spearmint;

// ── C++ lexer ──────────────────────────────────────────────────────────

TEST(CppLexerTest, Info) {
    lexers::cpp_lexer lex;
    EXPECT_STREQ(lex.info().name, "cpp");
    EXPECT_STREQ(lex.info().display_name, "C++");
}

TEST(CppLexerTest, Keywords) {
    lexers::cpp_lexer lex;
    auto result = lex.tokenize("#include <iostream>\nint main() { return 0; }\n");

    bool found_int = false, found_return = false;
    for (const auto &e : result) {
        if (e.text == "int" && e.type == token::keyword::self) {
            found_int = true;
        }
        if (e.text == "return" && e.type == token::keyword::self) {
            found_return = true;
        }
    }
    EXPECT_TRUE(found_int);
    EXPECT_TRUE(found_return);
}

TEST(CppLexerTest, Preprocessor) {
    lexers::cpp_lexer lex;
    auto result = lex.tokenize("#include <vector>\n");

    bool found_preproc = false;
    for (const auto &e : result) {
        if (e.type == token::comment::preproc) {
            found_preproc = true;
        }
    }
    EXPECT_TRUE(found_preproc);
}

TEST(CppLexerTest, Comments) {
    lexers::cpp_lexer lex;
    auto result = lex.tokenize("// single\n/* multi */\n");

    bool found_single = false, found_multi = false;
    for (const auto &e : result) {
        if (e.type == token::comment::single) {
            found_single = true;
        }
        if (e.type == token::comment::multiline) {
            found_multi = true;
        }
    }
    EXPECT_TRUE(found_single);
    EXPECT_TRUE(found_multi);
}

TEST(CppLexerTest, Numbers) {
    lexers::cpp_lexer lex;
    auto result = lex.tokenize("int a = 0xFF; double b = 3.14; int c = 0b1010;\n");

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

TEST(CppLexerTest, Strings) {
    lexers::cpp_lexer lex;
    auto result = lex.tokenize("auto s = \"hello\";\n");

    bool found_str = false;
    for (const auto &e : result) {
        if (e.text == "hello" && e.type == token::literal::string::double_) {
            found_str = true;
        }
    }
    EXPECT_TRUE(found_str);
}

TEST(CppLexerTest, AnalyseText) {
    lexers::cpp_lexer lex;
    EXPECT_GT(lex.analyse_text("#include <iostream>\nint main() {}"), 0.3f);
    EXPECT_EQ(lex.analyse_text("def foo(): pass"), 0.0f);
}

TEST(CppLexerTest, Lossless) {
    lexers::cpp_lexer lex;
    std::string_view src = "int x = 42;\n";
    auto result = lex.tokenize(src);
    std::string reconstructed;
    for (const auto &e : result) {
        reconstructed.append(e.text);
    }
    EXPECT_EQ(reconstructed, src);
}

// ── JSON lexer ─────────────────────────────────────────────────────────

TEST(JsonLexerTest, Info) {
    lexers::json_lexer lex;
    EXPECT_STREQ(lex.info().name, "json");
}

TEST(JsonLexerTest, BasicObject) {
    lexers::json_lexer lex;
    auto result = lex.tokenize(R"({"key": "value", "num": 42, "flag": true})");

    bool found_str = false, found_int = false, found_bool = false;
    for (const auto &e : result) {
        if (e.text == "key" && e.type == token::literal::string::double_) {
            found_str = true;
        }
        if (e.text == "42" && e.type == token::literal::number::integer) {
            found_int = true;
        }
        if (e.text == "true" && e.type == token::keyword::constant) {
            found_bool = true;
        }
    }
    EXPECT_TRUE(found_str);
    EXPECT_TRUE(found_int);
    EXPECT_TRUE(found_bool);
}

TEST(JsonLexerTest, FloatAndNull) {
    lexers::json_lexer lex;
    auto result = lex.tokenize(R"({"pi": 3.14, "empty": null})");

    bool found_float = false, found_null = false;
    for (const auto &e : result) {
        if (e.text == "3.14" && e.type == token::literal::number::float_) {
            found_float = true;
        }
        if (e.text == "null" && e.type == token::keyword::constant) {
            found_null = true;
        }
    }
    EXPECT_TRUE(found_float);
    EXPECT_TRUE(found_null);
}

TEST(JsonLexerTest, EscapeSequences) {
    lexers::json_lexer lex;
    auto result = lex.tokenize(R"({"msg": "hello\nworld"})");

    bool found_escape = false;
    for (const auto &e : result) {
        if (e.type == token::literal::string::escape) {
            found_escape = true;
        }
    }
    EXPECT_TRUE(found_escape);
}

TEST(JsonLexerTest, Lossless) {
    lexers::json_lexer lex;
    std::string_view src = "{\"a\": 1}\n";
    auto result = lex.tokenize(src);
    std::string reconstructed;
    for (const auto &e : result) {
        reconstructed.append(e.text);
    }
    EXPECT_EQ(reconstructed, src);
}

// ── JavaScript lexer ───────────────────────────────────────────────────

TEST(JsLexerTest, Info) {
    lexers::javascript_lexer lex;
    EXPECT_STREQ(lex.info().name, "javascript");
}

TEST(JsLexerTest, Keywords) {
    lexers::javascript_lexer lex;
    auto result = lex.tokenize("const x = 42;\nlet y = 'hello';\n");

    bool found_const = false, found_let = false;
    for (const auto &e : result) {
        if (e.text == "const" && e.type == token::keyword::self) {
            found_const = true;
        }
        if (e.text == "let" && e.type == token::keyword::self) {
            found_let = true;
        }
    }
    EXPECT_TRUE(found_const);
    EXPECT_TRUE(found_let);
}

TEST(JsLexerTest, ArrowFunction) {
    lexers::javascript_lexer lex;
    auto result = lex.tokenize("const f = (x) => x * 2;\n");

    bool found_arrow = false;
    for (const auto &e : result) {
        if (e.text == "=>" && e.type == token::operator_::self) {
            found_arrow = true;
        }
    }
    EXPECT_TRUE(found_arrow);
}

TEST(JsLexerTest, TemplateLiteral) {
    lexers::javascript_lexer lex;
    auto result = lex.tokenize("const s = `hello ${name}`;\n");

    bool found_backtick = false;
    for (const auto &e : result) {
        if (e.type == token::literal::string::backtick) {
            found_backtick = true;
        }
    }
    EXPECT_TRUE(found_backtick);
}

TEST(JsLexerTest, Comments) {
    lexers::javascript_lexer lex;
    auto result = lex.tokenize("// line\n/* block */\n");

    bool found_single = false, found_multi = false;
    for (const auto &e : result) {
        if (e.type == token::comment::single) {
            found_single = true;
        }
        if (e.type == token::comment::multiline) {
            found_multi = true;
        }
    }
    EXPECT_TRUE(found_single);
    EXPECT_TRUE(found_multi);
}

TEST(JsLexerTest, BuiltinObjects) {
    lexers::javascript_lexer lex;
    auto result = lex.tokenize("console.log(JSON.stringify(obj));\n");

    bool found_console = false, found_json = false;
    for (const auto &e : result) {
        if (e.text == "console" && e.type == token::name::builtin) {
            found_console = true;
        }
        if (e.text == "JSON" && e.type == token::name::builtin) {
            found_json = true;
        }
    }
    EXPECT_TRUE(found_console);
    EXPECT_TRUE(found_json);
}

TEST(JsLexerTest, AnalyseText) {
    lexers::javascript_lexer lex;
    EXPECT_GT(lex.analyse_text("function foo() { console.log('hi'); }"), 0.2f);
    EXPECT_EQ(lex.analyse_text("#include <iostream>"), 0.0f);
}

TEST(JsLexerTest, Lossless) {
    lexers::javascript_lexer lex;
    std::string_view src = "let x = 1;\n";
    auto result = lex.tokenize(src);
    std::string reconstructed;
    for (const auto &e : result) {
        reconstructed.append(e.text);
    }
    EXPECT_EQ(reconstructed, src);
}

// ── Registry with all lexers ───────────────────────────────────────────

class MultiLexerRegistryTest : public ::testing::Test {
  protected:
    void SetUp() override {
        lexers::register_cpp_lexer();
        lexers::register_json_lexer();
        lexers::register_javascript_lexer();
    }
};

TEST_F(MultiLexerRegistryTest, FindCpp) {
    auto lex = get_lexer_by_name("cpp");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "cpp");
}

TEST_F(MultiLexerRegistryTest, FindJson) {
    auto lex = get_lexer_by_name("json");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "json");
}

TEST_F(MultiLexerRegistryTest, FindJs) {
    auto lex = get_lexer_by_name("javascript");
    ASSERT_NE(lex, nullptr);
    EXPECT_STREQ(lex->info().name, "javascript");
}

TEST_F(MultiLexerRegistryTest, FindByFilename) {
    auto cpp = get_lexer_by_filename("main.cpp");
    ASSERT_NE(cpp, nullptr);
    EXPECT_STREQ(cpp->info().name, "cpp");

    auto js = get_lexer_by_filename("app.js");
    ASSERT_NE(js, nullptr);
    EXPECT_STREQ(js->info().name, "javascript");

    auto json = get_lexer_by_filename("config.json");
    ASSERT_NE(json, nullptr);
    EXPECT_STREQ(json->info().name, "json");
}

#include "spearmint/core/style.h"
#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/exporters/ansi.h"
#include "spearmint/exporters/html.h"
#include "spearmint/exporters/latex.h"
#include "spearmint/exporters/svg.h"
#include "spearmint/styles/builtin.h"

#include <gtest/gtest.h>

using namespace spearmint;

static token_stream make_sample() {
    return {
        {token::keyword::self, "def"},   {token::whitespace, " "},        {token::name::function, "foo"},
        {token::punctuation::self, "("}, {token::punctuation::self, ")"}, {token::punctuation::self, ":"},
        {token::whitespace, "\n    "},   {token::keyword::self, "pass"},  {token::whitespace, "\n"},
    };
}

// ── HTML exporter ──────────────────────────────────────────────────────

TEST(HtmlExporterTest, ContainsPreCode) {
    exporters::html_exporter exp;
    style_def_view style(styles::monokai);
    auto html = exp.format(make_sample(), style);
    EXPECT_NE(html.find("<pre"), std::string::npos);
    EXPECT_NE(html.find("<code>"), std::string::npos);
    EXPECT_NE(html.find("</code></pre>"), std::string::npos);
}

TEST(HtmlExporterTest, ContainsColorSpans) {
    exporters::html_exporter exp;
    style_def_view style(styles::monokai);
    auto html = exp.format(make_sample(), style);
    EXPECT_NE(html.find("color:#"), std::string::npos);
    EXPECT_NE(html.find("<span"), std::string::npos);
}

TEST(HtmlExporterTest, EscapesHtml) {
    token_stream ts = {{token::operator_::self, "<>&\""}};
    exporters::html_exporter exp;
    style_def_view style(styles::monokai);
    auto html = exp.format(ts, style);
    EXPECT_NE(html.find("&lt;"), std::string::npos);
    EXPECT_NE(html.find("&gt;"), std::string::npos);
    EXPECT_NE(html.find("&amp;"), std::string::npos);
    EXPECT_EQ(html.find("<>&"), std::string::npos);
}

TEST(HtmlExporterTest, FullPage) {
    exporters::html_exporter exp({.full_page = true});
    style_def_view style(styles::monokai);
    auto html = exp.format(make_sample(), style);
    EXPECT_NE(html.find("<!DOCTYPE html>"), std::string::npos);
    EXPECT_NE(html.find("</html>"), std::string::npos);
}

TEST(HtmlExporterTest, BackgroundColor) {
    exporters::html_exporter exp;
    style_def_view style(styles::monokai);
    auto html = exp.format(make_sample(), style);
    EXPECT_NE(html.find("background-color:#272822"), std::string::npos);
}

// ── ANSI exporter ──────────────────────────────────────────────────────

TEST(AnsiExporterTest, ContainsSGR) {
    exporters::ansi_exporter exp;
    style_def_view style(styles::monokai);
    auto ansi = exp.format(make_sample(), style);
    EXPECT_NE(ansi.find("\033["), std::string::npos);
    EXPECT_NE(ansi.find("\033[0m"), std::string::npos);
}

TEST(AnsiExporterTest, ContainsSourceText) {
    exporters::ansi_exporter exp;
    style_def_view style(styles::monokai);
    auto ansi = exp.format(make_sample(), style);
    EXPECT_NE(ansi.find("def"), std::string::npos);
    EXPECT_NE(ansi.find("foo"), std::string::npos);
    EXPECT_NE(ansi.find("pass"), std::string::npos);
}

TEST(AnsiExporterTest, TrueColorFormat) {
    exporters::ansi_exporter exp;
    style_def_view style(styles::monokai);
    auto ansi = exp.format(make_sample(), style);
    // Should contain 38;2;R;G;B format
    EXPECT_NE(ansi.find("38;2;"), std::string::npos);
}

// ── SVG exporter ───────────────────────────────────────────────────────

TEST(SvgExporterTest, ContainsSvgTags) {
    exporters::svg_exporter exp;
    style_def_view style(styles::monokai);
    auto svg = exp.format(make_sample(), style);
    EXPECT_NE(svg.find("<svg"), std::string::npos);
    EXPECT_NE(svg.find("</svg>"), std::string::npos);
}

TEST(SvgExporterTest, ContainsTextElements) {
    exporters::svg_exporter exp;
    style_def_view style(styles::monokai);
    auto svg = exp.format(make_sample(), style);
    EXPECT_NE(svg.find("<text"), std::string::npos);
    EXPECT_NE(svg.find("def"), std::string::npos);
}

TEST(SvgExporterTest, ContainsBackground) {
    exporters::svg_exporter exp;
    style_def_view style(styles::monokai);
    auto svg = exp.format(make_sample(), style);
    EXPECT_NE(svg.find("<rect"), std::string::npos);
    EXPECT_NE(svg.find("#272822"), std::string::npos);
}

// ── LaTeX exporter ─────────────────────────────────────────────────────

TEST(LatexExporterTest, ContainsVerbatim) {
    exporters::latex_exporter exp;
    style_def_view style(styles::monokai);
    auto tex = exp.format(make_sample(), style);
    EXPECT_NE(tex.find("\\begin{Verbatim}"), std::string::npos);
    EXPECT_NE(tex.find("\\end{Verbatim}"), std::string::npos);
}

TEST(LatexExporterTest, ContainsTextcolor) {
    exporters::latex_exporter exp;
    style_def_view style(styles::monokai);
    auto tex = exp.format(make_sample(), style);
    EXPECT_NE(tex.find("\\textcolor[RGB]"), std::string::npos);
}

TEST(LatexExporterTest, EscapesSpecialChars) {
    token_stream ts = {{token::operator_::self, "$&%#_{}"}};
    exporters::latex_exporter exp;
    style_def_view style(styles::monokai);
    auto tex = exp.format(ts, style);
    EXPECT_NE(tex.find("\\$"), std::string::npos);
    EXPECT_NE(tex.find("\\&"), std::string::npos);
    EXPECT_NE(tex.find("\\%"), std::string::npos);
    EXPECT_NE(tex.find("\\#"), std::string::npos);
    EXPECT_NE(tex.find("\\_"), std::string::npos);
}

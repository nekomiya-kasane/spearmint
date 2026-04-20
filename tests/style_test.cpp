#include "spearmint/core/style.h"
#include "spearmint/core/style_registry.h"
#include "spearmint/styles/builtin.h"

#include <gtest/gtest.h>

using namespace spearmint;

// ── static_assert: style_rule ──────────────────────────────────────────

static_assert(style_rule{}.has_fg == false);
static_assert(style_rule{}.has_bg == false);
static_assert(style_rule::with_fg(0xF92672).fg == 0xF92672);
static_assert(style_rule::with_fg(0xF92672).has_fg == true);
static_assert(style_rule::with_fg(0xF92672).has_bg == false);

// ── static_assert: monokai constexpr lookup ────────────────────────────

static_assert(styles::monokai.lookup(token::keyword::self) != nullptr);
static_assert(styles::monokai.lookup(token::keyword::self)->fg == 0xf92672);
static_assert(styles::monokai.lookup(token::keyword::self)->has_fg == true);

// keyword::reserved inherits from keyword::self
static_assert(styles::monokai.lookup(token::keyword::reserved) != nullptr);
static_assert(styles::monokai.lookup(token::keyword::reserved)->fg == 0xf92672);

// string tokens
static_assert(styles::monokai.lookup(token::literal::string::self) != nullptr);
static_assert(styles::monokai.lookup(token::literal::string::self)->fg == 0xe6db74);

// string subtypes inherit from string::self
static_assert(styles::monokai.lookup(token::literal::string::single) != nullptr);
static_assert(styles::monokai.lookup(token::literal::string::single)->fg == 0xe6db74);

// number
static_assert(styles::monokai.lookup(token::literal::number::hex) != nullptr);
static_assert(styles::monokai.lookup(token::literal::number::hex)->fg == 0xae81ff);

// ── static_assert: default_ style ──────────────────────────────────────

static_assert(styles::default_.lookup(token::keyword::self) != nullptr);
static_assert(styles::default_.lookup(token::keyword::self)->bold == true);
static_assert(styles::default_.lookup(token::keyword::self)->fg == 0x008000);

// ── static_assert: bw style (no colors, only attributes) ───────────────

static_assert(styles::bw.lookup(token::keyword::self) != nullptr);
static_assert(styles::bw.lookup(token::keyword::self)->bold == true);
static_assert(styles::bw.lookup(token::keyword::self)->has_fg == false);

// ── static_assert: style_def_view from static_style_def ────────────────

constexpr style_def_view monokai_view(styles::monokai);
static_assert(monokai_view.name == std::string_view("monokai"));
static_assert(monokai_view.has_background == true);
static_assert(monokai_view.background_color == 0x272822);

// ── Runtime tests ──────────────────────────────────────────────────────

TEST(StyleTest, MonokaiLookup) {
    const auto *rule = styles::monokai.lookup(token::keyword::self);
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->fg, 0xf92672u);
    EXPECT_TRUE(rule->has_fg);
}

TEST(StyleTest, MonokaiInheritance) {
    // keyword::pseudo has no explicit rule → inherits from keyword::self
    const auto *rule = styles::monokai.lookup(token::keyword::pseudo);
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->fg, 0xf92672u);
}

TEST(StyleTest, StyleDefViewLookup) {
    style_def_view view(styles::monokai);
    const auto *rule = view.lookup(token::name::function);
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->fg, 0xa6e22eu);
}

TEST(StyleTest, StyleDefViewInheritance) {
    // dracula has name::variable rule; variable_class inherits from variable
    style_def_view view(styles::dracula);
    const auto *rule = view.lookup(token::name::variable_class);
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->fg, 0x8be9fdu); // same as name::variable in dracula
}

TEST(StyleTest, RegistryGetStyle) {
    const auto *s = get_style("monokai");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->name, "monokai");
    EXPECT_EQ(s->background_color, 0x272822u);
}

TEST(StyleTest, RegistryGetAllStyles) {
    auto names = get_all_styles();
    EXPECT_GE(names.size(), 15u);

    // Check a few known styles
    bool found_monokai = false, found_dracula = false, found_bw = false;
    for (auto n : names) {
        if (n == "monokai") found_monokai = true;
        if (n == "dracula") found_dracula = true;
        if (n == "bw") found_bw = true;
    }
    EXPECT_TRUE(found_monokai);
    EXPECT_TRUE(found_dracula);
    EXPECT_TRUE(found_bw);
}

TEST(StyleTest, RegistryGetStyleNotFound) {
    EXPECT_EQ(get_style("nonexistent"), nullptr);
}

TEST(StyleTest, RegistryRegisterCustom) {
    static constexpr static_style_def<1> custom = {"custom-test",
                                                   "Custom Test",
                                                   0x000000,
                                                   true,
                                                   0,
                                                   false,
                                                   0,
                                                   0,
                                                   {{
                                                       {token::keyword::self, style_rule::with_fg(0xFF0000)},
                                                   }}};

    register_style(style_def_view(custom));
    const auto *s = get_style("custom-test");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->name, "custom-test");
    const auto *rule = s->lookup(token::keyword::self);
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->fg, 0xFF0000u);
}

TEST(StyleTest, AllBuiltinStylesHaveEntries) {
    auto names = get_all_styles();
    for (auto n : names) {
        const auto *s = get_style(n);
        ASSERT_NE(s, nullptr) << "Style " << n << " not found";
        EXPECT_GT(s->entries.size(), 0u) << "Style " << n << " has no entries";
    }
}

TEST(StyleTest, DraculaColors) {
    const auto *rule = styles::dracula.lookup(token::keyword::self);
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->fg, 0xff79c6u);
}

TEST(StyleTest, NordColors) {
    const auto *rule = styles::nord.lookup(token::literal::string::self);
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->fg, 0xa3be8cu);
}

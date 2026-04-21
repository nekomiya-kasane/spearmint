#pragma once

/**
 * @file builtin.h
 * @brief All builtin Pygments styles as constexpr globals.
 *
 * Each style is a static_style_def<N> with token→style_rule mappings
 * ported from Pygments source. Colors are 0xRRGGBB.
 */

#include "spearmint/core/style.h"

namespace spearmint::styles {

    // ── Helper macro for concise rule definitions ──────────────────────────

#define SM_FG(color)                                                                                                   \
    style_rule {                                                                                                       \
        color, 0, false, false, false, false, false, false, false, true, false                                         \
    }
#define SM_BG(color)                                                                                                   \
    style_rule {                                                                                                       \
        0, color, false, false, false, false, false, false, false, false, true                                         \
    }
#define SM_FGBG(f, b)                                                                                                  \
    style_rule {                                                                                                       \
        f, b, false, false, false, false, false, false, false, true, true                                              \
    }
#define SM_BOLD(color)                                                                                                 \
    style_rule {                                                                                                       \
        color, 0, true, false, false, false, false, false, false, true, false                                          \
    }
#define SM_ITALIC(color)                                                                                               \
    style_rule {                                                                                                       \
        color, 0, false, true, false, false, false, false, false, true, false                                          \
    }
#define SM_BOLDITALIC(c)                                                                                               \
    style_rule {                                                                                                       \
        c, 0, true, true, false, false, false, false, false, true, false                                               \
    }
#define SM_UL(color)                                                                                                   \
    style_rule {                                                                                                       \
        color, 0, false, false, true, false, false, false, false, true, false                                          \
    }
#define SM_BOLDUL(color)                                                                                               \
    style_rule {                                                                                                       \
        color, 0, true, false, true, false, false, false, false, true, false                                           \
    }
#define SM_PLAIN_BOLD                                                                                                  \
    style_rule {                                                                                                       \
        0, 0, true, false, false, false, false, false, false, false, false                                             \
    }
#define SM_PLAIN_ITALIC                                                                                                \
    style_rule {                                                                                                       \
        0, 0, false, true, false, false, false, false, false, false, false                                             \
    }
#define SM_PLAIN_UL                                                                                                    \
    style_rule {                                                                                                       \
        0, 0, false, false, true, false, false, false, false, false, false                                             \
    }
#define SM_PLAIN_NOBOLD                                                                                                \
    style_rule {                                                                                                       \
        0, 0, false, false, false, false, true, false, false, false, false                                             \
    }

    namespace tk = spearmint::token;

    // ═══════════════════════════════════════════════════════════════════════
    // monokai
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<24> monokai = {"monokai",
                                                     "Monokai",
                                                     0x272822,
                                                     true,
                                                     0x49483e,
                                                     true,
                                                     0x8f908a,
                                                     0x49483e,
                                                     {{
                                                         {tk::text, SM_FG(0xf8f8f2)},
                                                         {tk::error, SM_FGBG(0xf8f8f0, 0xf92672)},
                                                         {tk::comment::self, SM_FG(0x75715e)},
                                                         {tk::keyword::self, SM_FG(0xf92672)},
                                                         {tk::keyword::constant, SM_FG(0x66d9ef)},
                                                         {tk::keyword::declaration, SM_FG(0x66d9ef)},
                                                         {tk::keyword::namespace_, SM_FG(0xf92672)},
                                                         {tk::keyword::type, SM_ITALIC(0x66d9ef)},
                                                         {tk::name::class_, SM_BOLD(0xa6e22e)},
                                                         {tk::name::constant, SM_FG(0x66d9ef)},
                                                         {tk::name::decorator, SM_FG(0xa6e22e)},
                                                         {tk::name::exception, SM_BOLD(0xa6e22e)},
                                                         {tk::name::function, SM_FG(0xa6e22e)},
                                                         {tk::name::other, SM_FG(0xa6e22e)},
                                                         {tk::name::tag, SM_FG(0xf92672)},
                                                         {tk::name::attribute, SM_FG(0xa6e22e)},
                                                         {tk::literal::string::self, SM_FG(0xe6db74)},
                                                         {tk::literal::number::self, SM_FG(0xae81ff)},
                                                         {tk::operator_::self, SM_FG(0xf92672)},
                                                         {tk::operator_::word, SM_FG(0xf92672)},
                                                         {tk::punctuation::self, SM_FG(0xf8f8f2)},
                                                         {tk::generic::deleted, SM_FG(0xf92672)},
                                                         {tk::generic::inserted, SM_FG(0xa6e22e)},
                                                         {tk::generic::emph, SM_PLAIN_ITALIC},
                                                     }}};

    // ═══════════════════════════════════════════════════════════════════════
    // default_ (Pygments default)
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<30> default_ = {"default",
                                                      "Default",
                                                      0xf8f8f8,
                                                      true,
                                                      0xffffcc,
                                                      true,
                                                      0x666666,
                                                      0,
                                                      {{
                                                          {tk::whitespace, SM_FG(0xbbbbbb)},
                                                          {tk::error, SM_FGBG(0xff0000, 0xffaaaa)},
                                                          {tk::comment::self, SM_ITALIC(0x3d7b7b)},
                                                          {tk::comment::preproc, SM_FG(0x9c6500)},
                                                          {tk::comment::special, SM_BOLDITALIC(0x3d7b7b)},
                                                          {tk::keyword::self, SM_BOLD(0x008000)},
                                                          {tk::keyword::constant, SM_BOLD(0x008000)},
                                                          {tk::keyword::namespace_, SM_BOLD(0x008000)},
                                                          {tk::keyword::pseudo, SM_BOLD(0x008000)},
                                                          {tk::keyword::type, SM_FG(0xb00040)},
                                                          {tk::name::builtin, SM_FG(0x008000)},
                                                          {tk::name::class_, SM_BOLD(0x0000ff)},
                                                          {tk::name::constant, SM_FG(0x880000)},
                                                          {tk::name::decorator, SM_FG(0xaa22ff)},
                                                          {tk::name::entity, SM_BOLD(0x999999)},
                                                          {tk::name::exception, SM_BOLD(0xd2413a)},
                                                          {tk::name::function, SM_FG(0x0000ff)},
                                                          {tk::name::label, SM_FG(0xa0a000)},
                                                          {tk::name::namespace_, SM_BOLD(0x0000ff)},
                                                          {tk::name::tag, SM_BOLD(0x008000)},
                                                          {tk::name::variable, SM_FG(0x19177c)},
                                                          {tk::name::attribute, SM_FG(0x687822)},
                                                          {tk::literal::string::self, SM_FG(0xba2121)},
                                                          {tk::literal::string::doc, SM_ITALIC(0xba2121)},
                                                          {tk::literal::string::escape, SM_BOLD(0xbb6622)},
                                                          {tk::literal::string::interpol, SM_BOLD(0xbb6688)},
                                                          {tk::literal::string::regex, SM_FG(0xbb6688)},
                                                          {tk::literal::number::self, SM_FG(0x666666)},
                                                          {tk::operator_::self, SM_FG(0x666666)},
                                                          {tk::operator_::word, SM_BOLD(0xaa22ff)},
                                                      }}};

    // ═══════════════════════════════════════════════════════════════════════
    // dracula
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> dracula = {"dracula",
                                                     "Dracula",
                                                     0x282a36,
                                                     true,
                                                     0x44475a,
                                                     true,
                                                     0x6272a4,
                                                     0,
                                                     {{
                                                         {tk::text, SM_FG(0xf8f8f2)},
                                                         {tk::error, SM_FG(0xf8f8f2)},
                                                         {tk::comment::self, SM_FG(0x6272a4)},
                                                         {tk::keyword::self, SM_FG(0xff79c6)},
                                                         {tk::keyword::constant, SM_FG(0xff79c6)},
                                                         {tk::keyword::type, SM_FG(0x8be9fd)},
                                                         {tk::name::builtin, SM_ITALIC(0x8be9fd)},
                                                         {tk::name::class_, SM_FG(0x50fa7b)},
                                                         {tk::name::decorator, SM_FG(0x50fa7b)},
                                                         {tk::name::exception, SM_FG(0x50fa7b)},
                                                         {tk::name::function, SM_FG(0x50fa7b)},
                                                         {tk::name::tag, SM_FG(0xff79c6)},
                                                         {tk::name::attribute, SM_FG(0x50fa7b)},
                                                         {tk::name::variable, SM_ITALIC(0x8be9fd)},
                                                         {tk::literal::string::self, SM_FG(0xf1fa8c)},
                                                         {tk::literal::number::self, SM_FG(0xbd93f9)},
                                                         {tk::operator_::self, SM_FG(0xff79c6)},
                                                         {tk::punctuation::self, SM_FG(0xf8f8f2)},
                                                         {tk::generic::deleted, SM_FG(0xff5555)},
                                                         {tk::generic::inserted, SM_FG(0x50fa7b)},
                                                         {tk::generic::heading, SM_BOLD(0xf8f8f2)},
                                                         {tk::generic::emph, SM_PLAIN_ITALIC},
                                                     }}};

    // ═══════════════════════════════════════════════════════════════════════
    // nord
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> nord = {"nord",
                                                  "Nord",
                                                  0x2e3440,
                                                  true,
                                                  0x434c5e,
                                                  true,
                                                  0x616e88,
                                                  0,
                                                  {{
                                                      {tk::text, SM_FG(0xd8dee9)},
                                                      {tk::error, SM_FG(0xbf616a)},
                                                      {tk::comment::self, SM_ITALIC(0x616e88)},
                                                      {tk::keyword::self, SM_BOLD(0x81a1c1)},
                                                      {tk::keyword::constant, SM_FG(0x81a1c1)},
                                                      {tk::keyword::type, SM_FG(0x81a1c1)},
                                                      {tk::name::builtin, SM_FG(0x88c0d0)},
                                                      {tk::name::class_, SM_FG(0x8fbcbb)},
                                                      {tk::name::decorator, SM_FG(0xd08770)},
                                                      {tk::name::exception, SM_FG(0xbf616a)},
                                                      {tk::name::function, SM_FG(0x88c0d0)},
                                                      {tk::name::tag, SM_FG(0x81a1c1)},
                                                      {tk::name::attribute, SM_FG(0x8fbcbb)},
                                                      {tk::name::variable, SM_FG(0xd8dee9)},
                                                      {tk::literal::string::self, SM_FG(0xa3be8c)},
                                                      {tk::literal::string::escape, SM_FG(0xebcb8b)},
                                                      {tk::literal::number::self, SM_FG(0xb48ead)},
                                                      {tk::operator_::self, SM_FG(0x81a1c1)},
                                                      {tk::punctuation::self, SM_FG(0xeceff4)},
                                                      {tk::generic::deleted, SM_FG(0xbf616a)},
                                                      {tk::generic::inserted, SM_FG(0xa3be8c)},
                                                      {tk::generic::emph, SM_PLAIN_ITALIC},
                                                  }}};

    // ═══════════════════════════════════════════════════════════════════════
    // one_dark
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> one_dark = {"one-dark",
                                                      "One Dark",
                                                      0x282c34,
                                                      true,
                                                      0x3e4451,
                                                      true,
                                                      0x636d83,
                                                      0,
                                                      {{
                                                          {tk::text, SM_FG(0xabb2bf)},
                                                          {tk::error, SM_FG(0xe06c75)},
                                                          {tk::comment::self, SM_ITALIC(0x5c6370)},
                                                          {tk::keyword::self, SM_FG(0xc678dd)},
                                                          {tk::keyword::constant, SM_FG(0xd19a66)},
                                                          {tk::keyword::type, SM_FG(0xe5c07b)},
                                                          {tk::name::builtin, SM_FG(0xe5c07b)},
                                                          {tk::name::class_, SM_FG(0xe5c07b)},
                                                          {tk::name::decorator, SM_FG(0xe5c07b)},
                                                          {tk::name::exception, SM_FG(0xe06c75)},
                                                          {tk::name::function, SM_FG(0x61afef)},
                                                          {tk::name::tag, SM_FG(0xe06c75)},
                                                          {tk::name::attribute, SM_FG(0xd19a66)},
                                                          {tk::name::variable, SM_FG(0xe06c75)},
                                                          {tk::literal::string::self, SM_FG(0x98c379)},
                                                          {tk::literal::string::escape, SM_FG(0x56b6c2)},
                                                          {tk::literal::number::self, SM_FG(0xd19a66)},
                                                          {tk::operator_::self, SM_FG(0x56b6c2)},
                                                          {tk::punctuation::self, SM_FG(0xabb2bf)},
                                                          {tk::generic::deleted, SM_FG(0xe06c75)},
                                                          {tk::generic::inserted, SM_FG(0x98c379)},
                                                          {tk::generic::emph, SM_PLAIN_ITALIC},
                                                      }}};

    // ═══════════════════════════════════════════════════════════════════════
    // solarized_dark
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> solarized_dark = {"solarized-dark",
                                                            "Solarized Dark",
                                                            0x002b36,
                                                            true,
                                                            0x073642,
                                                            true,
                                                            0x586e75,
                                                            0,
                                                            {{
                                                                {tk::text, SM_FG(0x839496)},
                                                                {tk::error, SM_FG(0xdc322f)},
                                                                {tk::comment::self, SM_ITALIC(0x586e75)},
                                                                {tk::keyword::self, SM_BOLD(0x859900)},
                                                                {tk::keyword::constant, SM_FG(0x2aa198)},
                                                                {tk::keyword::type, SM_FG(0xb58900)},
                                                                {tk::name::builtin, SM_FG(0x268bd2)},
                                                                {tk::name::class_, SM_BOLD(0x268bd2)},
                                                                {tk::name::decorator, SM_FG(0x268bd2)},
                                                                {tk::name::exception, SM_FG(0xcb4b16)},
                                                                {tk::name::function, SM_FG(0x268bd2)},
                                                                {tk::name::tag, SM_FG(0x268bd2)},
                                                                {tk::name::attribute, SM_FG(0x93a1a1)},
                                                                {tk::name::variable, SM_FG(0xcb4b16)},
                                                                {tk::literal::string::self, SM_FG(0x2aa198)},
                                                                {tk::literal::string::escape, SM_FG(0xcb4b16)},
                                                                {tk::literal::number::self, SM_FG(0x2aa198)},
                                                                {tk::operator_::self, SM_FG(0x839496)},
                                                                {tk::punctuation::self, SM_FG(0x839496)},
                                                                {tk::generic::deleted, SM_FG(0xdc322f)},
                                                                {tk::generic::inserted, SM_FG(0x859900)},
                                                                {tk::generic::emph, SM_PLAIN_ITALIC},
                                                            }}};

    // ═══════════════════════════════════════════════════════════════════════
    // solarized_light
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> solarized_light = {"solarized-light",
                                                             "Solarized Light",
                                                             0xfdf6e3,
                                                             true,
                                                             0xeee8d5,
                                                             true,
                                                             0x93a1a1,
                                                             0,
                                                             {{
                                                                 {tk::text, SM_FG(0x657b83)},
                                                                 {tk::error, SM_FG(0xdc322f)},
                                                                 {tk::comment::self, SM_ITALIC(0x93a1a1)},
                                                                 {tk::keyword::self, SM_BOLD(0x859900)},
                                                                 {tk::keyword::constant, SM_FG(0x2aa198)},
                                                                 {tk::keyword::type, SM_FG(0xb58900)},
                                                                 {tk::name::builtin, SM_FG(0x268bd2)},
                                                                 {tk::name::class_, SM_BOLD(0x268bd2)},
                                                                 {tk::name::decorator, SM_FG(0x268bd2)},
                                                                 {tk::name::exception, SM_FG(0xcb4b16)},
                                                                 {tk::name::function, SM_FG(0x268bd2)},
                                                                 {tk::name::tag, SM_FG(0x268bd2)},
                                                                 {tk::name::attribute, SM_FG(0x586e75)},
                                                                 {tk::name::variable, SM_FG(0xcb4b16)},
                                                                 {tk::literal::string::self, SM_FG(0x2aa198)},
                                                                 {tk::literal::string::escape, SM_FG(0xcb4b16)},
                                                                 {tk::literal::number::self, SM_FG(0x2aa198)},
                                                                 {tk::operator_::self, SM_FG(0x657b83)},
                                                                 {tk::punctuation::self, SM_FG(0x657b83)},
                                                                 {tk::generic::deleted, SM_FG(0xdc322f)},
                                                                 {tk::generic::inserted, SM_FG(0x859900)},
                                                                 {tk::generic::emph, SM_PLAIN_ITALIC},
                                                             }}};

    // ═══════════════════════════════════════════════════════════════════════
    // gruvbox_dark
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> gruvbox_dark = {"gruvbox-dark",
                                                          "Gruvbox Dark",
                                                          0x282828,
                                                          true,
                                                          0x3c3836,
                                                          true,
                                                          0x665c54,
                                                          0,
                                                          {{
                                                              {tk::text, SM_FG(0xebdbb2)},
                                                              {tk::error, SM_FG(0xfb4934)},
                                                              {tk::comment::self, SM_ITALIC(0x928374)},
                                                              {tk::keyword::self, SM_FG(0xfb4934)},
                                                              {tk::keyword::constant, SM_FG(0xd3869b)},
                                                              {tk::keyword::type, SM_FG(0xfabd2f)},
                                                              {tk::name::builtin, SM_FG(0xfabd2f)},
                                                              {tk::name::class_, SM_FG(0xfabd2f)},
                                                              {tk::name::decorator, SM_FG(0x8ec07c)},
                                                              {tk::name::exception, SM_FG(0xfb4934)},
                                                              {tk::name::function, SM_FG(0x8ec07c)},
                                                              {tk::name::tag, SM_FG(0xfb4934)},
                                                              {tk::name::attribute, SM_FG(0xb8bb26)},
                                                              {tk::name::variable, SM_FG(0x83a598)},
                                                              {tk::literal::string::self, SM_FG(0xb8bb26)},
                                                              {tk::literal::string::escape, SM_FG(0xfe8019)},
                                                              {tk::literal::number::self, SM_FG(0xd3869b)},
                                                              {tk::operator_::self, SM_FG(0xfe8019)},
                                                              {tk::punctuation::self, SM_FG(0xebdbb2)},
                                                              {tk::generic::deleted, SM_FG(0xfb4934)},
                                                              {tk::generic::inserted, SM_FG(0xb8bb26)},
                                                              {tk::generic::emph, SM_PLAIN_ITALIC},
                                                          }}};

    // ═══════════════════════════════════════════════════════════════════════
    // vim
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> vim = {"vim",
                                                 "Vim",
                                                 0x000000,
                                                 true,
                                                 0x222222,
                                                 true,
                                                 0x808080,
                                                 0,
                                                 {{
                                                     {tk::text, SM_FG(0xcccccc)},
                                                     {tk::error, SM_FGBG(0xcccccc, 0xff0000)},
                                                     {tk::comment::self, SM_FG(0x000080)},
                                                     {tk::keyword::self, SM_FG(0xcdcd00)},
                                                     {tk::keyword::type, SM_FG(0x00cd00)},
                                                     {tk::name::builtin, SM_FG(0xcd00cd)},
                                                     {tk::name::class_, SM_BOLD(0x00cdcd)},
                                                     {tk::name::exception, SM_BOLD(0x666699)},
                                                     {tk::name::function, SM_FG(0xcccccc)},
                                                     {tk::name::tag, SM_FG(0xcccccc)},
                                                     {tk::name::variable, SM_FG(0x00cdcd)},
                                                     {tk::name::decorator, SM_FG(0xcccccc)},
                                                     {tk::name::attribute, SM_FG(0xcccccc)},
                                                     {tk::literal::string::self, SM_FG(0xcd0000)},
                                                     {tk::literal::number::self, SM_FG(0xcd00cd)},
                                                     {tk::operator_::self, SM_FG(0x3399cc)},
                                                     {tk::punctuation::self, SM_FG(0xcccccc)},
                                                     {tk::generic::deleted, SM_FG(0xcd0000)},
                                                     {tk::generic::inserted, SM_FG(0x00cd00)},
                                                     {tk::generic::heading, SM_BOLD(0x000080)},
                                                     {tk::generic::subheading, SM_FG(0x800080)},
                                                     {tk::generic::emph, SM_PLAIN_ITALIC},
                                                 }}};

    // ═══════════════════════════════════════════════════════════════════════
    // github_dark
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> github_dark = {"github-dark",
                                                         "GitHub Dark",
                                                         0x0d1117,
                                                         true,
                                                         0x161b22,
                                                         true,
                                                         0x8b949e,
                                                         0,
                                                         {{
                                                             {tk::text, SM_FG(0xc9d1d9)},
                                                             {tk::error, SM_FG(0xf85149)},
                                                             {tk::comment::self, SM_ITALIC(0x8b949e)},
                                                             {tk::keyword::self, SM_FG(0xff7b72)},
                                                             {tk::keyword::constant, SM_FG(0x79c0ff)},
                                                             {tk::keyword::type, SM_FG(0xff7b72)},
                                                             {tk::name::builtin, SM_FG(0xffa657)},
                                                             {tk::name::class_, SM_FG(0xffa657)},
                                                             {tk::name::decorator, SM_FG(0xd2a8ff)},
                                                             {tk::name::exception, SM_FG(0xffa657)},
                                                             {tk::name::function, SM_FG(0xd2a8ff)},
                                                             {tk::name::tag, SM_FG(0x7ee787)},
                                                             {tk::name::attribute, SM_FG(0x79c0ff)},
                                                             {tk::name::variable, SM_FG(0xffa657)},
                                                             {tk::literal::string::self, SM_FG(0xa5d6ff)},
                                                             {tk::literal::string::escape, SM_FG(0x79c0ff)},
                                                             {tk::literal::number::self, SM_FG(0x79c0ff)},
                                                             {tk::operator_::self, SM_FG(0xff7b72)},
                                                             {tk::punctuation::self, SM_FG(0xc9d1d9)},
                                                             {tk::generic::deleted, SM_FGBG(0xffdcd7, 0x67060c)},
                                                             {tk::generic::inserted, SM_FGBG(0xaff5b4, 0x033a16)},
                                                             {tk::generic::emph, SM_PLAIN_ITALIC},
                                                         }}};

    // ═══════════════════════════════════════════════════════════════════════
    // emacs
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<26> emacs = {"emacs",
                                                   "Emacs",
                                                   0xf8f8f8,
                                                   true,
                                                   0xffffcc,
                                                   true,
                                                   0x666666,
                                                   0,
                                                   {{
                                                       {tk::whitespace, SM_FG(0xbbbbbb)},
                                                       {tk::error, SM_FGBG(0xff0000, 0xffaaaa)},
                                                       {tk::comment::self, SM_ITALIC(0x008800)},
                                                       {tk::comment::preproc, SM_FG(0x008800)},
                                                       {tk::comment::special, SM_BOLDITALIC(0x008800)},
                                                       {tk::keyword::self, SM_BOLD(0xaa22ff)},
                                                       {tk::keyword::pseudo, SM_FG(0xaa22ff)},
                                                       {tk::keyword::type, SM_BOLD(0x00bb00)},
                                                       {tk::name::builtin, SM_FG(0x00bb00)},
                                                       {tk::name::class_, SM_BOLD(0x0000ff)},
                                                       {tk::name::constant, SM_FG(0x880000)},
                                                       {tk::name::decorator, SM_FG(0xaa22ff)},
                                                       {tk::name::entity, SM_BOLD(0x999999)},
                                                       {tk::name::exception, SM_BOLD(0xd2413a)},
                                                       {tk::name::function, SM_FG(0x00aa88)},
                                                       {tk::name::label, SM_FG(0xa0a000)},
                                                       {tk::name::namespace_, SM_BOLD(0x0000ff)},
                                                       {tk::name::tag, SM_BOLD(0x008000)},
                                                       {tk::name::variable, SM_FG(0xaa0000)},
                                                       {tk::name::attribute, SM_FG(0x1e90ff)},
                                                       {tk::literal::string::self, SM_FG(0xbb4444)},
                                                       {tk::literal::string::doc, SM_ITALIC(0xbb4444)},
                                                       {tk::literal::string::escape, SM_BOLD(0xbb6622)},
                                                       {tk::literal::string::interpol, SM_BOLD(0xbb6688)},
                                                       {tk::literal::number::self, SM_FG(0x666666)},
                                                       {tk::operator_::word, SM_BOLD(0xaa22ff)},
                                                   }}};

    // ═══════════════════════════════════════════════════════════════════════
    // friendly
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<26> friendly = {"friendly",
                                                      "Friendly",
                                                      0xf0f0f0,
                                                      true,
                                                      0xffffcc,
                                                      true,
                                                      0x666666,
                                                      0,
                                                      {{
                                                          {tk::whitespace, SM_FG(0xbbbbbb)},
                                                          {tk::error, SM_FGBG(0xff0000, 0xffaaaa)},
                                                          {tk::comment::self, SM_ITALIC(0x60a0b0)},
                                                          {tk::comment::preproc, SM_FG(0x007020)},
                                                          {tk::comment::special, SM_BOLDITALIC(0x60a0b0)},
                                                          {tk::keyword::self, SM_BOLD(0x007020)},
                                                          {tk::keyword::pseudo, SM_FG(0x007020)},
                                                          {tk::keyword::type, SM_FG(0x902000)},
                                                          {tk::name::builtin, SM_FG(0x007020)},
                                                          {tk::name::class_, SM_BOLD(0x0e84b5)},
                                                          {tk::name::constant, SM_FG(0x60add5)},
                                                          {tk::name::decorator, SM_BOLD(0x555555)},
                                                          {tk::name::entity, SM_BOLD(0xd55537)},
                                                          {tk::name::exception, SM_FG(0x007020)},
                                                          {tk::name::function, SM_FG(0x06287e)},
                                                          {tk::name::label, SM_BOLD(0xbb60d5)},
                                                          {tk::name::namespace_, SM_BOLD(0x0e84b5)},
                                                          {tk::name::tag, SM_BOLD(0x062873)},
                                                          {tk::name::variable, SM_FG(0xbb60d5)},
                                                          {tk::name::attribute, SM_FG(0x4070a0)},
                                                          {tk::literal::string::self, SM_FG(0x4070a0)},
                                                          {tk::literal::string::doc, SM_ITALIC(0x4070a0)},
                                                          {tk::literal::string::escape, SM_BOLD(0x4070a0)},
                                                          {tk::literal::string::interpol, SM_ITALIC(0x70a0d0)},
                                                          {tk::literal::number::self, SM_FG(0x40a070)},
                                                          {tk::operator_::word, SM_BOLD(0x007020)},
                                                      }}};

    // ═══════════════════════════════════════════════════════════════════════
    // native
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<22> native = {"native",
                                                    "Native",
                                                    0x202020,
                                                    true,
                                                    0x404040,
                                                    true,
                                                    0xaaaaaa,
                                                    0,
                                                    {{
                                                        {tk::text, SM_FG(0xd0d0d0)},
                                                        {tk::error, SM_FGBG(0xd0d0d0, 0xa61717)},
                                                        {tk::comment::self, SM_ITALIC(0x999999)},
                                                        {tk::comment::preproc, SM_FG(0xcd2828)},
                                                        {tk::keyword::self, SM_BOLD(0x6ab825)},
                                                        {tk::keyword::pseudo, SM_PLAIN_NOBOLD},
                                                        {tk::keyword::type, SM_FG(0x6ab825)},
                                                        {tk::name::builtin, SM_FG(0x24909d)},
                                                        {tk::name::class_, SM_UL(0x447fcf)},
                                                        {tk::name::constant, SM_FG(0x40ffff)},
                                                        {tk::name::decorator, SM_FG(0xffa500)},
                                                        {tk::name::exception, SM_FG(0xbbbbbb)},
                                                        {tk::name::function, SM_FG(0x447fcf)},
                                                        {tk::name::namespace_, SM_UL(0x447fcf)},
                                                        {tk::name::tag, SM_BOLD(0x6ab825)},
                                                        {tk::name::variable, SM_FG(0x40ffff)},
                                                        {tk::literal::string::self, SM_FG(0xed9d13)},
                                                        {tk::literal::number::self, SM_FG(0x3677a9)},
                                                        {tk::operator_::self, SM_FG(0xd0d0d0)},
                                                        {tk::generic::deleted, SM_FG(0xd22323)},
                                                        {tk::generic::inserted, SM_FG(0x589819)},
                                                        {tk::generic::emph, SM_PLAIN_ITALIC},
                                                    }}};

    // ═══════════════════════════════════════════════════════════════════════
    // zenburn
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<20> zenburn = {"zenburn",
                                                     "Zenburn",
                                                     0x3f3f3f,
                                                     true,
                                                     0x484848,
                                                     true,
                                                     0x5d6262,
                                                     0,
                                                     {{
                                                         {tk::text, SM_FG(0xdcdccc)},
                                                         {tk::error, SM_FGBG(0xe37170, 0x3d3535)},
                                                         {tk::comment::self, SM_FG(0x7f9f7f)},
                                                         {tk::comment::preproc, SM_FG(0xdfaf8f)},
                                                         {tk::keyword::self, SM_BOLD(0xefdcbc)},
                                                         {tk::keyword::type, SM_FG(0xdfdfbf)},
                                                         {tk::name::builtin, SM_FG(0xefef8f)},
                                                         {tk::name::class_, SM_FG(0xefef8f)},
                                                         {tk::name::constant, SM_FG(0xdca3a3)},
                                                         {tk::name::exception, SM_FG(0xc3bf9f)},
                                                         {tk::name::function, SM_FG(0xefef8f)},
                                                         {tk::name::namespace_, SM_FG(0xefef8f)},
                                                         {tk::name::tag, SM_BOLD(0xe89393)},
                                                         {tk::name::variable, SM_FG(0xdcdccc)},
                                                         {tk::literal::string::self, SM_FG(0xcc9393)},
                                                         {tk::literal::number::self, SM_FG(0x8cd0d3)},
                                                         {tk::operator_::self, SM_FG(0xf0efd0)},
                                                         {tk::generic::deleted, SM_FG(0x9c6363)},
                                                         {tk::generic::inserted, SM_FG(0x709080)},
                                                         {tk::generic::emph, SM_PLAIN_ITALIC},
                                                     }}};

    // ═══════════════════════════════════════════════════════════════════════
    // bw (black & white)
    // ═══════════════════════════════════════════════════════════════════════

    inline constexpr static_style_def<10> bw = {"bw",
                                                "Black & White",
                                                0xffffff,
                                                true,
                                                0xeeeeee,
                                                true,
                                                0x666666,
                                                0,
                                                {{
                                                    {tk::comment::self, SM_PLAIN_ITALIC},
                                                    {tk::comment::preproc, SM_PLAIN_BOLD},
                                                    {tk::keyword::self, SM_PLAIN_BOLD},
                                                    {tk::keyword::type, SM_PLAIN_BOLD},
                                                    {tk::name::class_, SM_PLAIN_BOLD},
                                                    {tk::name::tag, SM_PLAIN_BOLD},
                                                    {tk::literal::string::self, SM_PLAIN_ITALIC},
                                                    {tk::operator_::word, SM_PLAIN_BOLD},
                                                    {tk::generic::heading, SM_PLAIN_BOLD},
                                                    {tk::generic::emph, SM_PLAIN_ITALIC},
                                                }}};

#undef SM_FG
#undef SM_BG
#undef SM_FGBG
#undef SM_BOLD
#undef SM_ITALIC
#undef SM_BOLDITALIC
#undef SM_UL
#undef SM_BOLDUL
#undef SM_PLAIN_BOLD
#undef SM_PLAIN_ITALIC
#undef SM_PLAIN_UL
#undef SM_PLAIN_NOBOLD

} // namespace spearmint::styles

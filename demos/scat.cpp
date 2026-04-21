/**
 * @file scat.cpp
 * @brief scat — syntax-highlighting cat command.
 *
 * Combines spearmint (tokenization + style lookup) with tapiru (terminal
 * output via ansi_emitter) to produce a colorized file viewer, similar
 * to `bat` or `pygmentize`.
 *
 * Usage:
 *   scat <file> [--style <name>] [--lines] [--plain]
 *
 * If no file is given, reads from stdin (not yet implemented).
 * Language is auto-detected from the filename extension.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

// spearmint
#include "spearmint/core/lexer.h"
#include "spearmint/core/lexer_registry.h"
#include "spearmint/core/style.h"
#include "spearmint/core/style_registry.h"
#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/lexers/bash.h"
#include "spearmint/lexers/cpp.h"
#include "spearmint/lexers/csharp.h"
#include "spearmint/lexers/css.h"
#include "spearmint/lexers/dockerfile.h"
#include "spearmint/lexers/go.h"
#include "spearmint/lexers/html.h"
#include "spearmint/lexers/ini.h"
#include "spearmint/lexers/java.h"
#include "spearmint/lexers/javascript.h"
#include "spearmint/lexers/json.h"
#include "spearmint/lexers/kotlin.h"
#include "spearmint/lexers/lua.h"
#include "spearmint/lexers/makefile.h"
#include "spearmint/lexers/markdown.h"
#include "spearmint/lexers/php.h"
#include "spearmint/lexers/python.h"
#include "spearmint/lexers/ruby.h"
#include "spearmint/lexers/rust.h"
#include "spearmint/lexers/sql.h"
#include "spearmint/lexers/swift.h"
#include "spearmint/lexers/toml.h"
#include "spearmint/lexers/typescript.h"
#include "spearmint/lexers/xml.h"
#include "spearmint/lexers/yaml.h"
#include "spearmint/styles/builtin.h"

// tapiru
#include "tapiru/core/ansi.h"
#include "tapiru/core/console.h"
#include "tapiru/core/style.h"
#include "tapiru/core/terminal.h"

// ── Bridge: spearmint style_rule → tapiru style ─────────────────────────

static tapiru::style to_tapiru_style(const spearmint::style_rule &rule) {
    tapiru::style s;
    if (rule.has_fg) {
        s.fg =
            tapiru::color::from_rgb(static_cast<uint8_t>((rule.fg >> 16) & 0xFF),
                                    static_cast<uint8_t>((rule.fg >> 8) & 0xFF), static_cast<uint8_t>(rule.fg & 0xFF));
    }
    if (rule.has_bg) {
        s.bg =
            tapiru::color::from_rgb(static_cast<uint8_t>((rule.bg >> 16) & 0xFF),
                                    static_cast<uint8_t>((rule.bg >> 8) & 0xFF), static_cast<uint8_t>(rule.bg & 0xFF));
    }
    if (rule.bold) {
        s.attrs |= tapiru::attr::bold;
    }
    if (rule.italic) {
        s.attrs |= tapiru::attr::italic;
    }
    if (rule.underline) {
        s.attrs |= tapiru::attr::underline;
    }
    if (rule.strike) {
        s.attrs |= tapiru::attr::strike;
    }
    return s;
}

// ── Helpers ─────────────────────────────────────────────────────────────

static void register_all_lexers() {
    spearmint::lexers::register_python_lexer();
    spearmint::lexers::register_cpp_lexer();
    spearmint::lexers::register_json_lexer();
    spearmint::lexers::register_javascript_lexer();
    spearmint::lexers::register_rust_lexer();
    spearmint::lexers::register_go_lexer();
    spearmint::lexers::register_typescript_lexer();
    spearmint::lexers::register_java_lexer();
    spearmint::lexers::register_csharp_lexer();
    spearmint::lexers::register_ruby_lexer();
    spearmint::lexers::register_css_lexer();
    spearmint::lexers::register_html_lexer();
    spearmint::lexers::register_yaml_lexer();
    spearmint::lexers::register_toml_lexer();
    spearmint::lexers::register_bash_lexer();
    spearmint::lexers::register_sql_lexer();
    spearmint::lexers::register_xml_lexer();
    spearmint::lexers::register_markdown_lexer();
    spearmint::lexers::register_lua_lexer();
    spearmint::lexers::register_php_lexer();
    spearmint::lexers::register_kotlin_lexer();
    spearmint::lexers::register_swift_lexer();
    spearmint::lexers::register_dockerfile_lexer();
    spearmint::lexers::register_makefile_lexer();
    spearmint::lexers::register_ini_lexer();
}

static std::string read_file(const char *path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        return {};
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static std::string basename(const char *path) {
    std::string_view sv(path);
    auto pos = sv.find_last_of("/\\");
    if (pos != std::string_view::npos) {
        sv = sv.substr(pos + 1);
    }
    return std::string(sv);
}

static void print_usage(const char *prog) {
    std::fprintf(stderr,
                 "Usage: %s <file> [options]\n"
                 "\n"
                 "Options:\n"
                 "  --style <name>   Style name (default: monokai)\n"
                 "  --lines          Show line numbers\n"
                 "  --plain          No colors (plain text)\n"
                 "  --list-styles    List available styles\n"
                 "  --list-langs     List available languages\n"
                 "  --help           Show this help\n",
                 prog);
}

// ── Main ────────────────────────────────────────────────────────────────

int main(int argc, char *argv[]) {
    const char *filepath = nullptr;
    const char *style_name = "monokai";
    bool show_lines = false;
    bool plain = false;
    bool list_styles = false;
    bool list_langs = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--style") == 0 && i + 1 < argc) {
            style_name = argv[++i];
        } else if (std::strcmp(argv[i], "--lines") == 0) {
            show_lines = true;
        } else if (std::strcmp(argv[i], "--plain") == 0) {
            plain = true;
        } else if (std::strcmp(argv[i], "--list-styles") == 0) {
            list_styles = true;
        } else if (std::strcmp(argv[i], "--list-langs") == 0) {
            list_langs = true;
        } else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (argv[i][0] != '-') {
            filepath = argv[i];
        } else {
            std::fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    register_all_lexers();

    // ── List modes ──────────────────────────────────────────────────────
    if (list_styles) {
        tapiru::console con;
        con.print("[bold cyan]Available styles:[/]");
        auto all = spearmint::get_all_styles();
        for (const auto &name : all) {
            auto *sty = spearmint::get_style(name);
            std::string line = "  [dim]\u2022[/] ";
            if (sty) {
                line += sty->display_name;
                line += " [dim](";
                line += sty->name;
                line += ")[/]";
            } else {
                line += name;
            }
            con.print(line);
        }
        return 0;
    }

    if (list_langs) {
        tapiru::console con;
        con.print("[bold cyan]Available languages:[/]");
        auto all = spearmint::get_all_lexers();
        for (const auto *info : all) {
            std::string line = "  [dim]\u2022[/] [bold]";
            line += info->display_name;
            line += "[/] [dim](";
            line += info->name;
            line += ")[/]";
            con.print(line);
        }
        return 0;
    }

    // ── Require a file ──────────────────────────────────────────────────
    if (!filepath) {
        std::fprintf(stderr, "Error: no file specified.\n");
        print_usage(argv[0]);
        return 1;
    }

    // ── Read file ───────────────────────────────────────────────────────
    std::string source = read_file(filepath);
    if (source.empty()) {
        std::fprintf(stderr, "Error: cannot read '%s'\n", filepath);
        return 1;
    }

    // ── Find lexer ──────────────────────────────────────────────────────
    auto fname = basename(filepath);
    auto lexer = spearmint::get_lexer_by_filename(fname);
    if (!lexer) {
        // Fallback: try guessing
        lexer = spearmint::guess_lexer(source);
    }

    // ── Get style ───────────────────────────────────────────────────────
    auto style_ptr = spearmint::get_style(style_name);
    if (!style_ptr) {
        std::fprintf(stderr, "Warning: style '%s' not found, using monokai\n", style_name);
        style_ptr = spearmint::get_style("monokai");
    }

    // ── Setup tapiru console ────────────────────────────────────────────
    tapiru::console con;
    tapiru::ansi_emitter emitter;

    if (!lexer) {
        // No lexer found — just print plain text with header
        con.print(std::string("[bold yellow]scat:[/] [dim]no lexer for[/] [bold]") + fname + "[/]");
        std::fputs(source.c_str(), stdout);
        return 0;
    }

    // ── Tokenize ────────────────────────────────────────────────────────
    auto result = lexer->tokenize(source);

    // ── Print header ────────────────────────────────────────────────────
    {
        std::string header = "[bold cyan]";
        header += fname;
        header += "[/] [dim]│ ";
        header += lexer->info().display_name;
        header += " │ ";
        header += style_ptr->display_name;
        header += " │ ";
        header += std::to_string(result.size());
        header += " tokens[/]";
        con.print(header);

        // Separator line (bypass markup parser — write ANSI directly)
        auto tw = con.term_width();
        std::string sep_line = "\033[2m"; // dim
        for (uint32_t i = 0; i < (tw > 0 ? tw : 80); ++i) {
            sep_line += "\xe2\x94\x80"; // U+2500 ─
        }
        sep_line += "\033[0m\n";
        con.write(sep_line);
    }

    // ── Render tokens ───────────────────────────────────────────────────
    if (plain) {
        std::fputs(source.c_str(), stdout);
    } else {
        std::string buf;
        buf.reserve(source.size() * 2);

        int line_num = 1;
        bool at_line_start = true;

        // Background style for the whole output
        tapiru::style bg_style;
        if (style_ptr->has_background) {
            bg_style.bg = tapiru::color::from_rgb(static_cast<uint8_t>((style_ptr->background_color >> 16) & 0xFF),
                                                  static_cast<uint8_t>((style_ptr->background_color >> 8) & 0xFF),
                                                  static_cast<uint8_t>(style_ptr->background_color & 0xFF));
        }

        for (const auto &entry : result.tokens) {
            // Emit line numbers at line start
            if (show_lines && at_line_start) {
                tapiru::style ln_style;
                ln_style.fg = tapiru::color::from_rgb(0x88, 0x88, 0x88);
                ln_style.attrs = tapiru::attr::dim;
                emitter.transition(ln_style, buf);

                char ln_buf[16];
                std::snprintf(ln_buf, sizeof(ln_buf), "%4d │ ", line_num);
                buf += ln_buf;
                at_line_start = false;
            }

            // Look up style for this token
            const auto *rule = style_ptr->lookup(entry.type);
            tapiru::style token_style = bg_style;
            if (rule) {
                auto ts = to_tapiru_style(*rule);
                token_style.fg = ts.fg.is_default() ? token_style.fg : ts.fg;
                if (!ts.bg.is_default()) {
                    token_style.bg = ts.bg;
                }
                token_style.attrs = token_style.attrs | ts.attrs;
            }

            emitter.transition(token_style, buf);

            // Write text, tracking newlines for line numbers
            for (char c : entry.text) {
                buf += c;
                if (c == '\n') {
                    emitter.reset(buf);
                    at_line_start = true;
                    ++line_num;

                    if (show_lines && at_line_start) {
                        tapiru::style ln_style;
                        ln_style.fg = tapiru::color::from_rgb(0x88, 0x88, 0x88);
                        ln_style.attrs = tapiru::attr::dim;
                        emitter.transition(ln_style, buf);

                        char ln_buf[16];
                        std::snprintf(ln_buf, sizeof(ln_buf), "%4d │ ", line_num);
                        buf += ln_buf;
                        at_line_start = false;
                    }
                }
            }
        }

        emitter.reset(buf);
        std::fwrite(buf.data(), 1, buf.size(), stdout);
    }

    return 0;
}

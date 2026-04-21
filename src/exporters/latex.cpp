/**
 * @file latex.cpp
 * @brief LaTeX exporter implementation.
 */

#include "spearmint/exporters/latex.h"

#include <cstdio>

namespace spearmint::exporters {

    std::string latex_exporter::escape_latex(std::string_view text) {
        std::string out;
        out.reserve(text.size());
        for (char c : text) {
            switch (c) {
            case '\\':
                out += "\\textbackslash{}";
                break;
            case '{':
                out += "\\{";
                break;
            case '}':
                out += "\\}";
                break;
            case '$':
                out += "\\$";
                break;
            case '&':
                out += "\\&";
                break;
            case '#':
                out += "\\#";
                break;
            case '%':
                out += "\\%";
                break;
            case '_':
                out += "\\_";
                break;
            case '~':
                out += "\\textasciitilde{}";
                break;
            case '^':
                out += "\\textasciicircum{}";
                break;
            default:
                out += c;
                break;
            }
        }
        return out;
    }

    std::string latex_exporter::format(const token_stream &tokens, const style_def_view &style) const {
        std::string out;
        out.reserve(tokens.size() * 40);

        // Color definitions
        out += "% Requires: \\usepackage{xcolor}\n";
        out += "% Requires: \\usepackage{fancyvrb} (if using Verbatim)\n\n";

        if (opts_.use_fancyvrb) {
            out += "\\begin{";
            out += opts_.env_name;
            out += "}[commandchars=\\\\\\{\\}]\n";
        }

        for (const auto &entry : tokens) {
            const auto *rule = style.lookup(entry.type);

            if (rule && rule->has_fg) {
                char buf[32];
                uint8_t r = static_cast<uint8_t>((rule->fg >> 16) & 0xFF);
                uint8_t g = static_cast<uint8_t>((rule->fg >> 8) & 0xFF);
                uint8_t b = static_cast<uint8_t>(rule->fg & 0xFF);

                std::string escaped = escape_latex(entry.text);

                if (rule->bold) {
                    std::snprintf(buf, sizeof(buf), "\\textcolor[RGB]{%u,%u,%u}", r, g, b);
                    out += buf;
                    out += "{\\textbf{";
                    out += escaped;
                    out += "}}";
                } else if (rule->italic) {
                    std::snprintf(buf, sizeof(buf), "\\textcolor[RGB]{%u,%u,%u}", r, g, b);
                    out += buf;
                    out += "{\\textit{";
                    out += escaped;
                    out += "}}";
                } else {
                    std::snprintf(buf, sizeof(buf), "\\textcolor[RGB]{%u,%u,%u}", r, g, b);
                    out += buf;
                    out += "{";
                    out += escaped;
                    out += "}";
                }
            } else {
                // No color — just escape special chars
                // But preserve newlines as-is in Verbatim
                out += escape_latex(entry.text);
            }
        }

        if (opts_.use_fancyvrb) {
            out += "\\end{";
            out += opts_.env_name;
            out += "}\n";
        }

        return out;
    }

} // namespace spearmint::exporters

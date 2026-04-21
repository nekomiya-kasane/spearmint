/**
 * @file html.cpp
 * @brief HTML exporter implementation.
 */

#include "spearmint/exporters/html.h"

#include <cstdio>

namespace spearmint::exporters {

    std::string html_exporter::escape_html(std::string_view text) {
        std::string out;
        out.reserve(text.size());
        for (char c : text) {
            switch (c) {
            case '&':
                out += "&amp;";
                break;
            case '<':
                out += "&lt;";
                break;
            case '>':
                out += "&gt;";
                break;
            case '"':
                out += "&quot;";
                break;
            case '\'':
                out += "&#39;";
                break;
            default:
                out += c;
                break;
            }
        }
        return out;
    }

    std::string html_exporter::color_to_css(uint32_t color) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "#%06X", color);
        return buf;
    }

    std::string html_exporter::format(const token_stream &tokens, const style_def_view &style) const {
        std::string out;
        out.reserve(tokens.size() * 40);

        if (opts_.full_page) {
            out += "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n";
            out += "<title>";
            out += escape_html(opts_.title);
            out += "</title>\n</head>\n<body>\n";
        }

        out += "<pre class=\"";
        out += opts_.css_class;
        out += "\"";
        if (style.has_background) {
            out += " style=\"background-color:";
            out += color_to_css(style.background_color);
            out += ";padding:8px\"";
        }
        out += "><code>";

        for (const auto &entry : tokens) {
            const auto *rule = style.lookup(entry.type);

            bool has_style = rule && (rule->has_fg || rule->bold || rule->italic || rule->underline);

            if (has_style) {
                out += "<span style=\"";
                if (rule->has_fg) {
                    out += "color:";
                    out += color_to_css(rule->fg);
                    out += ";";
                }
                if (rule->bold) {
                    out += "font-weight:bold;";
                }
                if (rule->italic) {
                    out += "font-style:italic;";
                }
                if (rule->underline) {
                    out += "text-decoration:underline;";
                }
                out += "\">";
            }

            out += escape_html(entry.text);

            if (has_style) {
                out += "</span>";
            }
        }

        out += "</code></pre>";

        if (opts_.full_page) {
            out += "\n</body>\n</html>\n";
        }

        return out;
    }

} // namespace spearmint::exporters

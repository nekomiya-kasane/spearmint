/**
 * @file svg.cpp
 * @brief SVG exporter implementation.
 */

#include "spearmint/exporters/svg.h"

#include <cstdio>

namespace spearmint::exporters {

std::string svg_exporter::escape_xml(std::string_view text) {
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
            out += "&apos;";
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

std::string svg_exporter::color_to_hex(uint32_t color) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "#%06X", color);
    return buf;
}

std::string svg_exporter::format(const token_stream &tokens, const style_def_view &style) const {
    // First pass: compute dimensions
    int max_cols = 0;
    int lines = 1;
    int col = 0;
    for (const auto &entry : tokens) {
        for (char c : entry.text) {
            if (c == '\n') {
                if (col > max_cols) max_cols = col;
                col = 0;
                ++lines;
            } else if (c == '\t') {
                col += 4;
            } else {
                ++col;
            }
        }
    }
    if (col > max_cols) max_cols = col;

    int char_width = static_cast<int>(opts_.font_size * 0.6);
    int width = max_cols * char_width + opts_.padding * 2;
    int height = lines * opts_.line_height + opts_.padding * 2;

    std::string out;
    out.reserve(tokens.size() * 60);

    char buf[256];
    std::snprintf(buf, sizeof(buf), "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\">\n", width,
                  height);
    out += buf;

    // Background
    if (style.has_background) {
        std::snprintf(buf, sizeof(buf), "<rect width=\"100%%\" height=\"100%%\" fill=\"%s\"/>\n",
                      color_to_hex(style.background_color).c_str());
        out += buf;
    }

    // Text
    int x = opts_.padding;
    int y = opts_.padding + opts_.font_size;

    for (const auto &entry : tokens) {
        const auto *rule = style.lookup(entry.type);

        // Split text by newlines
        std::size_t start = 0;
        for (std::size_t i = 0; i <= entry.text.size(); ++i) {
            if (i == entry.text.size() || entry.text[i] == '\n') {
                if (i > start) {
                    auto segment = entry.text.substr(start, i - start);
                    out += "<text x=\"";
                    std::snprintf(buf, sizeof(buf), "%d", x);
                    out += buf;
                    out += "\" y=\"";
                    std::snprintf(buf, sizeof(buf), "%d", y);
                    out += buf;
                    out += "\" font-family=\"";
                    out += opts_.font_family;
                    out += "\" font-size=\"";
                    std::snprintf(buf, sizeof(buf), "%d", opts_.font_size);
                    out += buf;
                    out += "\"";

                    if (rule && rule->has_fg) {
                        out += " fill=\"";
                        out += color_to_hex(rule->fg);
                        out += "\"";
                    }
                    if (rule && rule->bold) {
                        out += " font-weight=\"bold\"";
                    }
                    if (rule && rule->italic) {
                        out += " font-style=\"italic\"";
                    }

                    out += ">";
                    out += escape_xml(segment);
                    out += "</text>\n";

                    x += static_cast<int>(segment.size()) * char_width;
                }

                if (i < entry.text.size() && entry.text[i] == '\n') {
                    x = opts_.padding;
                    y += opts_.line_height;
                }
                start = i + 1;
            }
        }
    }

    out += "</svg>\n";
    return out;
}

} // namespace spearmint::exporters

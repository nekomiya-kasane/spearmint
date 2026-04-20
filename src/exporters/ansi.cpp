/**
 * @file ansi.cpp
 * @brief ANSI terminal exporter implementation.
 */

#include "spearmint/exporters/ansi.h"

#include <cstdio>

namespace spearmint::exporters {

std::string ansi_exporter::format(
    const token_stream& tokens,
    const style_def_view& style) const
{
    std::string out;
    out.reserve(tokens.size() * 20);

    for (const auto& entry : tokens) {
        const auto* rule = style.lookup(entry.type);

        bool need_reset = false;

        if (rule) {
            std::string sgr;

            if (rule->has_fg && opts_.true_color) {
                uint8_t r = static_cast<uint8_t>((rule->fg >> 16) & 0xFF);
                uint8_t g = static_cast<uint8_t>((rule->fg >> 8) & 0xFF);
                uint8_t b = static_cast<uint8_t>(rule->fg & 0xFF);
                char buf[24];
                std::snprintf(buf, sizeof(buf), "38;2;%u;%u;%u", r, g, b);
                sgr += buf;
            }

            if (rule->bold && opts_.bold) {
                if (!sgr.empty()) sgr += ";";
                sgr += "1";
            }
            if (rule->italic && opts_.italic) {
                if (!sgr.empty()) sgr += ";";
                sgr += "3";
            }
            if (rule->underline && opts_.underline) {
                if (!sgr.empty()) sgr += ";";
                sgr += "4";
            }

            if (!sgr.empty()) {
                out += "\033[";
                out += sgr;
                out += "m";
                need_reset = true;
            }
        }

        out.append(entry.text);

        if (need_reset) {
            out += "\033[0m";
        }
    }

    return out;
}

}  // namespace spearmint::exporters

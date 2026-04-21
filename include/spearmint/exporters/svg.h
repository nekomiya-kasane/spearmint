#pragma once

/**
 * @file svg.h
 * @brief SVG exporter — produces an SVG image of highlighted code.
 */

#include "spearmint/exporters/exporter.h"

namespace spearmint::exporters {

    struct svg_options {
        int font_size = 14;
        int line_height = 20;
        int padding = 10;
        const char *font_family = "monospace";
    };

    class SPEARMINT_API svg_exporter : public exporter {
      public:
        explicit svg_exporter(svg_options opts = {}) : opts_(std::move(opts)) {}

        [[nodiscard]] std::string format(const token_stream &tokens, const style_def_view &style) const override;

      private:
        svg_options opts_;

        static std::string escape_xml(std::string_view text);
        static std::string color_to_hex(uint32_t color);
    };

} // namespace spearmint::exporters

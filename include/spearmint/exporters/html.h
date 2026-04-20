#pragma once

/**
 * @file html.h
 * @brief HTML exporter — produces <pre><code> with inline CSS styles.
 */

#include "spearmint/exporters/exporter.h"

namespace spearmint::exporters {

struct html_options {
    bool full_page = false; ///< wrap in <!DOCTYPE html>
    bool line_numbers = false;
    const char *css_class = "highlight";
    const char *title = "Highlighted Code";
};

class SPEARMINT_API html_exporter : public exporter {
  public:
    explicit html_exporter(html_options opts = {}) : opts_(std::move(opts)) {}

    [[nodiscard]] std::string format(const token_stream &tokens, const style_def_view &style) const override;

  private:
    html_options opts_;

    static std::string escape_html(std::string_view text);
    static std::string color_to_css(uint32_t color);
};

} // namespace spearmint::exporters

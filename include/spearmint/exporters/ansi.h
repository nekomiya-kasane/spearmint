#pragma once

/**
 * @file ansi.h
 * @brief ANSI terminal exporter — produces text with SGR escape sequences.
 */

#include "spearmint/exporters/exporter.h"

namespace spearmint::exporters {

struct ansi_options {
    bool true_color = true; ///< use 24-bit SGR (38;2;r;g;b)
    bool bold = true;
    bool italic = true;
    bool underline = true;
};

class SPEARMINT_API ansi_exporter : public exporter {
  public:
    explicit ansi_exporter(ansi_options opts = {}) : opts_(std::move(opts)) {}

    [[nodiscard]] std::string format(const token_stream &tokens, const style_def_view &style) const override;

  private:
    ansi_options opts_;
};

} // namespace spearmint::exporters

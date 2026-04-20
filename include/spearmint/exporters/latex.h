#pragma once

/**
 * @file latex.h
 * @brief LaTeX exporter — produces \textcolor{} commands for use in LaTeX documents.
 */

#include "spearmint/exporters/exporter.h"

namespace spearmint::exporters {

struct latex_options {
    bool use_fancyvrb = true;        ///< wrap in Verbatim environment
    const char* env_name = "Verbatim";
};

class SPEARMINT_API latex_exporter : public exporter {
public:
    explicit latex_exporter(latex_options opts = {}) : opts_(std::move(opts)) {}

    [[nodiscard]] std::string format(
        const token_stream& tokens,
        const style_def_view& style) const override;

private:
    latex_options opts_;

    static std::string escape_latex(std::string_view text);
};

}  // namespace spearmint::exporters

#pragma once

/**
 * @file exporter.h
 * @brief Abstract exporter base class. Exporters convert token_stream + style → formatted output.
 */

#include "spearmint/core/style.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/exports.h"

#include <string>
#include <string_view>

namespace spearmint {

class SPEARMINT_API exporter {
  public:
    virtual ~exporter() = default;

    /**
     * @brief Export a token stream with the given style to a formatted string.
     */
    [[nodiscard]] virtual std::string format(const token_stream &tokens, const style_def_view &style) const = 0;
};

} // namespace spearmint

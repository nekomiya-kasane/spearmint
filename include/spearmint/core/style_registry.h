#pragma once

/**
 * @file style_registry.h
 * @brief Runtime style registry for looking up styles by name.
 */

#include "spearmint/core/style.h"
#include "spearmint/exports.h"

#include <string_view>
#include <vector>

namespace spearmint {

SPEARMINT_API const style_def_view *get_style(std::string_view name);
SPEARMINT_API std::vector<std::string_view> get_all_styles();
SPEARMINT_API void register_style(style_def_view def);

} // namespace spearmint

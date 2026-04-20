#pragma once

/**
 * @file spearmint.h
 * @brief Umbrella header for the spearmint semantic tokenization library.
 */

#include "spearmint/exports.h"
#include "spearmint/core/token.h"
#include "spearmint/core/token_stream.h"
#include "spearmint/core/style.h"
#include "spearmint/core/style_registry.h"
#include "spearmint/core/lexer.h"
#include "spearmint/core/regex_lexer.h"
#include "spearmint/core/lexer_registry.h"
#include "spearmint/detail/constexpr_map.h"
#include "spearmint/detail/string_utils.h"
#include "spearmint/styles/builtin.h"
#include "spearmint/lexers/python.h"
#include "spearmint/lexers/cpp.h"
#include "spearmint/lexers/json.h"
#include "spearmint/lexers/javascript.h"
#include "spearmint/exporters/exporter.h"
#include "spearmint/exporters/html.h"
#include "spearmint/exporters/ansi.h"
#include "spearmint/exporters/svg.h"
#include "spearmint/exporters/latex.h"
#include "spearmint/filters/filter.h"
#include "spearmint/filters/builtin_filters.h"

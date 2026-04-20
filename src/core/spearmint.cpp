/**
 * @file spearmint.cpp
 * @brief Library entry point — ensures at least one translation unit exists.
 */

#include "spearmint/spearmint.h"

namespace spearmint {

SPEARMINT_API const char* version() noexcept {
    return "0.1.0";
}

}  // namespace spearmint

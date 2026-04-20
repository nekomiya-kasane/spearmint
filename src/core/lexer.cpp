/**
 * @file lexer.cpp
 * @brief lexer::preprocess() implementation.
 */

#include "spearmint/core/lexer.h"

namespace spearmint {

std::string lexer::preprocess(std::string_view source) const {
    std::string result(source);

    // Tab expansion
    if (options_.tab_size > 0) {
        std::string expanded;
        expanded.reserve(result.size());
        for (char c : result) {
            if (c == '\t') {
                int spaces = options_.tab_size - static_cast<int>(expanded.size()) % options_.tab_size;
                expanded.append(static_cast<std::size_t>(spaces), ' ');
            } else {
                expanded.push_back(c);
            }
        }
        result = std::move(expanded);
    }

    // Strip all whitespace
    if (options_.strip_all) {
        // Remove leading/trailing whitespace from each line
        std::string stripped;
        stripped.reserve(result.size());
        std::size_t i = 0;
        while (i < result.size()) {
            // Skip leading whitespace on line
            while (i < result.size() && result[i] != '\n' &&
                   (result[i] == ' ' || result[i] == '\t' || result[i] == '\r'))
                ++i;
            // Copy line content
            while (i < result.size() && result[i] != '\n') {
                stripped.push_back(result[i]);
                ++i;
            }
            // Trim trailing whitespace from what we just added
            while (!stripped.empty() && stripped.back() != '\n' &&
                   (stripped.back() == ' ' || stripped.back() == '\t' || stripped.back() == '\r'))
                stripped.pop_back();
            // Copy newline
            if (i < result.size() && result[i] == '\n') {
                stripped.push_back('\n');
                ++i;
            }
        }
        result = std::move(stripped);
    } else if (options_.strip_nl) {
        // Strip leading/trailing blank lines
        std::size_t start = 0;
        while (start < result.size() && (result[start] == '\n' || result[start] == '\r')) ++start;
        std::size_t end = result.size();
        while (end > start && (result[end - 1] == '\n' || result[end - 1] == '\r')) --end;
        result = result.substr(start, end - start);
    }

    // Ensure trailing newline
    if (options_.ensure_nl && !result.empty() && result.back() != '\n') {
        result.push_back('\n');
    }

    return result;
}

} // namespace spearmint

/**
 * @file builtin_filters.cpp
 * @brief Built-in token stream filter implementations.
 */

#include "spearmint/filters/builtin_filters.h"

#include <algorithm>

namespace spearmint::filters {

SPEARMINT_API token_filter merge_consecutive() {
    return [](const token_stream &ts) -> token_stream {
        if (ts.empty()) {
            return {};
        }
        token_stream out;
        out.reserve(ts.size());
        out.push_back(ts[0]);
        for (std::size_t i = 1; i < ts.size(); ++i) {
            if (ts[i].type == out.back().type) {
                // Merge: extend the view to cover both
                auto &prev = out.back();
                const char *prev_end = prev.text.data() + prev.text.size();
                const char *cur_start = ts[i].text.data();
                if (prev_end == cur_start) {
                    // Contiguous in memory — extend the view
                    prev.text = std::string_view(prev.text.data(), prev.text.size() + ts[i].text.size());
                } else {
                    // Not contiguous — can't merge views, keep separate
                    out.push_back(ts[i]);
                }
            } else {
                out.push_back(ts[i]);
            }
        }
        return out;
    };
}

SPEARMINT_API token_filter strip_whitespace() {
    return [](const token_stream &ts) -> token_stream {
        token_stream out;
        out.reserve(ts.size());
        for (const auto &e : ts) {
            if (e.type == token::whitespace) {
                continue;
            }
            // Also skip if text is all whitespace
            bool all_ws = true;
            for (char c : e.text) {
                if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
                    all_ws = false;
                    break;
                }
            }
            if (!all_ws) {
                out.push_back(e);
            }
        }
        return out;
    };
}

SPEARMINT_API token_filter remove_token(token_type type) {
    return [type](const token_stream &ts) -> token_stream {
        token_stream out;
        out.reserve(ts.size());
        for (const auto &e : ts) {
            if (!(e.type == type)) {
                out.push_back(e);
            }
        }
        return out;
    };
}

SPEARMINT_API token_filter keep_only(std::vector<token_type> types) {
    return [types = std::move(types)](const token_stream &ts) -> token_stream {
        token_stream out;
        out.reserve(ts.size());
        for (const auto &e : ts) {
            for (const auto &t : types) {
                if (e.type == t) {
                    out.push_back(e);
                    break;
                }
            }
        }
        return out;
    };
}

SPEARMINT_API token_filter remap_token(token_type from, token_type to) {
    return [from, to](const token_stream &ts) -> token_stream {
        token_stream out;
        out.reserve(ts.size());
        for (const auto &e : ts) {
            if (e.type == from) {
                out.push_back({to, e.text});
            } else {
                out.push_back(e);
            }
        }
        return out;
    };
}

SPEARMINT_API token_filter remap_if(std::function<bool(const token_entry &)> pred, token_type new_type) {
    return [pred = std::move(pred), new_type](const token_stream &ts) -> token_stream {
        token_stream out;
        out.reserve(ts.size());
        for (const auto &e : ts) {
            if (pred(e)) {
                out.push_back({new_type, e.text});
            } else {
                out.push_back(e);
            }
        }
        return out;
    };
}

SPEARMINT_API token_filter trim() {
    return [](const token_stream &ts) -> token_stream {
        if (ts.empty()) {
            return {};
        }

        std::size_t start = 0;
        while (start < ts.size() && ts[start].type == token::whitespace) {
            ++start;
        }

        std::size_t end = ts.size();
        while (end > start && ts[end - 1].type == token::whitespace) {
            --end;
        }

        return token_stream(ts.begin() + static_cast<std::ptrdiff_t>(start),
                            ts.begin() + static_cast<std::ptrdiff_t>(end));
    };
}

SPEARMINT_API token_filter normalize_whitespace() {
    return [](const token_stream &ts) -> token_stream {
        token_stream out;
        out.reserve(ts.size());
        bool prev_ws = false;
        for (const auto &e : ts) {
            if (e.type == token::whitespace) {
                if (!prev_ws) {
                    out.push_back({token::whitespace, " "});
                    prev_ws = true;
                }
            } else {
                out.push_back(e);
                prev_ws = false;
            }
        }
        return out;
    };
}

} // namespace spearmint::filters

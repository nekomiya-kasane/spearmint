/**
 * @file style_registry.cpp
 * @brief Runtime style registry implementation + builtin style registration.
 */

#include "spearmint/core/style_registry.h"
#include "spearmint/styles/builtin.h"

#include <algorithm>
#include <mutex>

namespace spearmint {

// ── style_def_view::lookup ─────────────────────────────────────────────

const style_rule* style_def_view::lookup(token_type t) const noexcept {
    for (int depth = 0; depth < 8; ++depth) {
        for (const auto& e : entries) {
            if (e.token == t) {
                return &e.rule;
            }
        }
        if (t.id >= detail::token_count) return nullptr;
        uint32_t pid = detail::token_table[t.id].parent_id;
        if (pid == t.id) return nullptr;
        t = detail::token_table[pid];
    }
    return nullptr;
}

// ── Registry singleton ─────────────────────────────────────────────────

namespace {

struct registry {
    std::vector<style_def_view> styles;
    std::mutex mtx;
    bool initialized = false;

    void ensure_builtins() {
        if (initialized) return;
        std::lock_guard lock(mtx);
        if (initialized) return;

        styles.push_back(style_def_view(styles::default_));
        styles.push_back(style_def_view(styles::monokai));
        styles.push_back(style_def_view(styles::dracula));
        styles.push_back(style_def_view(styles::nord));
        styles.push_back(style_def_view(styles::one_dark));
        styles.push_back(style_def_view(styles::solarized_dark));
        styles.push_back(style_def_view(styles::solarized_light));
        styles.push_back(style_def_view(styles::gruvbox_dark));
        styles.push_back(style_def_view(styles::vim));
        styles.push_back(style_def_view(styles::github_dark));
        styles.push_back(style_def_view(styles::emacs));
        styles.push_back(style_def_view(styles::friendly));
        styles.push_back(style_def_view(styles::native));
        styles.push_back(style_def_view(styles::zenburn));
        styles.push_back(style_def_view(styles::bw));

        initialized = true;
    }
};

registry& get_registry() {
    static registry r;
    r.ensure_builtins();
    return r;
}

}  // namespace

// ── Public API ─────────────────────────────────────────────────────────

SPEARMINT_API const style_def_view* get_style(std::string_view name) {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);
    for (const auto& s : reg.styles) {
        if (s.name == name) return &s;
    }
    return nullptr;
}

SPEARMINT_API std::vector<std::string_view> get_all_styles() {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);
    std::vector<std::string_view> names;
    names.reserve(reg.styles.size());
    for (const auto& s : reg.styles) {
        names.push_back(s.name);
    }
    return names;
}

SPEARMINT_API void register_style(style_def_view def) {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);
    reg.styles.push_back(def);
}

}  // namespace spearmint

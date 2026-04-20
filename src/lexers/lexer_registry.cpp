/**
 * @file lexer_registry.cpp
 * @brief Runtime lexer registry implementation.
 */

#include "spearmint/core/lexer_registry.h"

#include <algorithm>
#include <mutex>

namespace spearmint {

namespace {

struct registry_entry {
    lexer_factory factory;
    const lexer_info* info;
};

struct registry {
    std::vector<registry_entry> entries;
    std::mutex mtx;
};

registry& get_registry() {
    static registry r;
    return r;
}

bool glob_match(std::string_view pattern, std::string_view str) {
    // Simple glob: only supports * at start (e.g. "*.py")
    if (pattern.starts_with("*.")) {
        auto ext = pattern.substr(1);  // ".py"
        return str.size() >= ext.size() &&
               str.substr(str.size() - ext.size()) == ext;
    }
    return pattern == str;
}

}  // namespace

SPEARMINT_API void register_lexer(lexer_factory factory, const lexer_info& info) {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);
    reg.entries.push_back({std::move(factory), &info});
}

SPEARMINT_API std::unique_ptr<lexer> get_lexer_by_name(std::string_view name) {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);
    for (const auto& e : reg.entries) {
        if (name == e.info->name) return e.factory();
        for (const auto& alias : e.info->aliases) {
            if (name == alias) return e.factory();
        }
    }
    return nullptr;
}

SPEARMINT_API std::unique_ptr<lexer> get_lexer_by_filename(std::string_view filename) {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);

    const registry_entry* best = nullptr;
    int best_priority = -1;

    for (const auto& e : reg.entries) {
        for (const auto& pat : e.info->filenames) {
            if (glob_match(pat, filename)) {
                if (e.info->priority > best_priority) {
                    best = &e;
                    best_priority = e.info->priority;
                }
            }
        }
    }
    return best ? best->factory() : nullptr;
}

SPEARMINT_API std::unique_ptr<lexer> get_lexer_by_mime(std::string_view mime) {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);
    for (const auto& e : reg.entries) {
        for (const auto& m : e.info->mime_types) {
            if (mime == m) return e.factory();
        }
    }
    return nullptr;
}

SPEARMINT_API std::unique_ptr<lexer> guess_lexer(std::string_view source) {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);

    const registry_entry* best = nullptr;
    float best_score = 0.0f;

    for (const auto& e : reg.entries) {
        auto lex = e.factory();
        float score = lex->analyse_text(source);
        if (score > best_score) {
            best_score = score;
            best = &e;
        }
    }
    return (best && best_score > 0.0f) ? best->factory() : nullptr;
}

SPEARMINT_API std::vector<const lexer_info*> get_all_lexers() {
    auto& reg = get_registry();
    std::lock_guard lock(reg.mtx);
    std::vector<const lexer_info*> result;
    result.reserve(reg.entries.size());
    for (const auto& e : reg.entries) {
        result.push_back(e.info);
    }
    return result;
}

}  // namespace spearmint

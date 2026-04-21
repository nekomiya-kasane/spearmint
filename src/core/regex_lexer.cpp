/**
 * @file regex_lexer.cpp
 * @brief regex_lexer tokenization engine.
 */

#include "spearmint/core/regex_lexer.h"

#include <mutex>

namespace spearmint {

    void regex_lexer::ensure_compiled() const {
        if (compiled_ready_) {
            return;
        }

        // Thread safety for lazy compilation
        static std::mutex compile_mtx;
        std::lock_guard lock(compile_mtx);
        if (compiled_ready_) {
            return;
        }

        auto rules = get_rules();
        for (auto &[state_name, rlist] : rules) {
            detail::compiled_state cs;
            cs.reserve(rlist.size());
            for (auto &r : rlist) {
                detail::compiled_rule cr;
                cr.re = std::regex(r.pattern, std::regex::ECMAScript | std::regex::optimize);
                cr.token = r.token;
                cr.action = std::move(r.action);
                cr.group_tokens = std::move(r.group_tokens);
                cs.push_back(std::move(cr));
            }
            compiled_[state_name] = std::move(cs);
        }
        compiled_ready_ = true;
    }

    tokenize_result regex_lexer::tokenize(std::string_view source) const {
        ensure_compiled();

        tokenize_result out;
        out.source = preprocess(source);
        std::string_view src = out.source;

        out.tokens.reserve(src.size() / 4); // heuristic

        // State stack
        std::vector<std::string> state_stack;
        state_stack.push_back("root");

        std::size_t pos = 0;

        while (pos < src.size()) {
            const auto &current_state = state_stack.back();
            auto it = compiled_.find(current_state);
            if (it == compiled_.end()) {
                break;
            }

            const auto &rules = it->second;
            bool matched = false;

            for (const auto &rule : rules) {
                std::cmatch m;
                const char *start = src.data() + pos;
                std::size_t remaining = src.size() - pos;

                if (std::regex_search(start, start + remaining, m, rule.re, std::regex_constants::match_continuous)) {
                    if (m.length(0) == 0) {
                        continue; // skip zero-length matches
                    }

                    if (!rule.group_tokens.empty()) {
                        // Multi-group: each capture group → different token
                        for (std::size_t g = 0; g < rule.group_tokens.size(); ++g) {
                            if (m[g + 1].matched && m[g + 1].length() > 0) {
                                std::size_t gstart = static_cast<std::size_t>(m[g + 1].first - src.data());
                                std::size_t glen = static_cast<std::size_t>(m[g + 1].length());
                                out.tokens.push_back({rule.group_tokens[g], src.substr(gstart, glen)});
                            }
                        }
                    } else {
                        // Single token for entire match
                        out.tokens.push_back({rule.token, src.substr(pos, static_cast<std::size_t>(m.length(0)))});
                    }

                    pos += static_cast<std::size_t>(m.length(0));

                    // State transition
                    switch (rule.action.type) {
                    case state_action::stay:
                        break;
                    case state_action::push:
                        state_stack.push_back(rule.action.target);
                        break;
                    case state_action::pop:
                        if (state_stack.size() > 1) {
                            state_stack.pop_back();
                        }
                        break;
                    case state_action::push_pop:
                        if (state_stack.size() > 1) {
                            state_stack.pop_back();
                        }
                        state_stack.push_back(rule.action.target);
                        break;
                    }

                    matched = true;
                    break;
                }
            }

            if (!matched) {
                // No rule matched — emit single char as error token and advance
                out.tokens.push_back({token::error, src.substr(pos, 1)});
                ++pos;
            }
        }

        return out;
    }

} // namespace spearmint

#pragma once

/**
 * @file regex_lexer.h
 * @brief Regex-based lexer with state machine, modeled after Pygments' RegexLexer.
 *
 * Subclasses define `get_rules()` returning a map of state→rule_list.
 * Each rule is a (pattern, token_type, next_state) triple.
 * The engine tries rules in order, emits tokens, and transitions states.
 */

#include "spearmint/core/lexer.h"

#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace spearmint {

    // ── Rule actions ───────────────────────────────────────────────────────

    /**
     * @brief What to do after a rule matches.
     */
    struct state_action {
        enum kind : uint8_t {
            stay,     ///< remain in current state
            push,     ///< push a new state onto the stack
            pop,      ///< pop the current state
            push_pop, ///< pop then push (replace top)
        };

        kind type = stay;
        std::string target; ///< state name for push/push_pop

        static state_action none() { return {stay, {}}; }
        static state_action push_state(std::string s) { return {push, std::move(s)}; }
        static state_action pop_state() { return {pop, {}}; }
        static state_action goto_state(std::string s) { return {push_pop, std::move(s)}; }
    };

    // ── Lexer rule ─────────────────────────────────────────────────────────

    /**
     * @brief A single tokenization rule: pattern → token + state transition.
     */
    struct lexer_rule {
        std::string pattern;
        token_type token;
        state_action action;

        /**
         * @brief Multi-group rule: each capture group maps to a different token.
         *
         * If non-empty, `token` is ignored and `group_tokens` is used instead.
         * group_tokens[0] = token for capture group 1, etc.
         */
        std::vector<token_type> group_tokens;
    };

    /**
     * @brief A named list of rules forming one lexer state.
     */
    using rule_list = std::vector<lexer_rule>;

    /**
     * @brief Complete state machine: state_name → rules.
     */
    using state_map = std::unordered_map<std::string, rule_list>;

    // ── Compiled rule (internal) ───────────────────────────────────────────

    namespace detail {

        struct compiled_rule {
            std::regex re;
            token_type token;
            state_action action;
            std::vector<token_type> group_tokens;
        };

        using compiled_state = std::vector<compiled_rule>;
        using compiled_state_map = std::unordered_map<std::string, compiled_state>;

    } // namespace detail

    // ── regex_lexer ────────────────────────────────────────────────────────

    /**
     * @brief Base class for regex-driven lexers.
     *
     * Subclasses override `get_rules()` to define the state machine.
     * The tokenizer compiles regexes once and caches them.
     */
    class SPEARMINT_API regex_lexer : public lexer {
      public:
        [[nodiscard]] tokenize_result tokenize(std::string_view source) const override;

      protected:
        /**
         * @brief Define the lexer's state machine.
         *
         * Must return at least a "root" state.
         */
        [[nodiscard]] virtual state_map get_rules() const = 0;

        /**
         * @brief Ensure rules are compiled. Called lazily on first tokenize().
         */
        void ensure_compiled() const;

      private:
        mutable detail::compiled_state_map compiled_;
        mutable bool compiled_ready_ = false;
    };

} // namespace spearmint

#pragma once

/**
 * @file token.h
 * @brief Constexpr hierarchical token type system for semantic tokenization.
 *
 * All ~80 standard token types from Pygments are defined as constexpr inline
 * globals. Token parent relationships are resolved at compile time.
 *
 * Usage:
 *   using namespace spearmint;
 *   static_assert(token::keyword::constant.is_child_of(token::keyword::self));
 *   static_assert(token::keyword::self.is_child_of(token::root));
 *   constexpr auto t = token_from_string("Token.Keyword.Constant");
 *   static_assert(t == token::keyword::constant);
 */

#include <cstdint>
#include <array>

#include "spearmint/detail/string_utils.h"

namespace spearmint {

// ── Token type ─────────────────────────────────────────────────────────

/**
 * @brief A semantic token type with compile-time parent chain.
 *
 * Each token has a unique id, a parent_id linking to its parent in the
 * hierarchy, a fully qualified name, and a short display name.
 */
struct token_type {
    uint32_t id;
    uint32_t parent_id;
    const char* name;        ///< e.g. "Token.Keyword.Constant"
    const char* short_name;  ///< e.g. "Constant"

    /**
     * @brief Check if this token is a descendant of another.
     *
     * Uses the global token table for parent traversal.
     * Defined after the table is declared.
     */
    [[nodiscard]] constexpr bool is_child_of(token_type other) const noexcept;

    [[nodiscard]] constexpr bool operator==(token_type other) const noexcept {
        return id == other.id;
    }

    [[nodiscard]] constexpr bool operator!=(token_type other) const noexcept {
        return id != other.id;
    }

    [[nodiscard]] constexpr bool operator<(token_type other) const noexcept {
        return id < other.id;
    }
};

// ── Token ID constants ─────────────────────────────────────────────────

namespace detail {

enum token_id : uint32_t {
    // Root
    id_root = 0,

    // Top-level
    id_text = 1,
    id_whitespace = 2,
    id_error = 3,
    id_other = 4,

    // Keyword
    id_keyword = 10,
    id_keyword_constant = 11,
    id_keyword_declaration = 12,
    id_keyword_namespace = 13,
    id_keyword_pseudo = 14,
    id_keyword_reserved = 15,
    id_keyword_type = 16,

    // Name
    id_name = 20,
    id_name_attribute = 21,
    id_name_builtin = 22,
    id_name_builtin_pseudo = 23,
    id_name_class = 24,
    id_name_constant = 25,
    id_name_decorator = 26,
    id_name_entity = 27,
    id_name_exception = 28,
    id_name_function = 29,
    id_name_function_magic = 30,
    id_name_label = 31,
    id_name_namespace = 32,
    id_name_other = 33,
    id_name_property = 34,
    id_name_tag = 35,
    id_name_variable = 36,
    id_name_variable_class = 37,
    id_name_variable_global = 38,
    id_name_variable_instance = 39,
    id_name_variable_magic = 40,

    // Literal
    id_literal = 50,
    id_literal_date = 51,

    // Literal.String
    id_string = 52,
    id_string_affix = 53,
    id_string_backtick = 54,
    id_string_char = 55,
    id_string_delimiter = 56,
    id_string_doc = 57,
    id_string_double = 58,
    id_string_escape = 59,
    id_string_heredoc = 60,
    id_string_interpol = 61,
    id_string_other = 62,
    id_string_regex = 63,
    id_string_single = 64,
    id_string_symbol = 65,

    // Literal.Number
    id_number = 66,
    id_number_bin = 67,
    id_number_float = 68,
    id_number_hex = 69,
    id_number_integer = 70,
    id_number_integer_long = 71,
    id_number_oct = 72,

    // Operator
    id_operator = 80,
    id_operator_word = 81,

    // Punctuation
    id_punctuation = 85,
    id_punctuation_marker = 86,

    // Comment
    id_comment = 90,
    id_comment_hashbang = 91,
    id_comment_multiline = 92,
    id_comment_preproc = 93,
    id_comment_preproc_file = 94,
    id_comment_single = 95,
    id_comment_special = 96,

    // Generic
    id_generic = 100,
    id_generic_deleted = 101,
    id_generic_emph = 102,
    id_generic_error = 103,
    id_generic_heading = 104,
    id_generic_inserted = 105,
    id_generic_output = 106,
    id_generic_prompt = 107,
    id_generic_strong = 108,
    id_generic_strong_emph = 109,
    id_generic_subheading = 110,
    id_generic_traceback = 111,

    token_count = 112,
};

}  // namespace detail

// ── Token definitions ──────────────────────────────────────────────────

namespace token {

// Root
inline constexpr token_type root {detail::id_root, detail::id_root, "Token", "Token"};

// Top-level
inline constexpr token_type text       {detail::id_text,       detail::id_root, "Token.Text", "Text"};
inline constexpr token_type whitespace {detail::id_whitespace, detail::id_text, "Token.Text.Whitespace", "Whitespace"};
inline constexpr token_type error      {detail::id_error,      detail::id_root, "Token.Error", "Error"};
inline constexpr token_type other      {detail::id_other,      detail::id_root, "Token.Other", "Other"};

// Keyword
namespace keyword {
    inline constexpr token_type self        {detail::id_keyword,             detail::id_root,    "Token.Keyword", "Keyword"};
    inline constexpr token_type constant    {detail::id_keyword_constant,    detail::id_keyword, "Token.Keyword.Constant", "Constant"};
    inline constexpr token_type declaration {detail::id_keyword_declaration, detail::id_keyword, "Token.Keyword.Declaration", "Declaration"};
    inline constexpr token_type namespace_  {detail::id_keyword_namespace,   detail::id_keyword, "Token.Keyword.Namespace", "Namespace"};
    inline constexpr token_type pseudo      {detail::id_keyword_pseudo,      detail::id_keyword, "Token.Keyword.Pseudo", "Pseudo"};
    inline constexpr token_type reserved    {detail::id_keyword_reserved,    detail::id_keyword, "Token.Keyword.Reserved", "Reserved"};
    inline constexpr token_type type        {detail::id_keyword_type,        detail::id_keyword, "Token.Keyword.Type", "Type"};
}

// Name
namespace name {
    inline constexpr token_type self            {detail::id_name,                  detail::id_root, "Token.Name", "Name"};
    inline constexpr token_type attribute       {detail::id_name_attribute,        detail::id_name, "Token.Name.Attribute", "Attribute"};
    inline constexpr token_type builtin         {detail::id_name_builtin,          detail::id_name, "Token.Name.Builtin", "Builtin"};
    inline constexpr token_type builtin_pseudo  {detail::id_name_builtin_pseudo,   detail::id_name, "Token.Name.Builtin.Pseudo", "Pseudo"};
    inline constexpr token_type class_          {detail::id_name_class,            detail::id_name, "Token.Name.Class", "Class"};
    inline constexpr token_type constant        {detail::id_name_constant,         detail::id_name, "Token.Name.Constant", "Constant"};
    inline constexpr token_type decorator       {detail::id_name_decorator,        detail::id_name, "Token.Name.Decorator", "Decorator"};
    inline constexpr token_type entity          {detail::id_name_entity,           detail::id_name, "Token.Name.Entity", "Entity"};
    inline constexpr token_type exception       {detail::id_name_exception,        detail::id_name, "Token.Name.Exception", "Exception"};
    inline constexpr token_type function        {detail::id_name_function,         detail::id_name, "Token.Name.Function", "Function"};
    inline constexpr token_type function_magic  {detail::id_name_function_magic,   detail::id_name, "Token.Name.Function.Magic", "Magic"};
    inline constexpr token_type label           {detail::id_name_label,            detail::id_name, "Token.Name.Label", "Label"};
    inline constexpr token_type namespace_      {detail::id_name_namespace,        detail::id_name, "Token.Name.Namespace", "Namespace"};
    inline constexpr token_type other           {detail::id_name_other,            detail::id_name, "Token.Name.Other", "Other"};
    inline constexpr token_type property        {detail::id_name_property,         detail::id_name, "Token.Name.Property", "Property"};
    inline constexpr token_type tag             {detail::id_name_tag,              detail::id_name, "Token.Name.Tag", "Tag"};
    inline constexpr token_type variable        {detail::id_name_variable,         detail::id_name, "Token.Name.Variable", "Variable"};
    inline constexpr token_type variable_class  {detail::id_name_variable_class,   detail::id_name_variable, "Token.Name.Variable.Class", "Class"};
    inline constexpr token_type variable_global {detail::id_name_variable_global,  detail::id_name_variable, "Token.Name.Variable.Global", "Global"};
    inline constexpr token_type variable_instance{detail::id_name_variable_instance,detail::id_name_variable, "Token.Name.Variable.Instance", "Instance"};
    inline constexpr token_type variable_magic  {detail::id_name_variable_magic,   detail::id_name_variable, "Token.Name.Variable.Magic", "Magic"};
}

// Literal
namespace literal {
    inline constexpr token_type self {detail::id_literal,      detail::id_root,    "Token.Literal", "Literal"};
    inline constexpr token_type date {detail::id_literal_date, detail::id_literal, "Token.Literal.Date", "Date"};

    // String
    namespace string {
        inline constexpr token_type self      {detail::id_string,           detail::id_literal, "Token.Literal.String", "String"};
        inline constexpr token_type affix     {detail::id_string_affix,     detail::id_string,  "Token.Literal.String.Affix", "Affix"};
        inline constexpr token_type backtick  {detail::id_string_backtick,  detail::id_string,  "Token.Literal.String.Backtick", "Backtick"};
        inline constexpr token_type char_     {detail::id_string_char,      detail::id_string,  "Token.Literal.String.Char", "Char"};
        inline constexpr token_type delimiter {detail::id_string_delimiter, detail::id_string,  "Token.Literal.String.Delimiter", "Delimiter"};
        inline constexpr token_type doc       {detail::id_string_doc,       detail::id_string,  "Token.Literal.String.Doc", "Doc"};
        inline constexpr token_type double_   {detail::id_string_double,    detail::id_string,  "Token.Literal.String.Double", "Double"};
        inline constexpr token_type escape    {detail::id_string_escape,    detail::id_string,  "Token.Literal.String.Escape", "Escape"};
        inline constexpr token_type heredoc   {detail::id_string_heredoc,   detail::id_string,  "Token.Literal.String.Heredoc", "Heredoc"};
        inline constexpr token_type interpol  {detail::id_string_interpol,  detail::id_string,  "Token.Literal.String.Interpol", "Interpol"};
        inline constexpr token_type other     {detail::id_string_other,     detail::id_string,  "Token.Literal.String.Other", "Other"};
        inline constexpr token_type regex     {detail::id_string_regex,     detail::id_string,  "Token.Literal.String.Regex", "Regex"};
        inline constexpr token_type single    {detail::id_string_single,    detail::id_string,  "Token.Literal.String.Single", "Single"};
        inline constexpr token_type symbol    {detail::id_string_symbol,    detail::id_string,  "Token.Literal.String.Symbol", "Symbol"};
    }

    // Number
    namespace number {
        inline constexpr token_type self         {detail::id_number,              detail::id_literal, "Token.Literal.Number", "Number"};
        inline constexpr token_type bin          {detail::id_number_bin,          detail::id_number,  "Token.Literal.Number.Bin", "Bin"};
        inline constexpr token_type float_       {detail::id_number_float,        detail::id_number,  "Token.Literal.Number.Float", "Float"};
        inline constexpr token_type hex          {detail::id_number_hex,          detail::id_number,  "Token.Literal.Number.Hex", "Hex"};
        inline constexpr token_type integer      {detail::id_number_integer,      detail::id_number,  "Token.Literal.Number.Integer", "Integer"};
        inline constexpr token_type integer_long {detail::id_number_integer_long, detail::id_number_integer, "Token.Literal.Number.Integer.Long", "Long"};
        inline constexpr token_type oct          {detail::id_number_oct,          detail::id_number,  "Token.Literal.Number.Oct", "Oct"};
    }
}

// Operator
namespace operator_ {
    inline constexpr token_type self {detail::id_operator,      detail::id_root,     "Token.Operator", "Operator"};
    inline constexpr token_type word {detail::id_operator_word, detail::id_operator, "Token.Operator.Word", "Word"};
}

// Punctuation
namespace punctuation {
    inline constexpr token_type self   {detail::id_punctuation,        detail::id_root,        "Token.Punctuation", "Punctuation"};
    inline constexpr token_type marker {detail::id_punctuation_marker, detail::id_punctuation, "Token.Punctuation.Marker", "Marker"};
}

// Comment
namespace comment {
    inline constexpr token_type self         {detail::id_comment,              detail::id_root,    "Token.Comment", "Comment"};
    inline constexpr token_type hashbang     {detail::id_comment_hashbang,     detail::id_comment, "Token.Comment.Hashbang", "Hashbang"};
    inline constexpr token_type multiline    {detail::id_comment_multiline,    detail::id_comment, "Token.Comment.Multiline", "Multiline"};
    inline constexpr token_type preproc      {detail::id_comment_preproc,      detail::id_comment, "Token.Comment.Preproc", "Preproc"};
    inline constexpr token_type preproc_file {detail::id_comment_preproc_file, detail::id_comment, "Token.Comment.PreprocFile", "PreprocFile"};
    inline constexpr token_type single       {detail::id_comment_single,       detail::id_comment, "Token.Comment.Single", "Single"};
    inline constexpr token_type special      {detail::id_comment_special,      detail::id_comment, "Token.Comment.Special", "Special"};
}

// Generic
namespace generic {
    inline constexpr token_type self        {detail::id_generic,             detail::id_root,    "Token.Generic", "Generic"};
    inline constexpr token_type deleted     {detail::id_generic_deleted,     detail::id_generic, "Token.Generic.Deleted", "Deleted"};
    inline constexpr token_type emph        {detail::id_generic_emph,        detail::id_generic, "Token.Generic.Emph", "Emph"};
    inline constexpr token_type error       {detail::id_generic_error,       detail::id_generic, "Token.Generic.Error", "Error"};
    inline constexpr token_type heading     {detail::id_generic_heading,     detail::id_generic, "Token.Generic.Heading", "Heading"};
    inline constexpr token_type inserted    {detail::id_generic_inserted,    detail::id_generic, "Token.Generic.Inserted", "Inserted"};
    inline constexpr token_type output      {detail::id_generic_output,      detail::id_generic, "Token.Generic.Output", "Output"};
    inline constexpr token_type prompt      {detail::id_generic_prompt,      detail::id_generic, "Token.Generic.Prompt", "Prompt"};
    inline constexpr token_type strong      {detail::id_generic_strong,      detail::id_generic, "Token.Generic.Strong", "Strong"};
    inline constexpr token_type strong_emph {detail::id_generic_strong_emph, detail::id_generic, "Token.Generic.StrongEmph", "StrongEmph"};
    inline constexpr token_type subheading  {detail::id_generic_subheading,  detail::id_generic, "Token.Generic.Subheading", "Subheading"};
    inline constexpr token_type traceback   {detail::id_generic_traceback,   detail::id_generic, "Token.Generic.Traceback", "Traceback"};
}

}  // namespace token

// ── Global token table ─────────────────────────────────────────────────

namespace detail {

/**
 * @brief Flat table of all token types, indexed by id.
 *
 * Used for parent traversal in is_child_of() and token_from_string().
 */
inline constexpr std::array<token_type, token_count> token_table = {{
    // 0-4: root + top-level
    token::root,
    token::text,
    token::whitespace,
    token::error,
    token::other,
    // 5-9: padding
    {5, 0, "", ""}, {6, 0, "", ""}, {7, 0, "", ""}, {8, 0, "", ""}, {9, 0, "", ""},
    // 10-16: keyword
    token::keyword::self,
    token::keyword::constant,
    token::keyword::declaration,
    token::keyword::namespace_,
    token::keyword::pseudo,
    token::keyword::reserved,
    token::keyword::type,
    // 17-19: padding
    {17, 0, "", ""}, {18, 0, "", ""}, {19, 0, "", ""},
    // 20-40: name
    token::name::self,
    token::name::attribute,
    token::name::builtin,
    token::name::builtin_pseudo,
    token::name::class_,
    token::name::constant,
    token::name::decorator,
    token::name::entity,
    token::name::exception,
    token::name::function,
    token::name::function_magic,
    token::name::label,
    token::name::namespace_,
    token::name::other,
    token::name::property,
    token::name::tag,
    token::name::variable,
    token::name::variable_class,
    token::name::variable_global,
    token::name::variable_instance,
    token::name::variable_magic,
    // 41-49: padding
    {41, 0, "", ""}, {42, 0, "", ""}, {43, 0, "", ""}, {44, 0, "", ""},
    {45, 0, "", ""}, {46, 0, "", ""}, {47, 0, "", ""}, {48, 0, "", ""}, {49, 0, "", ""},
    // 50-72: literal
    token::literal::self,
    token::literal::date,
    token::literal::string::self,
    token::literal::string::affix,
    token::literal::string::backtick,
    token::literal::string::char_,
    token::literal::string::delimiter,
    token::literal::string::doc,
    token::literal::string::double_,
    token::literal::string::escape,
    token::literal::string::heredoc,
    token::literal::string::interpol,
    token::literal::string::other,
    token::literal::string::regex,
    token::literal::string::single,
    token::literal::string::symbol,
    token::literal::number::self,
    token::literal::number::bin,
    token::literal::number::float_,
    token::literal::number::hex,
    token::literal::number::integer,
    token::literal::number::integer_long,
    token::literal::number::oct,
    // 73-79: padding
    {73, 0, "", ""}, {74, 0, "", ""}, {75, 0, "", ""}, {76, 0, "", ""},
    {77, 0, "", ""}, {78, 0, "", ""}, {79, 0, "", ""},
    // 80-81: operator
    token::operator_::self,
    token::operator_::word,
    // 82-84: padding
    {82, 0, "", ""}, {83, 0, "", ""}, {84, 0, "", ""},
    // 85-86: punctuation
    token::punctuation::self,
    token::punctuation::marker,
    // 87-89: padding
    {87, 0, "", ""}, {88, 0, "", ""}, {89, 0, "", ""},
    // 90-96: comment
    token::comment::self,
    token::comment::hashbang,
    token::comment::multiline,
    token::comment::preproc,
    token::comment::preproc_file,
    token::comment::single,
    token::comment::special,
    // 97-99: padding
    {97, 0, "", ""}, {98, 0, "", ""}, {99, 0, "", ""},
    // 100-111: generic
    token::generic::self,
    token::generic::deleted,
    token::generic::emph,
    token::generic::error,
    token::generic::heading,
    token::generic::inserted,
    token::generic::output,
    token::generic::prompt,
    token::generic::strong,
    token::generic::strong_emph,
    token::generic::subheading,
    token::generic::traceback,
}};

}  // namespace detail

// ── is_child_of implementation ─────────────────────────────────────────

constexpr bool token_type::is_child_of(token_type other) const noexcept {
    if (id == other.id) return true;
    uint32_t cur = id;
    for (int depth = 0; depth < 8; ++depth) {
        if (cur >= detail::token_count) return false;
        uint32_t pid = detail::token_table[cur].parent_id;
        if (pid == other.id) return true;
        if (pid == cur) return false;  // reached root
        cur = pid;
    }
    return false;
}

// ── All valid tokens list ──────────────────────────────────────────────

namespace detail {

inline constexpr uint32_t valid_token_count = 79;

inline constexpr std::array<token_type, valid_token_count> all_tokens = {{
    token::root,
    token::text, token::whitespace, token::error, token::other,
    // keyword
    token::keyword::self,
    token::keyword::constant, token::keyword::declaration, token::keyword::namespace_,
    token::keyword::pseudo, token::keyword::reserved, token::keyword::type,
    // name
    token::name::self,
    token::name::attribute, token::name::builtin, token::name::builtin_pseudo,
    token::name::class_, token::name::constant, token::name::decorator,
    token::name::entity, token::name::exception, token::name::function,
    token::name::function_magic, token::name::label, token::name::namespace_,
    token::name::other, token::name::property, token::name::tag,
    token::name::variable, token::name::variable_class, token::name::variable_global,
    token::name::variable_instance, token::name::variable_magic,
    // literal
    token::literal::self, token::literal::date,
    // string
    token::literal::string::self,
    token::literal::string::affix, token::literal::string::backtick,
    token::literal::string::char_, token::literal::string::delimiter,
    token::literal::string::doc, token::literal::string::double_,
    token::literal::string::escape, token::literal::string::heredoc,
    token::literal::string::interpol, token::literal::string::other,
    token::literal::string::regex, token::literal::string::single,
    token::literal::string::symbol,
    // number
    token::literal::number::self,
    token::literal::number::bin, token::literal::number::float_,
    token::literal::number::hex, token::literal::number::integer,
    token::literal::number::integer_long, token::literal::number::oct,
    // operator
    token::operator_::self, token::operator_::word,
    // punctuation
    token::punctuation::self, token::punctuation::marker,
    // comment
    token::comment::self,
    token::comment::hashbang, token::comment::multiline,
    token::comment::preproc, token::comment::preproc_file,
    token::comment::single, token::comment::special,
    // generic
    token::generic::self,
    token::generic::deleted, token::generic::emph, token::generic::error,
    token::generic::heading, token::generic::inserted, token::generic::output,
    token::generic::prompt, token::generic::strong, token::generic::strong_emph,
    token::generic::subheading, token::generic::traceback,
}};

}  // namespace detail

// ── token_from_string ──────────────────────────────────────────────────

/**
 * @brief Look up a token type by its fully qualified name at compile time.
 *
 * Iterates the compact all_tokens list (no padding entries) to stay
 * within clang's default constexpr step limit even at -O0.
 *
 * Returns token::root if not found.
 */
constexpr token_type token_from_string(const char* name) noexcept {
    for (uint32_t i = 0; i < detail::valid_token_count; ++i) {
        if (detail::str_eq(detail::all_tokens[i].name, name)) {
            return detail::all_tokens[i];
        }
    }
    return token::root;
}

}  // namespace spearmint

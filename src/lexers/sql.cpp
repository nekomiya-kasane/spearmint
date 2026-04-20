#include "spearmint/lexers/sql.h"

namespace spearmint::lexers {

namespace {
constexpr const char *aliases[] = {"sql"};
constexpr const char *filenames[] = {"*.sql"};
constexpr const char *mimes[] = {"text/x-sql"};
const lexer_info sql_info = {
    "sql", "SQL", {aliases}, {filenames}, {mimes}, "https://en.wikipedia.org/wiki/SQL", 10,
};
} // namespace

const lexer_info &sql_lexer::info() const noexcept {
    return sql_info;
}

float sql_lexer::analyse_text(std::string_view src) const noexcept {
    float score = 0.0f;
    if (src.find("SELECT") != src.npos || src.find("select") != src.npos) score += 0.3f;
    if (src.find("FROM") != src.npos || src.find("from") != src.npos) score += 0.1f;
    if (src.find("CREATE TABLE") != src.npos || src.find("create table") != src.npos) score += 0.4f;
    return score > 1.0f ? 1.0f : score;
}

state_map sql_lexer::get_rules() const {
    namespace tk = token;
    state_map rules;
    rules["root"] = {
        {R"(\s+)", tk::whitespace, state_action::none()},
        {R"(--[^\n]*)", tk::comment::single, state_action::none()},
        {R"(/\*)", tk::comment::multiline, state_action::push_state("comment")},
        {R"(\b(?i)(SELECT|FROM|WHERE|INSERT|INTO|UPDATE|DELETE|CREATE|DROP|ALTER|TABLE|INDEX|VIEW|DATABASE|SCHEMA|GRANT|REVOKE|SET|SHOW|DESCRIBE|EXPLAIN|USE|BEGIN|COMMIT|ROLLBACK|SAVEPOINT|TRUNCATE|MERGE|REPLACE|CALL|EXECUTE|EXEC|WITH|RECURSIVE|UNION|INTERSECT|EXCEPT|ALL|DISTINCT|AS|ON|JOIN|INNER|LEFT|RIGHT|FULL|OUTER|CROSS|NATURAL|USING|GROUP|BY|ORDER|HAVING|LIMIT|OFFSET|FETCH|NEXT|ROWS|ONLY|ASC|DESC|CASE|WHEN|THEN|ELSE|END|IF|WHILE|LOOP|FOR|RETURN|DECLARE|CURSOR|OPEN|CLOSE|FETCH|INTO|PROCEDURE|FUNCTION|TRIGGER|EXISTS|IN|BETWEEN|LIKE|IS|NOT|AND|OR|NULL|TRUE|FALSE|VALUES|DEFAULT|PRIMARY|KEY|FOREIGN|REFERENCES|UNIQUE|CHECK|CONSTRAINT|ADD|COLUMN|RENAME|TO|CASCADE|RESTRICT|NO|ACTION)\b)",
         tk::keyword::self, state_action::none()},
        {R"(\b(?i)(INT|INTEGER|BIGINT|SMALLINT|TINYINT|FLOAT|DOUBLE|DECIMAL|NUMERIC|REAL|CHAR|VARCHAR|TEXT|NCHAR|NVARCHAR|NTEXT|BLOB|CLOB|DATE|TIME|TIMESTAMP|DATETIME|BOOLEAN|BOOL|SERIAL|BIGSERIAL|UUID|JSON|JSONB|XML|ARRAY|BINARY|VARBINARY|BIT)\b)",
         tk::keyword::type, state_action::none()},
        {R"(\b(?i)(COUNT|SUM|AVG|MIN|MAX|COALESCE|NULLIF|CAST|CONVERT|SUBSTRING|TRIM|UPPER|LOWER|LENGTH|CONCAT|REPLACE|ROUND|FLOOR|CEIL|ABS|NOW|CURRENT_DATE|CURRENT_TIME|CURRENT_TIMESTAMP|EXTRACT|DATE_PART|ROW_NUMBER|RANK|DENSE_RANK|NTILE|LAG|LEAD|FIRST_VALUE|LAST_VALUE)\b)",
         tk::name::builtin, state_action::none()},
        {R"([0-9]+\.[0-9]+([eE][+-]?[0-9]+)?)", tk::literal::number::float_, state_action::none()},
        {R"([0-9]+)", tk::literal::number::integer, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::push_state("string")},
        {R"("[^"]*")", tk::name::self, state_action::none()},
        {R"(`[^`]*`)", tk::name::self, state_action::none()},
        {R"([+\-*/%=<>!]=?|<>|\|\|)", tk::operator_::self, state_action::none()},
        {R"([\[\](){}.,;:@])", tk::punctuation::self, state_action::none()},
        {R"([a-zA-Z_]\w*)", tk::name::self, state_action::none()},
    };
    rules["comment"] = {
        {R"(\*/)", tk::comment::multiline, state_action::pop_state()},
        {R"([^*]+)", tk::comment::multiline, state_action::none()},
        {R"(\*)", tk::comment::multiline, state_action::none()},
    };
    rules["string"] = {
        {R"('')", tk::literal::string::escape, state_action::none()},
        {R"(')", tk::literal::string::single, state_action::pop_state()},
        {R"([^']+)", tk::literal::string::single, state_action::none()},
    };
    return rules;
}

SPEARMINT_API void register_sql_lexer() {
    register_lexer([]() -> std::unique_ptr<lexer> { return std::make_unique<sql_lexer>(); }, sql_info);
}

} // namespace spearmint::lexers

# spearmint — C++23 Semantic Tokenization & Syntax Highlighting Engine (COCA workspace build)
# Provides: spearmint (SHARED), spearmint::spearmint

# ── Sources ─────────────────────────────────────────────────────────────
set(SPEARMINT_SOURCES
    src/core/spearmint.cpp
    src/core/lexer.cpp
    src/core/regex_lexer.cpp
    src/styles/style_registry.cpp
    src/lexers/lexer_registry.cpp
    src/lexers/python.cpp
    src/lexers/cpp.cpp
    src/lexers/json.cpp
    src/lexers/javascript.cpp
    src/lexers/rust.cpp
    src/lexers/go.cpp
    src/lexers/typescript.cpp
    src/lexers/java.cpp
    src/lexers/csharp.cpp
    src/lexers/ruby.cpp
    src/lexers/css.cpp
    src/lexers/html_lexer.cpp
    src/lexers/yaml.cpp
    src/lexers/toml.cpp
    src/lexers/bash.cpp
    src/lexers/sql.cpp
    src/lexers/xml.cpp
    src/lexers/markdown.cpp
    src/lexers/lua.cpp
    src/lexers/php.cpp
    src/lexers/kotlin.cpp
    src/lexers/swift.cpp
    src/lexers/dockerfile.cpp
    src/lexers/makefile.cpp
    src/lexers/ini.cpp
    src/lexers/ftl.cpp
    src/exporters/html.cpp
    src/exporters/ansi.cpp
    src/exporters/svg.cpp
    src/exporters/latex.cpp
    src/filters/builtin_filters.cpp
)

# ── Shared library ──────────────────────────────────────────────────────
if(EMSCRIPTEN)
    add_library(spearmint STATIC ${SPEARMINT_SOURCES})
else()
    add_library(spearmint SHARED ${SPEARMINT_SOURCES})
endif()
add_library(spearmint::spearmint ALIAS spearmint)

target_include_directories(spearmint
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(spearmint PRIVATE SPEARMINT_BUILD_INTERNAL)

if(MSVC)
    target_compile_definitions(spearmint PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(spearmint PRIVATE /W4 /utf-8 /permissive-)
else()
    target_compile_options(spearmint PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── Tests ────────────────────────────────────────────────────────────────
if(TARGET gtest AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

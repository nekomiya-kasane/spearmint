# Use workspace gtest if available, otherwise FetchContent
if(NOT TARGET gtest)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/googletest"
    )
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()

# ── Token test ──────────────────────────────────────────────────────────
add_executable(spearmint_token_test token_test.cpp)
target_link_libraries(spearmint_token_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_token_test COMMAND spearmint_token_test)

# ── Constexpr map test ──────────────────────────────────────────────────
add_executable(spearmint_constexpr_map_test constexpr_map_test.cpp)
target_link_libraries(spearmint_constexpr_map_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_constexpr_map_test COMMAND spearmint_constexpr_map_test)

# ── Style test ──────────────────────────────────────────────────────────
add_executable(spearmint_style_test style_test.cpp)
target_link_libraries(spearmint_style_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_style_test COMMAND spearmint_style_test)

# ── Lexer test ─────────────────────────────────────────────────────────
add_executable(spearmint_lexer_test lexer_test.cpp)
target_link_libraries(spearmint_lexer_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_lexer_test COMMAND spearmint_lexer_test)

# ── Exporter test ──────────────────────────────────────────────────────
add_executable(spearmint_exporter_test exporter_test.cpp)
target_link_libraries(spearmint_exporter_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_exporter_test COMMAND spearmint_exporter_test)

# ── Filter test ────────────────────────────────────────────────────────
add_executable(spearmint_filter_test filter_test.cpp)
target_link_libraries(spearmint_filter_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_filter_test COMMAND spearmint_filter_test)

# ── Multi-lexer test ───────────────────────────────────────────────────
add_executable(spearmint_lexer_multi_test lexer_multi_test.cpp)
target_link_libraries(spearmint_lexer_multi_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_lexer_multi_test COMMAND spearmint_lexer_multi_test)

# ── FTL lexer test ─────────────────────────────────────────────────────
add_executable(spearmint_ftl_lexer_test ftl_lexer_test.cpp)
target_link_libraries(spearmint_ftl_lexer_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_ftl_lexer_test COMMAND spearmint_ftl_lexer_test)

# ── All-languages lexer test ──────────────────────────────────────────
add_executable(spearmint_lexer_all_languages_test lexer_all_languages_test.cpp)
target_link_libraries(spearmint_lexer_all_languages_test PRIVATE spearmint::spearmint GTest::gtest_main)
add_test(NAME spearmint_lexer_all_languages_test COMMAND spearmint_lexer_all_languages_test)

# ── Folder property ─────────────────────────────────────────────────────
set(test_targets
    spearmint_token_test
    spearmint_constexpr_map_test
    spearmint_style_test
    spearmint_lexer_test
    spearmint_exporter_test
    spearmint_filter_test
    spearmint_lexer_multi_test
    spearmint_ftl_lexer_test
    spearmint_lexer_all_languages_test
)

foreach(t IN LISTS test_targets)
    set_target_properties(${t} PROPERTIES FOLDER "spearmint/tests")
endforeach()

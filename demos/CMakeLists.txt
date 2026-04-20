add_executable(demo_phase1_tokens demo_phase1_tokens.cpp)
target_link_libraries(demo_phase1_tokens PRIVATE spearmint::spearmint)

add_executable(demo_phase2_styles demo_phase2_styles.cpp)
target_link_libraries(demo_phase2_styles PRIVATE spearmint::spearmint)

add_executable(demo_phase3_lexer demo_phase3_lexer.cpp)
target_link_libraries(demo_phase3_lexer PRIVATE spearmint::spearmint)

add_executable(demo_phase4_exporters demo_phase4_exporters.cpp)
target_link_libraries(demo_phase4_exporters PRIVATE spearmint::spearmint)

set(demo_targets
    demo_phase1_tokens
    demo_phase2_styles
    demo_phase3_lexer
    demo_phase4_exporters
)

# ── scat: syntax-highlighting cat (requires tapiru) ────────────────────
if(SPEARMINT_BUILD_TAPIRU)
    # Bring in tapiru as a sibling project (disable its tests/examples)
    if(NOT TARGET tapiru::tapiru)
        set(TAPIRU_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(TAPIRU_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(TAPIRU_BUILD_FUZZERS OFF CACHE BOOL "" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tapiru" "${CMAKE_BINARY_DIR}/_tapiru" EXCLUDE_FROM_ALL)
    endif()

    add_executable(scat scat.cpp)
    target_link_libraries(scat PRIVATE spearmint::spearmint tapiru::tapiru)
    list(APPEND demo_targets scat)
endif()

foreach(t IN LISTS demo_targets)
    set_target_properties(${t} PROPERTIES FOLDER "spearmint/demos")
endforeach()

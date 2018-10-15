# MODULE
add_library(
    test-errors-module SHARED
    module.cpp
)

# This line sets the output binary name to "<target_name>.so"
set_target_properties(
    test-errors-module
    PROPERTIES
    PREFIX ""
)

# FAIL-MODULE
add_library(
    test-errors-fail-module SHARED
    fail-module.cpp
)

# This line sets the output binary name to "<target_name>.so"
set_target_properties(
    test-errors-fail-module
    PROPERTIES
    PREFIX ""
)

# TEST
configure_file(scripts/fail.js scripts/fail.js)

add_executable(
    test-errors
    test.h
    test.cpp
)

target_link_libraries(
    test-errors

    v8-cpp
    gtest
)

add_test(
    test-errors
    test-errors
)

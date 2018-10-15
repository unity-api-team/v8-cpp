# MODULE
add_library(
    test-run-module SHARED
    module.cpp
)

# This line sets the output binary name to "<target_name>.so"
set_target_properties(
    test-run-module
    PROPERTIES
    PREFIX ""
)

# TEST
configure_file(scripts/test.js scripts/test.js)
configure_file(scripts/test2.js scripts/test2.js)

add_executable(
    test-run
    test.h
    test.cpp
)

target_link_libraries(
    test-run

    v8-cpp
    gtest
)

add_test(
    test-run
    test-run
)

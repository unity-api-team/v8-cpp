# MODULE
add_library(
    test-methods-module SHARED
    module.cpp
)

# This line sets the output binary name to "<target_name>.so"
set_target_properties(
    test-methods-module
    PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)

# TEST
add_executable(
    test-methods
    test.h
    test.cpp
)

target_link_libraries(
    test-methods

    v8-cpp
    ${GTEST_LIBRARIES}
)

add_test(
    test-methods
    test-methods
)

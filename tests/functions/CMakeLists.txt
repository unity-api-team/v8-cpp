# MODULE
add_library(
    test-functions-module SHARED
    module.cpp
)

# This line sets the output binary name to "<target_name>.node"
set_target_properties(
    test-functions-module
    PROPERTIES
    PREFIX ""
    SUFFIX ".node"
)

# TEST
add_executable(
    test-functions
    test.h
    test.cpp
)

target_link_libraries(
    test-functions

    v8-cpp
    ${GTEST_LIBRARIES}
)

add_test(
    test-functions
    test-functions
)

# MODULE
add_library(
    test-objects-module SHARED
    module.cpp
)

# This line sets the output binary name to "<target_name>"
set_target_properties(
    test-objects-module
    PROPERTIES
    PREFIX ""
    SUFFIX ""
)

# TEST
add_executable(
    test-objects
    test.h
    test.cpp
)

target_link_libraries(
    test-objects

    v8-cpp
    ${GTEST_LIBRARIES}
)

add_test(
    test-objects
    test-objects
)

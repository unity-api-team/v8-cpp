# MODULE
add_library(
    test-members-module SHARED
    module.cpp
)

# This line sets the output binary name to "<target_name>.node"
set_target_properties(
    test-members-module
    PROPERTIES
    PREFIX ""
    SUFFIX ".node"
)

# TEST
add_executable(
    test-members
    test.h
    test.cpp
)

target_link_libraries(
    test-members

    v8-cpp

    ${GTEST_BOTH_LIBRARIES}
    ${GMOCK_LIBRARIES}
    ${TEST_LDFLAGS}
)

add_test(
    test-members
    test-members
)

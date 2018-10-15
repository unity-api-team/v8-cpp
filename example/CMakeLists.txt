file(
    GLOB
    EXAMPLE_FILES
    *.h *.cpp *.js
)

configure_file(example.js example.js)

add_library(
    v8-cpp-example SHARED
    ${EXAMPLE_FILES}
)

# This line sets the output binary name to "v8-cpp-example.node"
set_target_properties(
    v8-cpp-example
    PROPERTIES
    PREFIX ""
    SUFFIX ".node"
)

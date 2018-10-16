file(
    GLOB_RECURSE
    SRC_FILES
    *.h
)

add_library(
    v8-cpp SHARED
    ${SRC_FILES}
)

set_target_properties(
    v8-cpp PROPERTIES
    LINKER_LANGUAGE CXX
)

target_link_libraries(
    v8-cpp

    v8

    dl
    pthread
)

add_custom_command(
    TARGET v8-cpp PRE_LINK
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../deps/build-deps.sh
)

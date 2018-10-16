find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(errors)
add_subdirectory(functions)
add_subdirectory(members)
add_subdirectory(methods)
add_subdirectory(objects)
add_subdirectory(run)

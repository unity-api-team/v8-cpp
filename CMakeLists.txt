cmake_minimum_required(VERSION 2.8)

# Retrieve dependencies

message(STATUS "Retrieving deps")

execute_process(
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/deps/get-deps.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  RESULT_VARIABLE _DEPS_RESULT
  )

if(NOT _DEPS_RESULT EQUAL 0)
  message(FATAL_ERROR "Retrieving deps failed")
endif()

# Configure project environment

project(v8-cpp CXX)

add_definitions(
    -std=c++11
    -fno-permissive
    -pedantic
    -Wall
    -Wextra
    -fPIC
    -Wno-unused-parameter
)

# Configure CMake environment

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower) # Build types should always be lowercase but sometimes they are not.

if(cmake_build_type_lower MATCHES coverage)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -g")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage -g")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -g")
endif()

include(EnableCoverageReport)

# Add includes

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/v8/include
)
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/v8/out/native/obj.target/tools/gyp
)

# Add sources

enable_testing()

add_subdirectory(example)
add_subdirectory(src)
#add_subdirectory(tests)
add_subdirectory(v8runner)

# Enable coverage report

if (cmake_build_type_lower MATCHES coverage)
  ENABLE_COVERAGE_REPORT(TARGETS v8-cpp
                         FILTER /usr/include
                                ${CMAKE_CURRENT_SOURCE_DIR}/deps/*
                                ${CMAKE_CURRENT_SOURCE_DIR}/example/*
                                ${CMAKE_CURRENT_SOURCE_DIR}/tests/*
                                ${CMAKE_BINARY_DIR}/*)
endif()

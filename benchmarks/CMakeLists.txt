cmake_minimum_required(VERSION 3.10)
project(augmento_benchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include main app headers
include_directories(${CMAKE_SOURCE_DIR}/../include)

# Your benchmark source file
set(BENCHMARK_SRC ${CMAKE_SOURCE_DIR}/src/benchmark.cpp)

# Collect all source files from ../src, excluding main.cpp
file(GLOB_RECURSE AUGMENTO_SRC_ALL ${CMAKE_SOURCE_DIR}/../src/*.cpp)

# Filter out main.cpp (any file ending in /main.cpp)
set(AUGMENTO_SRC "")
foreach(f ${AUGMENTO_SRC_ALL})
    if (NOT f MATCHES ".*/main\\.cpp$")
        list(APPEND AUGMENTO_SRC ${f})
    endif()
endforeach()

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(SIMDJSON REQUIRED simdjson)
pkg_check_modules(OPENCV REQUIRED opencv4)

# Create benchmark executable
add_executable(benchmark ${BENCHMARK_SRC} ${AUGMENTO_SRC})

# Include external headers
target_include_directories(benchmark PRIVATE
    ${SIMDJSON_INCLUDE_DIRS}
    ${OPENCV_INCLUDE_DIRS}
)

# Link external libraries
target_link_libraries(benchmark
    ${SIMDJSON_LIBRARIES}
    ${OPENCV_LIBRARIES}
)


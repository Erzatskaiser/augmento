cmake_minimum_required(VERSION 3.10)
project(augmento)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect all source files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SIMDJSON REQUIRED simdjson)
pkg_check_modules(OPENCV REQUIRED opencv4)

# Add executable
add_executable(augmento ${SOURCES})

# Link libraries
target_include_directories(augmento PRIVATE
    ${SIMDJSON_INCLUDE_DIRS}
    ${OPENCV_INCLUDE_DIRS}
)

target_link_libraries(augmento
    ${SIMDJSON_LIBRARIES}
    ${OPENCV_LIBRARIES}
)


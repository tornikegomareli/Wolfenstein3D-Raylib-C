cmake_minimum_required(VERSION 3.10)
project(Wolf3D-GPU C)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Find required packages
find_package(raylib QUIET)

if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Add source files
file(GLOB_RECURSE SOURCES "src/*.c")

# Create executable
add_executable(wolf3d ${SOURCES})

# Link libraries
target_link_libraries(wolf3d raylib)

# Include directories
target_include_directories(wolf3d PRIVATE src)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
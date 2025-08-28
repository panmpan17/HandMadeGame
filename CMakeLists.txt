cmake_minimum_required(VERSION 3.16)

project(GLFWTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (APPLE)
    set(CAMKE_MACOS_ARCHITECTURES "arm64;x86_64")
endif()

set(SOURCES
    src/*.cpp
    src/draw/*.cpp

    include/glad/glad.c
)

message(STATUS "Source files: ${SOURCES}")
add_executable("main" ${SOURCES})

# target_compile_options("main" PRIVATE)

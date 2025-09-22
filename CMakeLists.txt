cmake_minimum_required(VERSION 3.16)

project(GLFWTest
        VERSION 0.1
        DESCRIPTION "GLFW Test"
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (APPLE)
    set(CAMKE_MACOS_ARCHITECTURES "arm64;x86_64")
endif()

set(SOURCES
    "src/camera.cpp"
    "src/components/character2d.cpp"
    "src/components/component.cpp"
    "src/components/movement.cpp"
    "src/components/particle/particle_lifetime_change.cpp"
    "src/components/particle/particle_spawn.cpp"
    "src/components/particle/particle_system.cpp"
    "src/components/particle/simple_particle_system.cpp"
    "src/components/quad.cpp"
    "src/components/rotate.cpp"
    "src/components/sprite_animation.cpp"
    "src/components/triangle.cpp"
    "src/draw/image.cpp"
    "src/draw/shader_loader.cpp"
    "src/draw/shader.cpp"
    "src/error_handler.cpp"
    "src/file_utils.cpp"
    "src/game/pingpong/paddle_control.cpp"
    "src/game/pingpong/pong.cpp"
    "src/input_handle.cpp"
    "src/node.cpp"
    "src/post_process/bloom_test.cpp"
    "src/post_process/order_dithering.cpp"
    "src/post_process/render_process_queue.cpp"
    "src/random.cpp"
    "src/serialization/serializer.cpp"
    "src/vector.cpp"
    "src/window.cpp"
    "src/world.cpp"
)

message(STATUS "Source files: ${SOURCES}")
add_executable(GLFWTest
               "src/main.cpp"
               "include/glad/glad.c"
               ${SOURCES}
)

target_include_directories(GLFWTest PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_directories(GLFWTest PRIVATE ${CMAKE_SOURCE_DIR}/lib/mac)
target_link_libraries(GLFWTest PRIVATE glfw.3)

target_link_libraries(GLFWTest
    PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreVideo"
)

set_target_properties(GLFWTest PROPERTIES
    BUILD_RPATH "@loader_path/lib"
    INSTALL_RPATH "@loader_path/lib"
)
# target_include_directories(my_program PRIVATE include)



# target_compile_options("main" PRIVATE)

cmake_minimum_required(VERSION 3.16)

project(GLFWTest
        VERSION 0.1
        DESCRIPTION "GLFW Test"
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Options
option(BUILD_MAC_APP "Build as a macOS .app bundle" OFF)

# Default to Release if nothing is specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release") # "RelWithDebInfo" "MinSizeRel")


if (APPLE)
    set(CMAKE_MACOS_ARCHITECTURES "arm64;x86_64")
endif()


# Source files
set(SOURCES
    "src/main.cpp"
    "src/post_process/render_process_queue.cpp"
    "src/post_process/order_dithering.cpp"
    "src/post_process/bloom_test.cpp"
    "src/post_process/difference_of_gaussian.cpp"
    "src/camera.cpp"
    "src/error_handler.cpp"
    "src/window.cpp"
    "src/random.cpp"
    "src/serialization/serializer.cpp"
    "src/node.cpp"
    "src/models/simple_obj_reader.cpp"
    "src/game/pingpong/pong.cpp"
    "src/game/pingpong/paddle_control.cpp"
    "src/input_handle.cpp"
    "src/vector.cpp"
    "src/components/sprite_animation.cpp"
    "src/components/mesh_renderer.cpp"
    "src/components/component.cpp"
    "src/components/quad.cpp"
    "src/components/character2d.cpp"
    "src/components/movement.cpp"
    "src/components/rotate.cpp"
    "src/components/triangle.cpp"
    "src/components/particle/particle_spawn.cpp"
    "src/components/particle/particle_system.cpp"
    "src/components/particle/simple_particle_system.cpp"
    "src/components/particle/particle_lifetime_change.cpp"
    "src/file_utils.cpp"
    "src/world.cpp"
    "src/draw/image.cpp"
    "src/draw/shader_loader.cpp"
    "src/draw/shader.cpp"
)

if (APPLE)
    set(DYLIB_DIR ${CMAKE_SOURCE_DIR}/lib/mac)

    if (BUILD_MAC_APP)
        add_executable(GLFWTest MACOSX_BUNDLE
                    ${SOURCES}
                    ${CMAKE_SOURCE_DIR}/MacAppPack/icon-windowed.icns
        )

        set(MACOSX_BUNDLE_ICON_FILE icon-windowed.icns)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/MacAppPack/icon-windowed.icns
                                    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

        set(EXECUTABLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/GLFWTest.app/Contents/MacOS)

    else()
        add_executable(GLFWTest
                       ${SOURCES}
        )

        set(EXECUTABLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()

    # copy assets folder to beside executable
    add_custom_command(TARGET GLFWTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${EXECUTABLE_PATH}/assets
    )

    add_custom_command(TARGET GLFWTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            ${EXECUTABLE_PATH}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${DYLIB_DIR}
            ${EXECUTABLE_PATH}/lib
    )
else()
    add_executable(GLFWTest
                   ${SOURCES}
    )

    set(EXECUTABLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(DYLIB_DIR ${CMAKE_SOURCE_DIR}/lib/)
    
    # copy assets folder to beside executable
    add_custom_command(TARGET GLFWTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${EXECUTABLE_PATH}/assets
    )

    add_custom_command(TARGET GLFWTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${DYLIB_DIR}
            ${EXECUTABLE_PATH}
    )
endif()

target_include_directories(GLFWTest PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(GLFWTest PRIVATE DEBUG_FLAG=1)
else()
    target_compile_definitions(GLFWTest PRIVATE DEBUG_FLAG=0)
endif()


# General Library
add_library(glad include/glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/include)


# Platform-specific settings
if (APPLE)
    # MacOS specific GLFW settings
    add_library(glfw SHARED IMPORTED)
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/mac/libglfw.3.dylib
    )

    # Add mac specific frameworks
    add_library(mac_frameworks INTERFACE)
    target_link_libraries(GLFWTest PRIVATE mac_frameworks)

    # Properly set rpath for macOS
    set_target_properties(GLFWTest PROPERTIES
        BUILD_RPATH "@loader_path/lib"
        INSTALL_RPATH "@loader_path/lib"
    )

elseif (WIN32)
    # MacOS specific GLFW settings
    add_library(glfw SHARED IMPORTED)
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/glfw3.dll
        IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a
    )

    # target_link_options(GLFWTest PRIVATE -mwindows)
elseif(UNIX)

endif()

set_target_properties(glfw PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include
)


target_link_libraries(GLFWTest
    PRIVATE
        glad
        glfw
)

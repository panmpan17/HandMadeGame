cmake_minimum_required(VERSION 3.16)

project(MichaelHandMadeGame
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
if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING "Build type" FORCE)
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
    "src/serialization/type_registry.cpp"
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
    "src/components/first_person_free_control_camera.cpp"
    "src/file_utils.cpp"
    "src/world.cpp"
    "src/draw/image.cpp"
    "src/draw/shader_loader.cpp"
    "src/draw/shader.cpp"
    "src/editor/camera_inspector.cpp"
    "src/editor/hierarchy_view.cpp"
    "src/editor/post_process_inspector.cpp"
)

if (APPLE)
    if (BUILD_MAC_APP)
        add_executable(MichaelHandMadeGame MACOSX_BUNDLE
                    ${SOURCES}
                    ${CMAKE_SOURCE_DIR}/app_packing/icon-windowed.icns
        )

        set(MACOSX_BUNDLE_ICON_FILE icon-windowed.icns)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/app_packing/icon-windowed.icns
                                    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

        set(EXECUTABLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MichaelHandMadeGame.app/Contents/MacOS)

        set(ASSET_BUILD_TARGET_FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MichaelHandMadeGame.app/Contents/Resources/assets)

        target_compile_options(MichaelHandMadeGame PRIVATE
            -DMAC_APP_BUNDLE
        )

        add_custom_command(TARGET MichaelHandMadeGame POST_BUILD
            COMMAND codesign --force --deep --sign "ApplicationCodeSign"
                    "$<TARGET_BUNDLE_DIR:MichaelHandMadeGame>"
        )

    else()
        add_executable(MichaelHandMadeGame
                       ${SOURCES}
        )

        set(EXECUTABLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        set(ASSET_BUILD_TARGET_FOLDER ${EXECUTABLE_PATH}/assets)
    endif()

    target_compile_options(MichaelHandMadeGame PRIVATE
        -Wmacro-redefined
        -Wno-delete-abstract-non-virtual-dtor
    )
else()
    add_executable(MichaelHandMadeGame
                   ${SOURCES}
    )

    set(EXECUTABLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(ASSET_BUILD_TARGET_FOLDER ${EXECUTABLE_PATH}/assets)
    
    target_compile_options(MichaelHandMadeGame PRIVATE
        -Wbuiltin-macro-redefined
        # -w
    )
endif()

target_include_directories(MichaelHandMadeGame PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(MichaelHandMadeGame PRIVATE DEBUG_FLAG=1)
else()
    target_compile_definitions(MichaelHandMadeGame PRIVATE DEBUG_FLAG=0)
endif()



# Assets files
file(GLOB_RECURSE ASSETS "${CMAKE_SOURCE_DIR}/assets/*")

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/assets.timestamp
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${ASSET_BUILD_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/assets.timestamp
    DEPENDS ${ASSETS}
)
add_custom_target(copy_assets ALL DEPENDS ${CMAKE_BINARY_DIR}/assets.timestamp)
add_dependencies(MichaelHandMadeGame copy_assets)


# General Library
add_subdirectory( external/glfw )

add_library(glad include/glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(imgui STATIC
    "external/imgui/imgui.cpp"
    "external/imgui/imgui_demo.cpp"
    "external/imgui/imgui_draw.cpp"
    "external/imgui/imgui_tables.cpp"
    "external/imgui/imgui_widgets.cpp"
    "external/imgui/backends/imgui_impl_glfw.cpp"
    "external/imgui/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

target_link_libraries(MichaelHandMadeGame PRIVATE imgui)


# Platform-specific settings
if (APPLE)
    # Add mac specific frameworks
    add_library(mac_frameworks INTERFACE)
    target_link_libraries(MichaelHandMadeGame PRIVATE mac_frameworks)

    # Properly set rpath for macOS
    set_target_properties(MichaelHandMadeGame PROPERTIES
        BUILD_RPATH "@loader_path/lib"
        INSTALL_RPATH "@loader_path/lib"
    )

elseif (WIN32)
    target_link_options(MichaelHandMadeGame PRIVATE -mwindows)
elseif(UNIX)

endif()

target_link_libraries(MichaelHandMadeGame
    PRIVATE
        glad
        glfw
)


target_link_libraries(imgui
    PRIVATE
        glfw
)


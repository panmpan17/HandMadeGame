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
    "src/models/mesh.cpp"
    "src/models/simple_obj_reader.cpp"
    "src/models/assimp_model_reader.cpp"
    "src/game/pingpong/pong.cpp"
    "src/game/pingpong/paddle_control.cpp"
    "src/input_handle.cpp"
    "src/vector.cpp"
    "src/light_manager.cpp"
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


file(GLOB_RECURSE ASSETS "${CMAKE_SOURCE_DIR}/assets/*")
file(GLOB_RECURSE ASSETS_RELATIVE RELATIVE "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_SOURCE_DIR}/assets/*")


set(DO_COPY_ASSETS_CUSTOM_COMMAND ON)

if (APPLE)
    if (CMAKE_GENERATOR STREQUAL "Xcode")
        set(DO_COPY_ASSETS_CUSTOM_COMMAND OFF)

        # This will make xcode execute in command mode
        # add_executable(MichaelHandMadeGame
        #                ${SOURCES}
        # )

        # This will make xcode build an application build, then start it
        add_executable(MichaelHandMadeGame MACOSX_BUNDLE
                       ${SOURCES}
        )

        # For macOS builds, mark files as bundle resources
        
        set(ASSET_SOURCES "")
        foreach(asset ${ASSETS_RELATIVE})
            message(STATUS "Adding asset to bundle: ${asset}")
            set(src "${CMAKE_SOURCE_DIR}/assets/${asset}")
            # Compute the relative path inside the bundle
            get_filename_component(asset_path "${asset}" PATH)
            set_source_files_properties("${src}" PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources/assets/${asset_path}"
            )
            list(APPEND ASSET_SOURCES "${src}")
        endforeach()

        target_compile_options(MichaelHandMadeGame PRIVATE
            -DMAC_APP_BUNDLE
        )

        target_sources(MichaelHandMadeGame PRIVATE ${ASSET_SOURCES})

        # In future can modify the xcode project file directly to add a folder reference
        # to the assets folder, so that changes to assets are automatically reflected
        # isa = PBXFileReference;
        # lastKnownFileType = folder;
        # name = Resources;
        # path = assets;
        # sourceTree = "<group>";

    elseif (BUILD_MAC_APP)
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


if (DO_COPY_ASSETS_CUSTOM_COMMAND)
    # Copy assets files
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
endif()


### General Library
add_subdirectory( external/glfw )


# Glad Library
if (TRUE)
    add_library(glad include/glad/glad.c)
    target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(MichaelHandMadeGame PRIVATE glad)
endif()

# Assimp Library
if (TRUE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

    set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

    add_subdirectory( external/assimp )

    target_link_libraries(MichaelHandMadeGame PRIVATE assimp)
endif()

# ImGui Library
if (TRUE)
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
endif()


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

target_link_libraries(MichaelHandMadeGame PRIVATE glfw )
target_link_libraries(imgui PRIVATE glfw)


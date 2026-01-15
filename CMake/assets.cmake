set(ASSET_SOURCE_FOLDER "${CMAKE_SOURCE_DIR}/assets")

# Compile Metal Shaders Before Copy assets
set(METAL_SHADER_FOLDER "${ASSET_SOURCE_FOLDER}/metal_shaders")
set(METAL_SHADER_COMPILED_FOLDER "${CMAKE_BINARY_DIR}/metal_shaders")

file(GLOB_RECURSE METAL_SHADER_SRC_ABS "${METAL_SHADER_FOLDER}/*.metal")
file(GLOB_RECURSE METAL_SHADER_SRC RELATIVE "${METAL_SHADER_FOLDER}" "${METAL_SHADER_FOLDER}/*.metal")
set(METAL_SHADER_LIB "${ASSET_SOURCE_FOLDER}/metal_shaders.metallib")

function(ADD_COMPILE_METAL_SHADERS_COMMAND applicationName)
    add_custom_command(
        OUTPUT ${METAL_SHADER_LIB}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${METAL_SHADER_COMPILED_FOLDER}
        DEPENDS ${METAL_SHADER_SRC_ABS}
    )

    foreach (shaderFile ${METAL_SHADER_SRC})
        add_custom_command(
            OUTPUT ${METAL_SHADER_LIB}
            APPEND COMMAND xcrun -sdk macosx metal -o ${METAL_SHADER_COMPILED_FOLDER}/${shaderFile}.ir -c ${METAL_SHADER_FOLDER}/${shaderFile}
        )
    endforeach()

    # TODO: First remove the .metalar, then for loop link the .ir files into a .metalar, then compile that into a .metallib
    # xcrun -sdk macosx metal-ar -q "output.metalar" "input1.ir"
    # xcrun -sdk macosx metal-ar -q "output.metalar" "input2.ir"
    foreach (shaderFile ${METAL_SHADER_SRC})
        add_custom_command(
            OUTPUT ${METAL_SHADER_LIB}
            APPEND COMMAND xcrun -sdk macosx metal -o ${METAL_SHADER_LIB} ${METAL_SHADER_COMPILED_FOLDER}/${shaderFile}.ir
        )
    endforeach()

    add_custom_target(copy_metal_lib ALL DEPENDS ${METAL_SHADER_LIB})
    add_dependencies(${applicationName} copy_metal_lib)
endfunction()


file(GLOB_RECURSE ASSETS "${ASSET_SOURCE_FOLDER}/*")
file(GLOB_RECURSE ASSETS_RELATIVE RELATIVE "${ASSET_SOURCE_FOLDER}" "${ASSET_SOURCE_FOLDER}/*")

function(ADD_COPY_ASSETS_COMMAND applicationName assetBuildTargetFolder)
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/assets.timestamp
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSET_SOURCE_FOLDER}
            ${assetBuildTargetFolder}
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/assets.timestamp
        DEPENDS ${ASSETS}
    )
    add_custom_target(copy_assets ALL DEPENDS ${CMAKE_BINARY_DIR}/assets.timestamp)
    add_dependencies(${applicationName} copy_assets)
endfunction()

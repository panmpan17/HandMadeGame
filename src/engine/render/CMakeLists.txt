list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/image.h
    ${CMAKE_CURRENT_LIST_DIR}/image_loader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/image_loader.h
    ${CMAKE_CURRENT_LIST_DIR}/material.h
    ${CMAKE_CURRENT_LIST_DIR}/material.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shader.h
    ${CMAKE_CURRENT_LIST_DIR}/shader_loader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shader_loader.h
    ${CMAKE_CURRENT_LIST_DIR}/skybox.cpp
    ${CMAKE_CURRENT_LIST_DIR}/skybox.h
    ${CMAKE_CURRENT_LIST_DIR}/vertex.h
)

# Include sub-directory CMakeLists
include(${CMAKE_CURRENT_LIST_DIR}/post_process/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/models/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/lighting/CMakeLists.txt)

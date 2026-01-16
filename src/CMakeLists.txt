list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

# Include sub-directory CMakeLists
include(${CMAKE_CURRENT_LIST_DIR}/utils/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/game/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/engine/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/editor/CMakeLists.txt)

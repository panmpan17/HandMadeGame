list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/component.cpp
    ${CMAKE_CURRENT_LIST_DIR}/component.h
    ${CMAKE_CURRENT_LIST_DIR}/drawable_interface.h
)

# Include sub-directory CMakeLists
include(${CMAKE_CURRENT_LIST_DIR}/input/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/render/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/particle/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/transform/CMakeLists.txt)

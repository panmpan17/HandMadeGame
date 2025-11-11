list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/camera.h
    ${CMAKE_CURRENT_LIST_DIR}/debug_macro.h
    ${CMAKE_CURRENT_LIST_DIR}/engine_event_dispatcher.h
    ${CMAKE_CURRENT_LIST_DIR}/error_handler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/error_handler.h
    ${CMAKE_CURRENT_LIST_DIR}/window.cpp
    ${CMAKE_CURRENT_LIST_DIR}/window.h
)

# Include sub-directory CMakeLists
include(${CMAKE_CURRENT_LIST_DIR}/input/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/serialization/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/math/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/scene/CMakeLists.txt)

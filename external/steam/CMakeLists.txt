cmake_minimum_required(VERSION 3.16)
project(SteamAppTicketSDK)


add_library(SteamAppTicket INTERFACE)

target_include_directories(SteamAppTicket INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})



if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64-bit
        set(STEAM_LIB_PATH "lib/win64/steam_api64.lib")
        set(STEAM_ENCRYPTED_APP_TICKET_LIB_PATH "lib/win64/sdkencryptedappticket64.lib")
    else()
        # 32-bit
        set(STEAM_LIB_PATH "lib/win32/steam_api.lib")
        set(STEAM_ENCRYPTED_APP_TICKET_LIB_PATH "lib/win32/sdkencryptedappticket.lib")
    endif()

elseif(APPLE)
    set(STEAM_LIB_PATH "lib/osx/libsteam_api.dylib")
    set(STEAM_ENCRYPTED_APP_TICKET_LIB_PATH "lib/osx/libsdkencryptedappticket.dylib")

elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64|arm64")
        # ARM 64-bit
        set(STEAM_LIB_PATH "lib/linuxarm64/libsteam_api.so")
        set(STEAM_ENCRYPTED_APP_TICKET_LIB_PATH "lib/linuxarm64/libsdkencryptedappticket.so")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # x86 64-bit
        set(STEAM_LIB_PATH "lib/linux64/libsteam_api.so")
        set(STEAM_ENCRYPTED_APP_TICKET_LIB_PATH "lib/linux64/libsdkencryptedappticket.so")
    else()
        # x86 32-bit
        set(STEAM_LIB_PATH "lib/linux32/libsteam_api.so")
        set(STEAM_ENCRYPTED_APP_TICKET_LIB_PATH "lib/linux32/libsdkencryptedappticket.so")
    endif()

else()
    message(FATAL_ERROR "Unsupported platform for the Steam App Ticket SDK.")
endif()

set(STEAM_SHARED_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${STEAM_LIB_PATH}" PARENT_SCOPE) 
target_link_libraries(SteamAppTicket INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${STEAM_LIB_PATH}")

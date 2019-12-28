cmake_minimum_required(VERSION 3.6)
project(XNativeMain)

# Pedantic Warnings
if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-long-long -pedantic")
endif()

# Libraries
add_subdirectory(lib/lib_common)
add_subdirectory(lib/lib_game)
add_subdirectory(lib/lib_net)

# Applications
add_subdirectory(app/app_client_headless)
add_subdirectory(app/app_server)


# Settings
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT app_server)
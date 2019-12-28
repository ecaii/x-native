cmake_minimum_required(VERSION 3.6)
project(XNativeMain)

# Libraries
add_subdirectory(lib/lib_common)
add_subdirectory(lib/lib_game)

# Applications
add_subdirectory(app/client-headless)
add_subdirectory(app/server)

# Settings
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT server)
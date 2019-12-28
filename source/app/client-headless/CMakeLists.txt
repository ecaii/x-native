project(client_headless)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Libraries
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_common/inc)
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_game/inc)
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_net/inc)

# IO query headers - could we remove these?
file(GLOB_RECURSE CLIENT_HEADLESS_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
file(GLOB_RECURSE CLIENT_HEADLESS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(client_headless "${CLIENT_HEADLESS_SOURCES}" "${CLIENT_HEADLESS_HEADERS}")
add_dependencies(client_headless
    lib_common
    lib_game
    lib_net
)

target_link_libraries(client_headless
    lib_common
    lib_game
    lib_net
)

install(
    TARGETS  client_headless
    CONFIGURATIONS Debug
    DESTINATION bin/Debug
)

install(
    TARGETS client_headless
    CONFIGURATIONS Release
    DESTINATION bin/Release
)
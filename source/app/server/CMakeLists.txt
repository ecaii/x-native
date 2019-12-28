project(server)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Libraries
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_common/inc)
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_game/inc)
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_net/inc)

# IO query headers - could we remove these?
file(GLOB_RECURSE SERVER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(server "${SERVER_SOURCES}" "${SERVER_HEADERS}")
add_dependencies(server
    lib_common
    lib_game
    lib_net
)

target_link_libraries(server
    lib_common
    lib_game
    lib_net
)

install(
    TARGETS  server
    CONFIGURATIONS Debug
    DESTINATION bin/Debug
)

install(
    TARGETS server
    CONFIGURATIONS Release
    DESTINATION bin/Release
)
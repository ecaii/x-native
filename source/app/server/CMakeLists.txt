project(server)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Libraries
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_common/inc)

# IO query headers
file(GLOB_RECURSE SERVER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(server "${SERVER_SOURCES}" "${SERVER_HEADERS}")
add_dependencies(server
    lib_common
)

target_link_libraries(server
    lib_common
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
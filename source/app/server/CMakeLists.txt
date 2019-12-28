project(server)

include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SERVER_SOURCES)

# IO query headers
file(GLOB_RECURSE SERVER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")

add_executable(server "${SERVER_SOURCES}" "${SERVER_HEADERS}")
add_dependencies(server shared)
target_link_libraries(server shared)

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
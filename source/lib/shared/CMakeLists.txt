project(shared)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SHARED_SOURCES)

# IO query headers
file(GLOB_RECURSE SHARED_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")

add_library(shared "${SHARED_SOURCES}" "${SHARED_HEADERS}")

install(
    TARGETS shared
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    CONFIGURATIONS Debug
)

install(
    TARGETS shared
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    CONFIGURATIONS Release
)
project(lib_game)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

#
# Require the common headers
#
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_common/inc)

set(lib_game_headers
	"inc/consoledevices/consoledevice"
	"inc/consoledevices/consoledevicewindows"
	"inc/gameconsole"
	"inc/gamehighrestimer"
	"inc/gamejobsystem"
	"inc/gamesettings"
)

set(lib_game_sources
	"src/consoledevices/consoledevice"
	"src/consoledevices/consoledevicewindows"
	"src/gameconsole"
	"src/gamehighrestimer"
	"src/gamejobsystem"
	"src/gamesettings"
)

add_library(lib_game "${lib_game_sources}" "${lib_game_headers}")

add_dependencies(lib_game
    lib_common
)

install(
    TARGETS lib_game
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    CONFIGURATIONS Debug
)

install(
    TARGETS lib_game
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    CONFIGURATIONS Release
)
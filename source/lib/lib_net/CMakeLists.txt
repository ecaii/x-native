project(lib_net)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

#
# Require the common headers
#
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_common/inc)
include_directories(${CMAKE_SOURCE_DIR}/lib/lib_game/inc)
include_directories(${ENET_INCLUDE_DIRS})

set(lib_net_headers
	"inc/networksynchronisation"
	"inc/networkconnection"
	"inc/networkconnectionmanager"
	"inc/networkmanager"
	"inc/networkmanagerclient"
	"inc/networkmanagerdedicatedserver"
	"inc/networkobject"
	"inc/networkshared"
	"inc/networkvariable"
	"inc/objects/networkactorobject"
)

set(lib_net_sources
	"src/networksynchronisation"
	"src/networkconnection"
	"src/networkconnectionmanager"
	"src/networkmanager"
	"src/networkmanagerclient"
	"src/networkmanagerdedicatedserver"
	"src/networkobject"
	"src/networkshared"
	"src/networkvariable"
	"src/objects/networkactorobject"
)

add_library(lib_net "${lib_net_sources}" "${lib_net_headers}")

add_dependencies(lib_net
    lib_common
	lib_game
)

target_link_libraries(lib_net
    "${ENET_LIBRARIES}"
)

install(
    TARGETS lib_net
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    CONFIGURATIONS Debug
)

install(
    TARGETS lib_net
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    CONFIGURATIONS Release
)
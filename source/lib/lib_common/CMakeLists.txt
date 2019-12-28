project(lib_common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(lib_common_headers
	"inc/assert"
	"inc/hash"
	"inc/maths"
	"inc/constants"
	"inc/logging"
	"inc/libdummy"
	"inc/unittests"
)

set(lib_common_sources
	"src/logging"
	"src/libdummy"
	"src/unittests"
)

add_library(lib_common "${lib_common_sources}" "${lib_common_headers}")

install(
    TARGETS lib_common
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    CONFIGURATIONS Debug
)

install(
    TARGETS lib_common
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    CONFIGURATIONS Release
)
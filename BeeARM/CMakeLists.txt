set(BEEARM_HEADERS
	beearm.h)

set(BEEARM_SOURCES
	beearm.cpp)

add_library(beearm ${BEEARM_SOURCES} ${BEEARM_HEADERS})
target_include_directories(beearm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(libbeearm ALIAS beearm)
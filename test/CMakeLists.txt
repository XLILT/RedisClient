cmake_minimum_required(VERSION 2.8)

###############################################################################
# project name
set(PROJ_NAME main)
project(${PROJ_NAME})

###############################################################################
# compile attribute            
set(CMAKE_VERBOSE_MAKEFILE ON) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wno-deprecated")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -g  -Wall -Wno-deprecated")

# include directors
include_directories(
    ../..
)

# link directors
link_directories()

# target
set(TARGET ${PROJECT_NAME})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} current_sources)

add_executable(${TARGET} ${current_sources})

# link libraries
target_link_libraries(
	${TARGET}
    libhiredis.a
)


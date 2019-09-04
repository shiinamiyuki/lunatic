
cmake_minimum_required (VERSION 3.1)

project(lunatic)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
    "-std=c++11 -O3")
include_directories("include/")
include_directories("include/lunatic")
file( GLOB  lunaticSRC "src/*.cc")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

add_executable(lunatic 
	${lunaticSRC}
)
list(FILTER  lunaticSRC EXCLUDE REGEX "src/main.cc")
add_library(liblunatic ${lunaticSRC})


cmake_minimum_required (VERSION 3.1)

project(lunatic)
set(CMAKE_CXX_STANDARD 11)
include_directories("include/lunatic")
file( GLOB  lunaticSRC "src/*.cc")
add_executable(lunatic 
	${lunaticSRC}
)
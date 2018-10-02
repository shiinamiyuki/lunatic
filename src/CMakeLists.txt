cmake_minimum_required(VERSION 3.12)
project(lunatic)
set(CMAKE_CXX_STANDARD 17)
link_libraries(glfw3 opengl32)
add_executable(lunatic main.cc lib.cc
        ast.cc gc.cc opcode.cc value.cc table.cc
        codegen.cc
        vm.cc parse.cc closure.cc scriptengine.cc visitor.h format.cc format.h upvalue.cc upvalue.h)
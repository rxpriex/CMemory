cmake_minimum_required(VERSION 3.10)
project(CMemory C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(main src/main.c src/Memory.c)

set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "out")

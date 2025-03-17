cmake_minimum_required(VERSION 3.10)
project(CMemory C)

set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/clang.exe")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --target=x86_64-w64-windows-gnu -g -v")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(main src/main.c src/Memory.c)

set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "out")
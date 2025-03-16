# CMemory
A C project demonstrating memory management with CMake.

## Build Instructions
1. Install MSYS2, Clang, and CMake.
2. Clone the repository: `git clone https://github.com/YourUsername/CMemory.git`
3. Navigate to the project: `cd CMemory`
4. Create a build directory: `mkdir build && cd build`
5. Configure: `cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER="C:/msys64/mingw64/bin/clang.exe" ..`
6. Build: `cmake --build .`
7. Run: `out/main.exe`

## Dependencies
- MSYS2 MinGW 64-bit
- Clang
- CMake
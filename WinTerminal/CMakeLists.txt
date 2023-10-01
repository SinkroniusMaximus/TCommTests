cmake_minimum_required(VERSION 3.13)  # CMake version check
project(TCommWinTerminal)             # Create project "TCommWinTerminal"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

#search and find all required files using recursive globbin
file(GLOB_RECURSE Library  ../Lib/*.h)
file(GLOB_RECURSE Headers ./*.h)
file(GLOB_RECURSE Source ./*.cpp)

# Add executable target with source files
add_executable(TCommWinTerminal ${Source})
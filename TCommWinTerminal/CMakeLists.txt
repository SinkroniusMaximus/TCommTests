cmake_minimum_required(VERSION 3.13)  # CMake version check
project(TCommTest)             # Create project "TCommTest"
set(CMAKE_CXX_STANDARD 17)            # Enable c++19 standard

# Add the SetupAPI library
find_library(SETUPAPI_LIB setupapi)

add_subdirectory(lib/Communication/open62541pp)  # the submodule directory

#search and find all required files using globbin
file(GLOB Source ./*.cpp)

# Add executable target with source files
add_executable(TCommTest ${Source})
target_link_libraries(GenericLib PRIVATE open62541pp::open62541pp)
target_link_libraries(GenericLib PRIVATE ${SETUPAPI_LIB})
cmake_minimum_required(VERSION 3.13)  # CMake version check
project(simple_example)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 17)            # Enable c++14 standard

# Add main.cpp file of the project root directory as a source file
set(SOURCE_FILES main.cpp ../lib/msp.cpp ../lib/utils.cpp ../lib/osd/layout.cpp ../lib/osd/utils.cpp ../lib/osd/elements/base.cpp ../lib/osd/elements/objects.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(example ${SOURCE_FILES})
target_compile_options(example PUBLIC -std=c++17 -Wno-narrowing)

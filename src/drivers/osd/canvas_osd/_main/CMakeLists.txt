cmake_minimum_required(VERSION 3.13)
project(simple_example)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(osdlib)

add_executable(main main.cpp)
set_target_properties(main PROPERTIES COMPILE_FLAGS "-nostdinc++")
target_link_libraries(main PRIVATE stdosdlib)
target_link_options(main PRIVATE -nostdlib)
# target_link_options(main PRIVATE -static-libgcc -static-libstdc++)

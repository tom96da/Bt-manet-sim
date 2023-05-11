cmake_minimum_required(VERSION 3.22)  # CMake version check
project(Bt_manet_sim)                 # Create project "simple_example"
set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard

# Add main.cpp file of project root directory as source file
set(SOURCE main.cpp Device.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(Bt_manet_sim ${SOURCE})
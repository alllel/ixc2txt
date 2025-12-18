cmake_minimum_required(VERSION 4.0)
project(ixc2txt LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_executable(ixc2txt main.cpp
        RegFileExt.cpp)
target_link_options(ixc2txt PUBLIC -static-libstdc++ -static-libgcc)
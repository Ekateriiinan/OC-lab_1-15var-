cmake_minimum_required(VERSION 3.10)
project(OS_Project)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(parent parent.cpp common.cpp)
add_executable(child child.cpp common.cpp)

cmake_minimum_required (VERSION 3.10)
project (SDF2)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (SDF2 src/main.cpp)
target_include_directories (SDF2 PRIVATE dep src)
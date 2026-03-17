cmake_minimum_required (VERSION 3.10)
project (SDF2)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (superblob src/scripts/superblob.cpp)
target_include_directories (superblob PRIVATE dep src)
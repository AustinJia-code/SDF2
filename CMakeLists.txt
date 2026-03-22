cmake_minimum_required (VERSION 3.10)
project (SDF2)


set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable (superblob src/scripts/superblob.cpp)
target_include_directories (superblob PRIVATE dep src)

add_executable (box src/scripts/box.cpp)
target_include_directories (box PRIVATE dep src)

add_executable (wheel src/scripts/wheel.cpp)
target_include_directories (wheel PRIVATE dep src)

add_executable (box_bench src/tools/box_bench.cpp)
target_include_directories (box_bench PRIVATE dep src)
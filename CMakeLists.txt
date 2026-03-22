cmake_minimum_required (VERSION 3.10)
project (SDF2)


set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)


find_package (OpenMP REQUIRED)


add_executable (superblob src/scripts/superblob.cpp)
target_include_directories (superblob PRIVATE dep src)
target_link_libraries (superblob PRIVATE OpenMP::OpenMP_CXX)

add_executable (box src/scripts/box.cpp)
target_include_directories (box PRIVATE dep src)
target_link_libraries (box PRIVATE OpenMP::OpenMP_CXX)

add_executable (wheel src/scripts/wheel.cpp)
target_include_directories (wheel PRIVATE dep src)
target_link_libraries (wheel PRIVATE OpenMP::OpenMP_CXX)

add_executable (box_bench src/tools/box_bench.cpp)
target_include_directories (box_bench PRIVATE dep src)
target_link_libraries (box_bench PRIVATE OpenMP::OpenMP_CXX)
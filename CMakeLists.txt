cmake_minimum_required (VERSION 3.10)
project (SDF2)


set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)


# Build type flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options (-g -O0)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options (-O3 -DNDEBUG)
endif ()


set (SCRIPT_INCLUDES dep src)

add_executable (superblob src/examples/superblob.cpp)
target_include_directories (superblob PRIVATE ${SCRIPT_INCLUDES})

add_executable (box src/examples/box.cpp)
target_include_directories (box PRIVATE ${SCRIPT_INCLUDES})

add_executable (wheel src/examples/wheel.cpp)
target_include_directories (wheel PRIVATE ${SCRIPT_INCLUDES})

add_executable (prisms src/examples/prisms.cpp)
target_include_directories (prisms PRIVATE ${SCRIPT_INCLUDES})

add_executable (box_bench src/tools/box_bench.cpp)
target_include_directories (box_bench PRIVATE dep src)

add_executable (sdf2_script src/script/sdf2_script.cpp)
target_include_directories (sdf2_script PRIVATE ${SCRIPT_INCLUDES})

/**
 * @file box_bench.cpp
 * @brief Benchmarking box gen time.
 * 
 * ./box_bench
 */

#include "minis/arg_parser/arg_parser.hpp"

#include "core/sdf2.hpp"
#include <iostream>
#include <fstream>
#include "minis/benchmark/benchmark.hpp"

static FormPtr form;
static trimesh_t mesh;
static std::vector<char> stl;

void march_bench ()
{
    mesh = marching_cubes (form, gu::dist_t {0.2});
}

void t_to_stl_bench ()
{
    stl = trimesh_to_stl_binary (mesh);
}

int main (int argc, char *argv[])
{
    // Build form
    form = add_rotation (make_box (40, 40, 40), Z_AXIS, 10);

    // Output
    bench::Params params {warmup_iterations: 0,
                          iterations: 1};

    auto results = bench::benchmark (params, march_bench);

    std::cout << "Marching Cubes Benchmark:\n"
              << results << std::endl;


    results = bench::benchmark (params, t_to_stl_bench);

    std::cout << "STL Conversion Benchmark:\n"
              << results << std::endl;

    return EXIT_SUCCESS;
}
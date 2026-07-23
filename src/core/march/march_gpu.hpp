/**
 * @file march_gpu.cpp
 * @brief Two-pass CUDA marching cubes.
 */

#pragma once

#include <vector>
#include <array>
#include <thread>
#include "cube_lut.hpp"
#include "core/common/form.hpp"
#include "core/common/tri.hpp"
#include "minis/utils/math_utils.hpp"

namespace mu = mutils;

trimesh_t marching_cubes_gpu (std::shared_ptr<const Form> form,
                              gu::dist_t cube_size = 1)
{
}
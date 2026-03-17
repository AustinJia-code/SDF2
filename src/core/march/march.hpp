/**
 * @file march.hpp
 * @brief Marching Cubes for isosurface extraction.
 * @cite https://www.cs.carleton.edu/cs_comps/0405/shape/marching_cubes.html
 */

#pragma once

#include <vector>
#include <array>
#include "core/form/form.hpp"
#include "cube_lut.hpp"

struct tri_t
{
    gu::vec3_t v1;
    gu::vec3_t v2;
    gu::vec3_t v3;
};

using trimesh_t = std::vector<tri_t>;

trimesh_t marching_cubes (const Form& form, gu::dist_t cube_size)
{

}
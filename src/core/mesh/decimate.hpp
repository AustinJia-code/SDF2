/**
 * @file decimate.hpp
 * @brief Decimation of triangle meshes.
 * 
 * @todo
 *      - Edge directions degrading?
 */

#pragma once

#include "core/common/tri.hpp"

const trimesh_t decimate (const trimesh_t& mesh, gu::dist_t threshold)
{
    return mesh;
}
/**
 * @file sdf2.hpp
 * @brief Include helper for all sdf2 core components.
 */

#pragma once

// Boolean operations
#include "form/boolean/union.hpp"
#include "form/boolean/intersection.hpp"
#include "form/boolean/difference.hpp"

// Primitives
#include "form/primitive/sphere.hpp"
#include "form/primitive/box.hpp"
#include "form/primitive/cylinder.hpp"

// Transforms
#include "form/transform/rotation.hpp"
#include "form/transform/translation.hpp"

// Complex
#include "form/complex/pattern.hpp"

// Marching cubes
#include "march/march.hpp"

// Form to stl helper
#include "mesh/stl.hpp"
#include "mesh/decimate.hpp"
#include "mesh/coalesce.hpp"

std::string form_to_stl (const std::string name,
                         std::shared_ptr<const Form> form,
                         gu::dist_t cube_size = 1,
                         gu::dist_t decimation_thresh = -1)
{
    trimesh_t trimesh = marching_cubes (form, cube_size);
    // trimesh = coalesce (trimesh);

    // if (decimation_thresh > 0)
    //     trimesh = decimate (trimesh, decimation_thresh);

    return trimesh_to_stl (name, trimesh);
}
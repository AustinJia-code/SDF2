/**
 * @file core.hpp
 * @brief Include helper for all core components.
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

// Form to stl helper
#include "stl.hpp"

std::string form_to_stl (const std::string name,
                         std::shared_ptr<const Form> form,
                         gu::dist_t cube_size = 1)
{
    return trimesh_to_stl (name, marching_cubes (form, cube_size));
}
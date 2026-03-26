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
#include "form/primitive/prism.hpp"

// Transforms
#include "form/transform/rotation.hpp"
#include "form/transform/translation.hpp"

// Complex
#include "form/complex/pattern.hpp"

// Marching cubes
#include "march/march.hpp"

// Form to stl helper
#include <fstream>
#include "mesh/stl.hpp"
#include "mesh/decimate.hpp"
#include "mesh/coalesce.hpp"

struct STLParams
{
    gu::dist_t cube_size            = 1;
    gu::dist_t decimation_thresh    = -1;
    bool coalesce                   = false;
    bool binary                     = true;
};

void form_to_stl (std::shared_ptr<const Form> form,
                  const std::string& path,
                  STLParams params = {},
                  const std::string& name = "")
{
    trimesh_t trimesh = marching_cubes (form, params.cube_size);

    if (params.coalesce)
        trimesh = coalesce (trimesh);

    // if (params.decimation_thresh > 0)
    //     trimesh = decimate (trimesh, params.decimation_thresh);

    if (params.binary)
    {
        auto buf = trimesh_to_stl_binary (trimesh);
        std::ofstream out (path, std::ios::binary);
        out.write (buf.data (), buf.size ());
    }
    else
    {
        std::ofstream out (path);
        out << trimesh_to_stl (name, trimesh);
    }
}
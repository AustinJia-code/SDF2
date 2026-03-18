/**
 * @file march.hpp
 * @brief Marching Cubes for isosurface extraction.
 * @cite https://www.cs.carleton.edu/cs_comps/0405/shape/marching_cubes.html
 */

#pragma once

#include <vector>
#include <array>
#include "cube_lut.hpp"
#include "common/form.hpp"
#include "common/tri.hpp"

trimesh_t marching_cubes (std::shared_ptr<const Form> form,
                          gu::dist_t cube_size = 1)
{
    trimesh_t mesh;
    gu::vec3_t min = form->bbox ().min;
    gu::vec3_t max = form->bbox ().max;

    // Make bounds divisible and add padding for easier math
    gu::vec3_t deltas = max - min;
    min.x -= std::fmod (deltas.x, cube_size) / 2 + cube_size;
    min.y -= std::fmod (deltas.y, cube_size) / 2 + cube_size;
    min.z -= std::fmod (deltas.z, cube_size) / 2 + cube_size;
    max.x += std::fmod (deltas.x, cube_size) / 2 + cube_size;
    max.y += std::fmod (deltas.y, cube_size) / 2 + cube_size;
    max.z += std::fmod (deltas.z, cube_size) / 2 + cube_size;

    // Sample and triangulate
    for (gu::dist_t x = min.x; x < max.x; x += cube_size)
    {
        for (gu::dist_t y = min.y; y < max.y; y += cube_size)
        {
            for (gu::dist_t z = min.z; z < max.z; z += cube_size)
            {
                // Sample
                int v_flags = 0;
                std::array<gu::vec3_t, 8> vert_pos;
                std::array<gu::dist_t, 8> v_sdf;

                for (int i = 0; i < 8; i++)
                {
                    vert_pos[i] = {x + v_offsets[i][0] * cube_size,
                                   y + v_offsets[i][1] * cube_size,
                                   z + v_offsets[i][2] * cube_size};

                    v_sdf[i] = form->dist (vert_pos[i]);

                    v_flags |= (v_sdf[i] > 0) << i;
                }

                // Get triangles for vertex state
                const auto& edgetris = v_to_etri[v_flags];

                auto lerp = [&] (int e)
                {
                    int va = e_to_v[e][0];
                    int vb = e_to_v[e][1];
                    float t = v_sdf[va] / (v_sdf[va] - v_sdf[vb]);
                    
                    return vert_pos[va] + (vert_pos[vb] - vert_pos[va]) * t;
                };

                for (size_t i = 0; i < edgetris.size (); i += 3)
                {
                    mesh.push_back
                    ({
                        lerp (edgetris[i]),
                        lerp (edgetris[i + 1]),
                        lerp (edgetris[i + 2])
                    }); 
                }
            }
        }
    }

    return mesh;
}
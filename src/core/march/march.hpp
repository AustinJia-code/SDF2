/**
 * @file march.hpp
 * @brief Marching Cubes for isosurface extraction.
 * @cite https://www.cs.carleton.edu/cs_comps/0405/shape/marching_cubes.html
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

trimesh_t marching_cubes (std::shared_ptr<const Form> form,
                          gu::dist_t cube_size = 1)
{
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

    // Integer cell counts along each axis
    int nx = (int) std::ceil ((max.x - min.x) / cube_size);
    int ny = (int) std::ceil ((max.y - min.y) / cube_size);
    int nz = (int) std::ceil ((max.z - min.z) / cube_size);

    unsigned int n_threads = std::thread::hardware_concurrency ();
    if (n_threads == 0)
        n_threads = 4;

    std::vector<trimesh_t> thread_meshes (n_threads);
    std::vector<std::thread> threads;
    threads.reserve (n_threads);

    for (unsigned int tid = 0; tid < n_threads; tid++)
    {
        threads.emplace_back ([&, tid]
        {
            auto& local_mesh = thread_meshes[tid];

            int x_start = (nx * tid) / n_threads;
            int x_end = (nx * (tid + 1)) / n_threads;

            // Store planes at x = xi, x=xi+1, cache the bread of the sandwich
            // Index w/ [yi * (nz + 1) + zi], nz +1 b/c holding corners
            int plane_stride = nz + 1;
            int plane_size = (ny + 1) * plane_stride;

            std::vector<gu::dist_t> cur_plane (plane_size);
            std::vector<gu::dist_t> nxt_plane (plane_size);

            auto fill_plane = [&] (std::vector<gu::dist_t>& plane, int xi)
            {
                gu::dist_t x = min.x + xi * cube_size;
                for (int yi = 0; yi <= ny; yi++)
                for (int zi = 0; zi <= nz; zi++)
                    plane[yi * plane_stride + zi] =
                        form->dist
                        ({
                            x,
                            min.y + yi * cube_size,
                            min.z + zi * cube_size
                        });
            };

            fill_plane (cur_plane, x_start);

            // March loop
            for (int xi = x_start; xi < x_end; xi++)
            {
                fill_plane (nxt_plane, xi + 1);

                gu::dist_t x = min.x + xi * cube_size;

                for (int yi = 0; yi < ny; yi++)
                for (int zi = 0; zi < nz; zi++)
                {
                    // Get cube vertices
                    int i00 =  yi      * plane_stride + zi;
                    int i10 = (yi + 1) * plane_stride + zi;
                    int i01 =  yi      * plane_stride + zi + 1;
                    int i11 = (yi + 1) * plane_stride + zi + 1;

                    std::array<gu::dist_t, 8> v_sdf =
                    {{
                        cur_plane[i00], // v0 (0,0,0)
                        nxt_plane[i00], // v1 (1,0,0)
                        nxt_plane[i10], // v2 (1,1,0)
                        cur_plane[i10], // v3 (0,1,0)
                        cur_plane[i01], // v4 (0,0,1)
                        nxt_plane[i01], // v5 (1,0,1)
                        nxt_plane[i11], // v6 (1,1,1)
                        cur_plane[i11], // v7 (0,1,1)
                    }};

                    int v_flags = 0;
                    for (int i = 0; i < 8; i++)
                        v_flags |= (v_sdf[i] > 0) << i;

                    // Build triangles
                    const auto& edgetris = v_to_etri[v_flags];
                    if (edgetris.empty ())
                        continue;

                    gu::dist_t y = min.y + yi * cube_size;
                    gu::dist_t z = min.z + zi * cube_size;

                    std::array<gu::vec3_t, 8> vert_pos;
                    for (int i = 0; i < 8; i++)
                    {
                        vert_pos[i] =
                        {
                            x + v_offsets[i][0] * cube_size,
                            y + v_offsets[i][1] * cube_size,
                            z + v_offsets[i][2] * cube_size
                        };
                    }

                    auto lerp = [&] (int e)
                    {
                        int va = e_to_v[e][0];
                        int vb = e_to_v[e][1];
                        double t = v_sdf[va] / (v_sdf[va] - v_sdf[vb]);
                        return vert_pos[va] + (vert_pos[vb] - vert_pos[va]) * t;
                    };

                    for (size_t i = 0; i < edgetris.size (); i += 3)
                    {
                        local_mesh.push_back
                        ({
                            lerp (edgetris[i]),
                            lerp (edgetris[i + 1]),
                            lerp (edgetris[i + 2])
                        });
                    }
                }

                // No need to recompute i + 1 plane
                std::swap (cur_plane, nxt_plane);
            }
        });
    }

    for (auto& t : threads)
        t.join ();

    trimesh_t mesh;
    for (auto& m : thread_meshes)
        mesh.insert (mesh.end (), m.begin (), m.end ());

    return mesh;
}
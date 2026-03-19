/**
 * @file coalesce.hpp
 * @brief Coalescing coplanar triangles in a triangle mesh.
 * 
 * @todo
 *     - Better hash function
 *     - Combine colinear triangle edges in merge
 */

#pragma once

#include <unordered_map>
#include <iostream>
#include "core/common/tri.hpp"
#include "core/common/hash.hpp"

static trimesh_t coalesce_plane (const std::vector<tri_t>& tris);

const trimesh_t coalesce (const trimesh_t& mesh)
{
    // Get coplanar triangles
    std::unordered_map<triplane_key, std::vector<tri_t>, triplane_key_hash>
        plane_map;

    for (const tri_t& tri : mesh)
        plane_map[make_plane_key (tri)].push_back (tri);

    std::cout << "Coalescing " << mesh.size () << " triangles into "
              << plane_map.size () << " planes..." << std::endl;

    // Coalesce each plane and combine results
    trimesh_t out;
    for (const auto& [key, tris] : plane_map)
    {
        trimesh_t coalesced = coalesce_plane (tris);
        out.insert (out.end (), coalesced.begin (), coalesced.end ());
    }

    std::cout << "Coalesced into " << out.size () << " triangles." << std::endl;

    return out;
}

static trimesh_t coalesce_plane (const std::vector<tri_t>& tris)
{
    // Count edges
    std::unordered_map<edge_t, int, edge_hash, edge_approx> edge_count;

    for (const tri_t& tri : tris)
    {
        auto add_edge = [&](gu::vec3_t a, gu::vec3_t b)
        {
            if (a > b) std::swap (a, b);
            edge_count[{a,b}]++;
        };

        add_edge (tri.v1, tri.v2);
        add_edge (tri.v2, tri.v3);
        add_edge (tri.v3, tri.v1);
    }

    // Get boundary loop, boundary edges will only appear once
    std::unordered_map<gu::vec3_t, std::vector<gu::vec3_t>,
                       vec3_hash, vec3_approx> adj;

    for (auto& [edge, count] : edge_count)
    {
        if (count == 1)
        {
            adj[edge.first].push_back (edge.second);
            adj[edge.second].push_back (edge.first);
        }
    }

    // Walk the loop
    std::vector<gu::vec3_t> loop;
    gu::vec3_t prev  = adj.begin()->first;
    gu::vec3_t start = prev;
    gu::vec3_t cur   = adj[prev][0];

    loop.push_back (prev);

    while (cur != start)
    {
        if (loop.size () > adj.size ())
         return tris;

        // Check if we can merge
        if (loop.size () >= 2)
        {
            gu::vec3_t prev_slope = gu::norm (loop[loop.size () - 1] 
                                            - loop[loop.size () - 2]);
            gu::vec3_t cur_slope  = gu::norm (cur - loop[loop.size () - 1]) ;
            if (gu::approx (prev_slope, cur_slope))
                loop.back () = cur;
            else
                loop.push_back (cur);
        }
        else
        {
            loop.push_back (cur);
        }

        // Of the two neighbours of cur, take the one we didn't come from
        const auto& neighbours = adj[cur];
        if (neighbours.size () < 2)
            return tris;

        gu::vec3_t next = (neighbours[0] != prev)
                            ? neighbours[0]
                            : neighbours[1];
        prev = cur;
        cur = next;
    }

    // Fan triangulation from first vertex
    trimesh_t out;

    for (size_t i = 1; i < loop.size () - 1; i++)
        out.push_back ({loop[0], loop[i], loop[i + 1]});

    return out;
}
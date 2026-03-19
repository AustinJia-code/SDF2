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

inline void canonicalize (gu::vec3_t& n, gu::dist_t& d)
{
    if (n.x < 0 || (n.x == 0 && (n.y < 0 || (n.y == 0 && n.z < 0))))
    {
        n = n * -1;
        d = -d;
    }
}

struct plane_key
{
    int64_t nx, ny, nz, d;

    bool operator== (const plane_key& other) const
    {
        return nx == other.nx
            && ny == other.ny
            && nz == other.nz
            && d == other.d;
    }
};

struct plane_key_hash
{
    std::size_t operator() (const plane_key& k) const
    {
        std::size_t h = 0;
        h ^= std::hash<int64_t> {} (k.nx) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int64_t> {} (k.ny) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int64_t> {} (k.nz) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int64_t> {} (k.d)  + 0x9e3779b9 + (h << 6) + (h >> 2);

        return h;
    }
};

plane_key make_plane_key (const tri_t& tri)
{
    gu::vec3_t n = get_tri_normal(tri);
    gu::dist_t d = gu::dot (n * -1, tri.v1);

    canonicalize(n, d);

    return
    {
        mu::quantize (n.x),
        mu::quantize (n.y),
        mu::quantize (n.z),
        mu::quantize (d)
    };
}

const trimesh_t coalesce (const trimesh_t& mesh)
{
    // Get coplanar triangles
    std::unordered_map<plane_key, std::vector<tri_t>, plane_key_hash> plane_map;
    for (const tri_t& tri : mesh)
        plane_map[make_plane_key (tri)].push_back (tri);

    // Merge coplanar triangles

    return mesh;
}
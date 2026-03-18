/**
 * @file stl.hpp
 * @brief STL file export.
 * @cite https://www.cs.carleton.edu/cs_comps/0405/shape/decimation.html
 */

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "march/march.hpp"

/**
 * Decimation helper
 */
static const trimesh_t decimate (const trimesh_t& mesh, gu::dist_t threshold);

/**
 * Convert trimesh to STL format string
 */
std::string trimesh_to_stl (const std::string name,
                            const trimesh_t& mesh,
                            const gu::dist_t decimation_thresh = 0)
{
    std::string stl = "solid " + name + "\n";

    // Decimate, avoid copy if no decimation needed
    trimesh_t decimated_mesh =
        (decimation_thresh > 0)
            ? decimate (mesh, decimation_thresh)
            : mesh;

    for (const auto& tri : decimated_mesh)
    {
        gu::vec3_t normal = norm (cross (tri.v2 - tri.v1, tri.v3 - tri.v1));
        stl += "  facet normal " + std::to_string (normal.x) + " " 
                                 + std::to_string (normal.y) + " " 
                                 + std::to_string (normal.z) + "\n";
        stl += "    outer loop\n";
        stl += "      vertex " + std::to_string (tri.v1.x) + " " 
                               + std::to_string (tri.v1.y) + " "
                               + std::to_string (tri.v1.z) + "\n";
        stl += "      vertex " + std::to_string (tri.v2.x) + " " 
                               + std::to_string (tri.v2.y) + " " 
                               + std::to_string (tri.v2.z) + "\n";
        stl += "      vertex " + std::to_string (tri.v3.x) + " " 
                               + std::to_string (tri.v3.y) + " "
                               + std::to_string (tri.v3.z) + "\n";
        stl += "    endloop\n";
        stl += "  endfacet\n";
    }
    
    stl += "endsolid\n";
    return stl;
}

/**
 * Define some hashes for map use
 * TODO: fix hash func it's kind of chopped... 
 */
struct Edge
{
    gu::vec3_t v1;
    gu::vec3_t v2;

    bool operator== (const Edge& other) const
    {
        return (v1 == other.v1 && v2 == other.v2)
            || (v1 == other.v2 && v2 == other.v1);
    }
};

namespace std
{
    template <>
    struct hash<gu::vec3_t>
    {
        std::size_t operator () (const gu::vec3_t& v) const noexcept
        {
            std::size_t h1 = std::hash<gu::dist_t> {} (v.x);
            std::size_t h2 = std::hash<gu::dist_t> {} (v.y);
            std::size_t h3 = std::hash<gu::dist_t> {} (v.z);

            return h1
                 & h2 << sizeof (gu::dist_t)
                 & h3 << (sizeof (gu::dist_t) << 1); 
        }
    };

    template <>
    struct hash<Edge>
    {
        std::size_t operator()(const Edge& e) const noexcept
        {
            return hash<gu::vec3_t>{} (e.v1) ^ hash<gu::vec3_t>{} (e.v2);
        }
    };
}

/**
 * TODO: Currently only handles face vertices, handle boundary vertices as well.
 * TODO: Inefficient
 * TODO: Recursive loop closure
 */
static const trimesh_t decimate (const trimesh_t& mesh, gu::dist_t threshold)
{
    if (threshold <= 0)
        return mesh;

    // Find neighbor info for each vertex
    struct neighbor_info
    {
        gu::vec3_t tri_normal_sum;
        gu::vec3_t neighbor_pos_sum;
        size_t tri_count;
        std::unordered_set<size_t> tri_indices;

        void update (gu::vec3_t normal,
                     gu::vec3_t nbr_a, gu::vec3_t nbr_b,
                     size_t tri_index)
        {
            tri_normal_sum += normal;
            neighbor_pos_sum += nbr_a + nbr_b;
            tri_indices.insert (tri_index);
            tri_count++;
        }
    };

    std::unordered_map<gu::vec3_t, neighbor_info> v_to_info;
    for (size_t i = 0; i < mesh.size (); i++)
    {
        const auto& tri = mesh[i];

        gu::vec3_t normal = norm (cross (tri.v2 - tri.v1, tri.v3 - tri.v1));
        v_to_info[tri.v1].update (normal, tri.v2, tri.v3, i);
        v_to_info[tri.v2].update (normal, tri.v1, tri.v3, i);
        v_to_info[tri.v3].update (normal, tri.v1, tri.v2, i);
    }

    // Calculate error, mark vertices to be removed
    std::unordered_set<gu::vec3_t> remove_vs;
    for (const auto& [v, info] : v_to_info)
    {
        // Define plane of neighbor vertices
        gu::vec3_t avg_normal = gu::norm (info.tri_normal_sum / info.tri_count);
        gu::vec3_t avg_pos = info.neighbor_pos_sum / (info.tri_count * 2);

        // p in plane if (N * p + D = 0)
        gu::dist_t error = std::abs (gu::dot (v - avg_pos, avg_normal));

        if (error < threshold)
            remove_vs.insert (v);
    }

    // Mark triangles if all vertices can be removed
    std::unordered_set<size_t> remove_tri_indices;
    for (size_t i = 0; i < mesh.size (); i++)
    {
        const auto& tri = mesh[i];
        if (remove_vs.find (tri.v1) != remove_vs.end ()
         && remove_vs.find (tri.v2) != remove_vs.end ()
         && remove_vs.find (tri.v3) != remove_vs.end ())
        {
            remove_tri_indices.insert (i);
        }
    }

    // Create decimated mesh without marked triangles
    trimesh_t decimated_mesh;
    for (size_t i = 0; i < mesh.size (); i++)
    {
        if (remove_tri_indices.find (i) == remove_tri_indices.end ())
            decimated_mesh.push_back (mesh[i]);
    }    

    // ID hole edges
    std::unordered_map<Edge, int> edge_count;
    for (const auto& tri : decimated_mesh)
    {
        edge_count[{tri.v1, tri.v2}]++;
        edge_count[{tri.v2, tri.v3}]++;
        edge_count[{tri.v3, tri.v1}]++;
    }

    std::vector<Edge> boundary_edges;
    for (const auto& [e, count] : edge_count)
        if (count == 1)
            boundary_edges.push_back (e);

    // Chain edges into ordered loops
    std::vector<std::vector<gu::vec3_t>> loops;
    std::unordered_set<Edge> remaining (boundary_edges.begin (),
                                        boundary_edges.end ());

    while (!remaining.empty ())
    {
        std::vector<gu::vec3_t> loop;
        auto start = *remaining.begin ();
        remaining.erase (start);
        loop.push_back (start.v1);
        gu::vec3_t cur = start.v2;

        while (cur != loop[0])
        {
            loop.push_back (cur);
            // Find next edge starting at cur
            auto it = std::find_if (remaining.begin (), remaining.end (),
                [&](const Edge& e)
                {
                    return e.v1 == cur || e.v2 == cur;
                });

            cur = (it->v1 == cur) ? it->v2 : it->v1;
            remaining.erase (it);
        }

        loops.push_back (loop);
    }

    // Close loops
    for (const auto& loop : loops)
    {
        gu::vec3_t center;
        for (const auto& v : loop)
            center += v;
        center = center / loop.size ();

        for (size_t i = 0; i < loop.size (); i++)
            decimated_mesh.push_back ({center, loop[i],
                                       loop[(i+1) % loop.size ()]});
    }

    return decimated_mesh;
}
/**
 * @file coalesce.hpp
 * @brief Coalescing coplanar triangles in a triangle mesh.
 */

#pragma once

#include <unordered_map>
#include <utility>
#include <iostream>
#include "core/common/tri.hpp"
#include "core/common/hash.hpp"

using adj_map_t = std::unordered_map<gu::vec3_t, std::vector<gu::vec3_t>,
                                     vec3_hash, vec3_approx>;

/**
 * Snap all triangle vertices to a quantization grid.
 */
static std::vector<tri_t> snap_tris (const std::vector<tri_t>& tris)
{
    static constexpr double EPS = 1e-4;

    auto snap   = [] (double x)
    {
        return std::round (x / EPS) * EPS;
    };

    auto snap_v = [&] (gu::vec3_t v) -> gu::vec3_t
    {
        return
        {
            snap (v.x),
            snap (v.y),
            snap (v.z)
        };
    };

    std::vector<tri_t> out;

    out.reserve (tris.size ());

    for (const tri_t& t : tris)
    {
        out.push_back
        ({
            snap_v (t.v1),
            snap_v (t.v2),
            snap_v (t.v3)
        });
    }

    return out;
}

/**
 * Build adjacency map of boundary edges
 */
static adj_map_t boundary_adj (const std::vector<tri_t>& tris)
{
    std::unordered_map<edge_t, int, edge_hash, edge_approx> edge_count;

    for (const tri_t& tri : tris)
    {
        auto add_edge = [&] (gu::vec3_t a, gu::vec3_t b)
        {
            if (a > b) std::swap (a, b);
            edge_count[{a, b}]++;
        };

        add_edge (tri.v1, tri.v2);
        add_edge (tri.v2, tri.v3);
        add_edge (tri.v3, tri.v1);
    }

    adj_map_t adj;
    for (const auto& [edge, count] : edge_count)
    {
        if (count == 1)
        {
            adj[edge.first].push_back (edge.second);
            adj[edge.second].push_back (edge.first);
        }
    }

    return adj;
}

/**
 * Walk a boundary adjacency map to produce an ordered loop of vertices.
 * Returns empty on failure.
 */
static std::vector<gu::vec3_t> walk_loop (const adj_map_t& adj)
{
    std::vector<gu::vec3_t> loop;
    gu::vec3_t start = adj.begin ()->first;
    gu::vec3_t prev  = start;

    if (adj.at (start).empty ())
        return {};

    gu::vec3_t cur = adj.at (start)[0];
    loop.push_back (start);

    while (cur != start)
    {
        if (loop.size () > adj.size () + 10)
        {
            // std::cout << "Error: loop size exceeded adjacency size" << std::endl;
            return {};
        }

        // Merge collinear edges
        if (loop.size () >= 2)
        {
            gu::vec3_t ps = gu::norm (loop[loop.size () - 1]
                                    - loop[loop.size () - 2]);
            gu::vec3_t cs = gu::norm (cur - loop[loop.size () - 1]);

            if (gu::approx (ps, cs))
                loop.back () = cur;
            else
                loop.push_back (cur);
        }
        else
        {
            loop.push_back (cur);
        }

        if (adj.find (cur) == adj.end ())
        {
            // std::cout << "Error: cur not in adjacency map" << std::endl;
            return {};
        }

        const auto& nb = adj.at (cur);
        if (nb.size () < 2)
        {
            // std::cout << "Error: not enough neighbours" << std::endl;
            return {};
        }

        gu::vec3_t next = (nb[0] != prev) ? nb[0] : nb[1];
        prev = cur;
        cur  = next;
    }

    return loop;
}

/**
 * Ear-clipping triangulation of a 3D polygon loop.
 * Projects to 2D via the face normal, handles concave polygons (theoretically).
 */
static trimesh_t ear_clip (const std::vector<gu::vec3_t>& loop,
                           const gu::vec3_t& normal)
{
    using vec2_t = std::pair<double, double>;

    gu::vec3_t arb = (std::abs (normal.x) < 0.9)
                    ? gu::vec3_t {1, 0, 0} : gu::vec3_t {0, 1, 0};
    gu::vec3_t bu = gu::norm (gu::cross (normal, arb));
    gu::vec3_t bv = gu::cross (normal, bu);

    std::vector<vec2_t> poly;
    poly.reserve (loop.size ());
    for (const auto& p : loop)
        poly.push_back ({gu::dot (p, bu), gu::dot (p, bv)});

    // Signed area to winding direction
    double area = 0;
    for (size_t i = 0; i < poly.size (); i++)
    {
        const auto& a = poly[i];
        const auto& b = poly[(i + 1) % poly.size ()];
        area += a.first * b.second - b.first * a.second;
    }
    double wind = (area >= 0) ? 1.0 : -1.0;

    auto tri_area2d = [] (vec2_t a, vec2_t b, vec2_t c) -> double
    {
        return (b.first  - a.first)  * (c.second - a.second)
             - (b.second - a.second) * (c.first  - a.first);
    };

    auto in_tri2d = [&] (vec2_t a, vec2_t b, vec2_t c, vec2_t p) -> bool
    {
        double d1 = tri_area2d (a, b, p);
        double d2 = tri_area2d (b, c, p);
        double d3 = tri_area2d (c, a, p);

        return !((d1 < 0 || d2 < 0 || d3 < 0) && (d1 > 0 || d2 > 0 || d3 > 0));
    };

    auto orient = [&](tri_t& tri)
    {
        if (gu::dot (get_tri_normal (tri), normal) < 0)
            std::swap (tri.v2, tri.v3);
    };

    std::vector<size_t> idx;
    idx.reserve (loop.size ());
    for (size_t i = 0; i < loop.size (); i++)
        idx.push_back (i);

    trimesh_t out;
    out.reserve (loop.size () > 2 ? loop.size () - 2 : 0);

    // Find and clip ears
    while (idx.size () > 3)
    {
        bool found = false;

        for (size_t i = 0; i < idx.size (); i++)
        {
            size_t ia = idx[(i + idx.size () - 1) % idx.size ()];
            size_t ib = idx[i];
            size_t ic = idx[(i + 1) % idx.size ()];

            if (tri_area2d (poly[ia], poly[ib], poly[ic]) * wind <= 0)
                continue;

            bool is_ear = true;
            for (size_t j = 0; j < idx.size () && is_ear; j++)
            {
                if (idx[j] == ia || idx[j] == ib || idx[j] == ic) continue;
                if (in_tri2d (poly[ia], poly[ib], poly[ic], poly[idx[j]]))
                    is_ear = false;
            }

            if (is_ear)
            {
                tri_t tri {loop[ia], loop[ib], loop[ic]};
                orient (tri);
                out.push_back (tri);
                idx.erase (idx.begin () + i);
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cout << "Error: ear-clipping found no ear" << std::endl;
            return {};
        }
    }

    tri_t last {loop[idx[0]], loop[idx[1]], loop[idx[2]]};
    orient (last);
    out.push_back (last);

    return out;
}

/**
 * Plane helper
 */
static const trimesh_t coalesce_plane (const std::vector<tri_t>& tris)
{
    if (tris.size () == 1)
        return tris;

    std::vector<tri_t>     snapped = snap_tris (tris);
    adj_map_t              adj     = boundary_adj (snapped);

    if (adj.empty ())
        return {};

    std::vector<gu::vec3_t> loop = walk_loop (adj);

    if (loop.size () < 3)
        return tris;

    gu::vec3_t normal = {0, 0, 0};
    for (const tri_t& t : snapped)
    {
        normal = get_tri_normal (t);
        if (gu::mag (normal) > 0.5) break;
    }

    trimesh_t out = ear_clip (loop, normal);
    return (!out.empty () && out.size () < tris.size ()) ? out : tris;
}

/**
 * Coalesce a mesh
 */
const trimesh_t coalesce (const trimesh_t& mesh)
{
    std::unordered_map<triplane_key, std::vector<tri_t>, triplane_key_hash>
        plane_map;

    for (const tri_t& tri : mesh)
        plane_map[make_plane_key (tri)].push_back (tri);

    std::cout << "Coalescing " << mesh.size () << " triangles into "
              << plane_map.size () << " planes..." << std::endl;

    trimesh_t out;
    for (const auto& [key, tris] : plane_map)
    {
        trimesh_t coalesced = coalesce_plane (tris);
        out.insert (out.end (), coalesced.begin (), coalesced.end ());
    }

    std::cout << "Coalesced into " << out.size () << " triangles." << std::endl;

    return out;
}

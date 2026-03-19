/**
 * @file hash.hpp
 * @brief Hash utilities for various data structures.
 */

#pragma once

#include <cstdint>
#include "core/common/tri.hpp"
#include "minis/utils/math_utils.hpp"

namespace mu = mutils;

// From boost::hash_combine
template <typename T>
std::size_t hash_combine (std::vector<T> v)
{
    std::size_t h = 0;
    for (const T& elem : v)
        h ^= std::hash<T> {} (elem) + 0x9e3779b9 + (h << 6) + (h >> 2);
        
    return h;
}

/************* TRIPLANE HASHING *************/
struct triplane_key
{
    int64_t nx, ny, nz, d;

    bool operator== (const triplane_key& other) const
    {
        return nx == other.nx
            && ny == other.ny
            && nz == other.nz
            && d  == other.d;
    }
};

struct triplane_key_hash
{
    std::size_t operator() (const triplane_key& k) const
    {
        std::size_t h = 0;

        return hash_combine (std::vector<int64_t> {k.nx, k.ny, k.nz, k.d});
    }
};

triplane_key make_plane_key (const tri_t& tri)
{
    gu::vec3_t n = get_tri_normal (tri);
    gu::dist_t d = gu::dot (n * -1, tri.v1);

    return
    {
        mu::quantize (n.x),
        mu::quantize (n.y),
        mu::quantize (n.z),
        mu::quantize (d)
    };
}

/************* EDGE_T HASHING *************/
using edge_t = std::pair<gu::vec3_t, gu::vec3_t>;

struct edge_hash
{
    std::size_t operator() (const edge_t& e) const
    {
        return hash_combine (std::vector<int64_t>
        {
            mu::quantize (e.first.x),  mu::quantize (e.first.y),
            mu::quantize (e.first.z),  mu::quantize (e.second.x),
            mu::quantize (e.second.y), mu::quantize (e.second.z)
        });
    }
};

struct edge_approx
{
    bool operator() (const edge_t& a, const edge_t& b) const
    {
        return mu::quantize (a.first.x)  == mu::quantize (b.first.x)
            && mu::quantize (a.first.y)  == mu::quantize (b.first.y)
            && mu::quantize (a.first.z)  == mu::quantize (b.first.z)
            && mu::quantize (a.second.x) == mu::quantize (b.second.x)
            && mu::quantize (a.second.y) == mu::quantize (b.second.y)
            && mu::quantize (a.second.z) == mu::quantize (b.second.z);
    }
};

/************* VEC3_T HASHING *************/
struct vec3_hash
{
    std::size_t operator() (const gu::vec3_t& v) const
    {
        return hash_combine (std::vector<int64_t>
        {
            mu::quantize (v.x), mu::quantize (v.y), mu::quantize (v.z)
        });
    }
};

struct vec3_approx
{
    bool operator() (const gu::vec3_t& a, const gu::vec3_t& b) const
    {
        return mu::quantize (a.x) == mu::quantize (b.x)
            && mu::quantize (a.y) == mu::quantize (b.y)
            && mu::quantize (a.z) == mu::quantize (b.z);
    }
};
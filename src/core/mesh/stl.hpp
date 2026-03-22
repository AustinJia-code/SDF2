/**
 * @file stl.hpp
 * @brief STL file export.
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include "core/common/tri.hpp"

/**
 * Convert trimesh to STL format string
 */
std::string trimesh_to_stl (const std::string name, const trimesh_t& mesh)
{
    std::string stl = "solid " + name + "\n";

    for (const auto& tri : mesh)
    {
        gu::vec3_t normal = get_tri_normal (tri);

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
 * Convert trimesh to binary STL buffer.
 * Format: 80B header | uint32 count | per tri: float[3] normal,
 *                                              3x float[3] vert, uint16 attrib
 */
std::vector<char> trimesh_to_stl_binary (const trimesh_t& mesh)
{
    std::vector<char> buf (80 + 4 + mesh.size () * 50, 0);
    char* p = buf.data ();

    // 80B Header
    p += 80;

    // Triangle count
    uint32_t count = (uint32_t) mesh.size ();
    std::memcpy (p, &count, 4);
    p += 4;

    for (const auto& tri : mesh)
    {
        gu::vec3_t n = get_tri_normal (tri);

        auto write_vec = [&] (const gu::vec3_t& v)
        {
            float xyz[3] =
            {
                (float) v.x,
                (float) v.y,
                (float) v.z
            };
            
            std::memcpy (p, xyz, 12);
            p += 12;
        };

        write_vec (n);
        write_vec (tri.v1);
        write_vec (tri.v2);
        write_vec (tri.v3);

        // 2B Attribute
        p += 2;
    }

    return buf;
}
/**
 * @file stl.hpp
 * @brief STL file export.
 */

#pragma once

#include <string>
#include "common/tri.hpp"

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
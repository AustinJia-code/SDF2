/**
 * @file tri.hpp
 * @brief Triangle mesh utilities.
 */

#pragma once

#include <vector>
#include "minis/utils/geo_utils.hpp"

namespace gu = gutils;

struct tri_t
{
    gu::vec3_t v1;
    gu::vec3_t v2;
    gu::vec3_t v3;
};

/**
 * Get tri normal
 */
const gu::vec3_t get_tri_normal (const tri_t& t)
{
    return gu::norm (gu::cross (t.v2 - t.v1, t.v3 - t.v1));
}

using trimesh_t = std::vector<tri_t>;
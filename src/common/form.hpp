/**
 * @file form.hpp
 * @brief Abstract form type for both primitives and composites.
 */

#pragma once

#include <memory>
#include "minis/utils/geo_utils.hpp"

namespace gu = gutils;

struct BoundingBox
{
    gu::vec3_t min;
    gu::vec3_t max;
};

class Form
{
public:
    virtual ~Form () = default;
    virtual gu::dist_t dist (const gu::vec3_t& p) const = 0;
    virtual BoundingBox bbox () const = 0;
};

using FormPtr = std::shared_ptr<Form>;
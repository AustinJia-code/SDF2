/**
 * @file form.hpp
 * @brief Abstract form type for both primitives and composites.
 */

#pragma once

#include <memory>
#include "minis/utils/geo_utils.hpp"

namespace gu = gutils;
using FormPtr = std::shared_ptr<Form>;

class Form
{
public:
    virtual ~Form () = default;
    virtual gu::dist_t dist (const gu::vec3_t& p) const = 0;
};
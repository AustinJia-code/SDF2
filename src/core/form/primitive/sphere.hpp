/**
 * @file sphere.hpp
 * @brief Sphere primitive.
 */

#pragma once

#include "core/common/form.hpp"

/**
 * Sphere
 */
class Sphere : public Form
{
private:
    gu::dist_t r;

public:
    Sphere (gu::dist_t r_) : r (r_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        return gu::mag (p) - r;
    }

    BoundingBox bbox () const override
    {
        return {{-r, -r, -r},
                {r, r, r}};
    }
};

/**
 * Make a sphere form with radius r.
 */
FormPtr make_sphere (gu::dist_t r)
{
    return std::make_shared<Sphere> (r);
}
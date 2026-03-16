/**
 * @file sphere.hpp
 * @brief Sphere primitive.
 */

#pragma once

#include "core/form/form.hpp"

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
        return p.mag () - r;
    }
};

/**
 * Make a sphere form with radius r.
 */
FormPtr sphere (gu::dist_t r)
{
    return std::make_shared<Sphere> (r);
}
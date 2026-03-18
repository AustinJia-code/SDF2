/**
 * @file cylinder.hpp
 * @brief Cylinder primitive.
 */

#pragma once

#include "common/form.hpp"

/**
 * Cylinder
 */
class Cylinder : public Form
{
private:
    gu::dist_t r;
    gu::dist_t h;

public:
    Cylinder (gu::dist_t r_, gu::dist_t h_) : r (r_), h (h_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        gu::dist_t dr = std::sqrt (p.x * p.x + p.y * p.y) - r;
        gu::dist_t dz = std::abs (p.z) - h / 2;

        return std::min  (std::max (dr, dz), 0.0) +
               std::sqrt (std::pow (std::max (dr, 0.0), 2) +
                          std::pow (std::max (dz, 0.0), 2));
    }

    BoundingBox bbox () const override
    {
        return {{-r, -r, -h / 2},
                {r, r, h / 2}};
    }
};

/**
 * Make a vertical cylinder form with height h and radius r.
 */
FormPtr make_cylinder (gu::dist_t r, gu::dist_t h)
{
    return std::make_shared<Cylinder> (r, h);
}
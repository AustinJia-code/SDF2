/**
 * @file prism.hpp
 * @brief Generic regular prism generator.
 */

#pragma once

#include "core/common/form.hpp"

/**
 * Generic regular prism
 */
class Prism : public Form
{
private:
    size_t n;              // number of sides
    gu::dist_t r;          // circumradius
    gu::dist_t h;          // height

    /**
     * Distance from 2d polygon
     */
    gu::dist_t dist_2d (const gu::vec3_t& q) const
    {
        gu::dist_t an = M_PI / n;
        gu::dist_t acs_x = std::cos (an);
        gu::dist_t acs_y = std::sin (an);

        // Reduce to first sector using positive modulo
        gu::dist_t a = std::atan2 (q.x, q.y);
        gu::dist_t bn = a - 2.0 * an * std::floor (a / (2.0 * an)) - an;

        gu::dist_t len = std::sqrt (q.x * q.x + q.y * q.y);
        gu::dist_t px = len * std::cos (bn) - r * acs_x;
        gu::dist_t py = std::fabs (len * std::sin (bn)) - r * acs_y;

        py += std::clamp (-py, 0.0, r * acs_y);

        return std::sqrt (px * px + py * py) * (px >= 0 ? 1.0 : -1.0);
    }

public:
    Prism (size_t n, gu::dist_t r, gu::dist_t h) : n (n), r (r), h (h) {}

    gu::dist_t dist (const gu::vec3_t& q) const override
    {
        gu::dist_t dxy = dist_2d (q);
        gu::dist_t dz = std::fabs (q.z) - h / 2;

        // Case 1: inside
        if (dxy < 0 && dz < 0)
            return std::max (dxy, dz);
        // Case 2: outside in xy but inside in z
        else if (dxy > 0 && dz < 0)
            return dxy;
        // Case 3: inside in xy but outside in z
        else if (dxy < 0 && dz > 0)
            return dz;
        // Case 4: outside in both xy and z
        else if (dxy > 0 && dz > 0)
            return std::sqrt (dxy * dxy + dz * dz);
    
        // Should never reach
        return 0.0;
    }

    BoundingBox bbox () const override
    {
        // Overestimate, too lazy to figure out the actual... 
        return
        {
            {-r, -r, -h / 2},
            { r,  r,  h / 2}
        };
    }
};

/**
 * Make a generic regular circumscribed prism
 */
FormPtr make_circprism (size_t n, gu::dist_t r, gu::dist_t h)
{
    return std::make_shared<Prism> (n, r, h);
}

/**
 * Make a generic regular inscribed prism
 */
FormPtr make_inscprism (size_t n, gu::dist_t apothem, gu::dist_t h)
{
    gu::dist_t r = apothem / std::cos (M_PI / n);
    return std::make_shared<Prism> (n, r, h);
}

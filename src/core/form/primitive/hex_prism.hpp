/**
 * @file hex_prism.hpp
 * @brief Hexagonal prism primitive.
 */

#pragma once

#include "core/common/form.hpp"

/**
 * Hexagonal prism
 */
class HexPrism : public Form
{
private:
    gu::vec3_t p;
    gu::dist_t h;

public:
    HexPrism (gu::vec3_t p_, gu::dist_t h_) : p (p_), h (h_) {}

    gu::dist_t dist (const gu::vec3_t& q) const override
    {
        const gu::dist_t kx = -0.8660254;   // -sqrt (3) / 2
        const gu::dist_t ky = 0.5;
        const gu::dist_t kz = 0.57735027;   // 1 / sqrt (3)

        gu::dist_t ax = std::abs (q.x);
        gu::dist_t ay = std::abs (q.y);
        gu::dist_t az = std::abs (q.z);

        // fold into first sextant
        gu::dist_t d = kx * ax + ky * ay;
        if (d < 0.0)
        {
            ax -= 2.0 * d * kx;
            ay -= 2.0 * d * ky;
        }

        // 2D distance from hex edge, signed
        gu::dist_t cx = std::clamp (ax, -kz * p.x, kz * p.x);
        gu::dist_t dx = std::sqrt ((ax - cx) * (ax - cx)
                                 + (ay - p.x) * (ay - p.x))
                      * (ay < p.x ? -1.0 : 1.0);
        gu::dist_t dz = az - p.z;

        return std::min  (std::max (dx, dz), 0.0) +
               std::sqrt (std::pow (std::max (dx, 0.0), 2) +
                          std::pow (std::max (dz, 0.0), 2));
    }

    BoundingBox bbox () const override
    {
        return
        {
            {-p.x, -p.x, -p.z},
            { p.x,  p.x,  p.z}
        };
    }
};

/**
 * Make a hexagonal prism form with circumradius p and height h
 */
FormPtr make_hexprism (gu::dist_t p, gu::dist_t h)
{
    return std::make_shared<HexPrism> (gu::vec3_t (p / 2, p / 2, h / 2), h);
}

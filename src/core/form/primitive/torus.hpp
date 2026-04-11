/**
 * @file torus.hpp
 * @brief Torus primitive.
 */

#pragma once

#include "core/common/form.hpp"

/**
 * Torus
 */
class Torus : public Form
{
private:
    gu::dist_t ma_r;
    gu::dist_t mi_r;

public:
    /**
     * 
     */
    Torus (gu::dist_t ma_r, gu::dist_t mi_r) : ma_r (ma_r), mi_r (mi_r) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
       gu::vec3_t q = 
       {
            // length p.xz - major radius
            gu::mag (gu::vec3_t (p.x, p.z, 0)) - ma_r,
            // p.y
            p.y,
            0
       };

       return gu::mag (q) - mi_r;
    }

    BoundingBox bbox () const override
    {
        return
        {
            gu::vec3_t (ma_r + mi_r, mi_r, ma_r + mi_r) * -1,
            gu::vec3_t (ma_r + mi_r, mi_r, ma_r + mi_r)
        };
    }
};

/**
 * Make a Torus form with major radius ma_r, minor radius mi_r
 */
FormPtr make_torus (gu::dist_t ma_r, gu::dist_t mi_r)
{
    return std::make_shared<Torus> (ma_r, mi_r);
}
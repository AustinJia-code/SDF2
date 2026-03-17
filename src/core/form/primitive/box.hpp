/**
 * @file box.hpp
 * @brief Box primitive.
 */

#pragma once

#include "core/form/form.hpp"

/**
 * Box
 */
class Box : public Form
{
private:
    gu::vec3_t b;
    gu::dist_t r;

public:
    /**
     * b is half the distance the box extends in each direction
     */
    Box (gu::vec3_t b_, gu::dist_t r_ = 0) : b (gu::abs (b_)), r (r_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        // TODO add r for smooth box
        gu::vec3_t q = gu::abs (p) - b;
        return gu::mag (gu::max (q, {0.0})) + 
               std::min (std::max (q.x, std::max (q.y, q.z)), 0.0);
    }

    BoundingBox bbox () const override
    {
        return {b * -1, b};
    }
};

/**
 * Make a box form with dimensions l, w, h == x, y, z
 */
FormPtr make_box (gu::dist_t l, gu::dist_t w, gu::dist_t h, gu::dist_t r = 0)
{
    return std::make_shared<Box> (gu::vec3_t (l / 2, w / 2, h / 2), r);
}
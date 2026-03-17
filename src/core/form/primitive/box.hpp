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

public:
    /**
     * b is half the distance the box extends in each direction
     */
    Box (gu::vec3_t b_) : b (gu::abs (b_)) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
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
 * Make a box form with half-extents b.
 */
FormPtr make_box (gu::vec3_t b)
{
    return std::make_shared<Box> (b);
}
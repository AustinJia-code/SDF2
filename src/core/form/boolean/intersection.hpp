/**
 * @file intersection.hpp
 * @brief Intersection form operation.
 */

#pragma once

#include "common/form.hpp"

/**
 * Intersection
 */
class Intersection : public Form
{
private:
    FormPtr a;
    FormPtr b;
    gu::dist_t k;

public:
    Intersection (FormPtr a_, FormPtr b_, gu::dist_t k_ = 0)
        : a (a_), b (b_), k (k_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        if (k == 0)
            return std::max (a->dist (p), b->dist (p));

        gu::dist_t a_sdf = a->dist (p);
        gu::dist_t b_sdf = b->dist (p);

        gu::dist_t h = std::max (k - std::abs (a_sdf - b_sdf), 0.0) / k;

        return std::max (a_sdf, b_sdf) + h * h * k * (1.0 / 4.0);
    }

    BoundingBox bbox () const override
    {
        return {gu::max (a->bbox ().min, b->bbox ().min),
                gu::min (a->bbox ().max, b->bbox ().max)};
    }
};

/**
 * Build an intersection form of a and b.
 */
FormPtr build_intersection (FormPtr a, FormPtr b, gu::dist_t k = 0)
{
    return std::make_shared<Intersection> (a, b, k);
}

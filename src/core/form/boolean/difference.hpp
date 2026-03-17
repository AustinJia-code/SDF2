/**
 * @file difference.hpp
 * @brief Difference form operation.
 */

#pragma once

#include "core/form/form.hpp"

/**
 * Difference
 */
class Difference : public Form
{
private:
    FormPtr a;
    FormPtr b;
    gu::dist_t k;

public:
    Difference (FormPtr a_, FormPtr b_, gu::dist_t k_ = 0)
        : a (a_), b (b_), k (k_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        if (k == 0)
            return std::max (a->dist (p), -b->dist (p));

        gu::dist_t a_sdf = a->dist (p);
        gu::dist_t b_sdf = b->dist (p);

        gu::dist_t h = std::max (k - std::abs (a_sdf + b_sdf), 0.0) / k;
                                 
        return std::max (a_sdf, -b_sdf) + h * h * k * (1.0 / 4.0);
    }

    BoundingBox bbox () const override
    {
        return a->bbox ();
    }
};

/**
 * Build a difference form of a and b.
 */
FormPtr build_difference (FormPtr a, FormPtr b, gu::dist_t k = 0)
{
    return std::make_shared<Difference> (a, b, k);
}
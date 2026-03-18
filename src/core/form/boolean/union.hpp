/**
 * @file union.hpp
 * @brief Union form operation.
 */

#pragma once

#include "common/form.hpp"

/**
 * Union
 */
class Union : public Form
{
private:
    FormPtr a;
    FormPtr b;
    gu::dist_t k;

public:
    Union (FormPtr a_, FormPtr b_, gu::dist_t k_ = 0)
        : a (a_), b (b_), k (k_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        // Normal union
        if (k == 0)
            return std::min (a->dist (p), b->dist (p));

        // Smooth union
        gu::dist_t h = std::max (k - std::abs (a->dist (p) - b->dist (p)),
                                 0.0) / k;

        return std::min (a->dist (p), b->dist (p)) - h * h * k * (1.0 / 4.0);       
    }
    
    BoundingBox bbox () const override
    {
        return {gu::min (a->bbox ().min, b->bbox ().min),
                gu::max (a->bbox ().max, b->bbox ().max)};
    }
};

/**
 * Build a union form of a and b.
 */
FormPtr build_union (FormPtr a, FormPtr b, gu::dist_t k = 0)
{
    return std::make_shared<Union> (a, b, k);
}
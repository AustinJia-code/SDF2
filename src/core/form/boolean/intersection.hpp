/**
 * @file intersection.hpp
 * @brief Intersection form operation.
 */

#pragma once

#include "core/form/form.hpp"

/**
 * Intersection
 */
class Intersection : public Form
{
private:
    FormPtr a;
    FormPtr b;

public:
    Intersection (FormPtr a_, FormPtr b_)
        : a (a_), b (b_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        return std::max (a->dist (p), b->dist (p));
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
FormPtr build_intersection (FormPtr a, FormPtr b)
{
    return std::make_shared<Intersection> (a, b);
}

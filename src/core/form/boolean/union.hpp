/**
 * @file union.hpp
 * @brief Union form operation.
 */

#pragma once

#include "core/form/form.hpp"

/**
 * Union
 */
class Union : public Form
{
private:
    FormPtr a;
    FormPtr b;
    bool smooth;

public:
    Union (FormPtr a_, FormPtr b_, bool smooth_ = false)
        : a (a_), b (b_), smooth (smooth_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        // TODO: Implement smooth union
        return std::min (a->dist (p), b->dist (p));
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
FormPtr build_union (FormPtr a, FormPtr b)
{
    return std::make_shared<Union> (a, b);
}
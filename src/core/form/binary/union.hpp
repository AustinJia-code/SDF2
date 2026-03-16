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

public:
    Union (FormPtr a_, FormPtr b_)
        : a (a_), b (b_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        return std::min (a->dist (p), b->dist (p));
    }
};

/**
 * Make a union form of a and b.
 */
FormPtr union_op (FormPtr a, FormPtr b)
{
    return std::make_shared<Union> (a, b);
}
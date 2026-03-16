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

public:
    Difference (FormPtr a_, FormPtr b_)
        : a (a_), b (b_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        return std::max (a->dist (p), -b->dist (p));
    }
};

/**
 * Make a difference form of a and b.
 */
FormPtr difference_op (FormPtr a, FormPtr b)
{
    return std::make_shared<Difference> (a, b);
}
/**
 * @file intersection.hpp
 * @brief Intersection form operation.
 */

#pragma once

#include "core/common/form.hpp"
#include <vector>

/**
 * Intersection
 */
class Intersection : public Form
{
private:
    std::vector <FormPtr> forms;
    gu::dist_t k;

    gu::dist_t smax (gu::dist_t a, gu::dist_t b, gu::dist_t k) const
    {
        gu::dist_t h = std::max (k - std::abs (a - b), 0.0) / k;
        return std::max (a, b) - h * h * h * k / 4.0;
    }

public:
    Intersection (const std::vector <FormPtr>& forms_, gu::dist_t k_ = 0)
        : forms (forms_), k (k_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        gu::dist_t result = forms[0]->dist (p);

        for (size_t i = 1; i < forms.size (); ++i)         
        {                                                  
            gu::dist_t d = forms[i]->dist (p);             
            result = (k == 0)
                   ? std::max (result, d)
                   : smax (result, d, k);                                           
        }

        return result;
    }

    BoundingBox bbox () const override
    {
        BoundingBox result = forms[0]->bbox ();

        for (size_t i = 1; i < forms.size (); ++i)
        {
            BoundingBox b = forms[i]->bbox ();
            result.min = gu::max (result.min, b.min);
            result.max = gu::min (result.max, b.max);
        }
        return result;
    }
};

/**
 * Build an intersection form of a and b.
 */
FormPtr build_intersection (FormPtr a, FormPtr b, gu::dist_t k = 0)
{
    return std::make_shared<Intersection> (std::vector<FormPtr>{a, b}, k);
}

/**
 * Build an intersection form of multiple forms.
 */
FormPtr build_intersection (const std::vector<FormPtr>& forms, gu::dist_t k = 0)
{
    return std::make_shared<Intersection> (forms, k);
}

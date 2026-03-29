/**
 * @file union.hpp
 * @brief Union form operation.
 * 
 * @bug
 *      - Strange behavior on coplaner unions w/ k > 0, likely need normals to
 *        fix
 */

#pragma once

#include <vector>
#include <cassert>
#include "core/common/form.hpp"

/**
 * Union
 */
class Union : public Form
{
private:
    std::vector <FormPtr> forms;
    gu::dist_t k;

    gu::dist_t smin (gu::dist_t a, gu::dist_t b, gu::dist_t k) const
    {
        gu::dist_t h = std::max (k - std::abs (a - b), 0.0) / k;
        return std::min (a, b) - h * h * h * k / 6.0;
    }

public:
    Union (const std::vector <FormPtr>& forms_, gu::dist_t k_ = 0)
        : forms (forms_), k (k_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        gu::dist_t result = forms[0]->dist (p);

        for (size_t i = 1; i < forms.size (); ++i)         
        {                                                  
            gu::dist_t d = forms[i]->dist (p);             
            result = (k == 0)
                   ? std::min (result, d)
                   : smin (result, d, k);                                           
        }

        return result;
    }
    
    BoundingBox bbox () const override
    {
        BoundingBox result = forms[0]->bbox ();

        for (size_t i = 1; i < forms.size (); ++i)
        {
            BoundingBox b = forms[i]->bbox ();
            result.min = gu::min (result.min, b.min);
            result.max = gu::max (result.max, b.max);
        }
        return result;
    }
};

/**
 * Build a union form of two forms
 */
FormPtr build_union (FormPtr a, FormPtr b, gu::dist_t k = 0)
{
    return std::make_shared<Union> (std::vector<FormPtr>{a, b}, k);
}

/**
 * Build a union form of multiple forms
 * @note If less than 2 forms are provided, returns nullptr.
 */
FormPtr build_union (const std::vector <FormPtr>& forms, gu::dist_t k = 0)
{
    if (forms.size () < 2)
        return nullptr;
    
    return std::make_shared<Union> (forms, k);
}
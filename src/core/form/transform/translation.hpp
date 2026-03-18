/**
 * @file translation.hpp
 * @brief Translate form laterally.
 */

#pragma once

#include "common/form.hpp"

/**
 * Translation
 */
class Translation : public Form
{
private:
    FormPtr form;
    gu::vec3_t delta;

public:
    Translation (FormPtr form_, const gu::vec3_t& delta_)
        : form (form_), delta (delta_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        return form->dist (p - delta);
    }

    BoundingBox bbox () const override
    {
        BoundingBox b = form->bbox ();
        return {b.min + delta, b.max + delta};
    }
};

/**
 * Add a translation to a given form.
 */
FormPtr add_translation (FormPtr form, const gu::vec3_t& delta)
{
    return std::make_shared<Translation> (form, delta);
}
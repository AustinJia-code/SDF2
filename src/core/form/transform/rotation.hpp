/**
 * @file rotation.hpp
 * @brief Rotate form around an axis.
 */

#pragma once

#include "core/form/form.hpp"

/**
 * Rotation
 */
class Rotation : public Form
{
private:
    FormPtr form;
    gu::vec3_t axis;
    double angle;

public:
    Rotation (FormPtr form_, const gu::vec3_t& axis_, double angle_)
        : form (form_), axis (axis_), angle (angle_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        // TODO: Implement axis-angle rotation
        return form->dist (p);
    }

    BoundingBox bbox () const override
    {
        // TODO: Implement bounding box rotation
        return form->bbox ();
    }
};

/**
 * Add a rotation to a given form.
 */
FormPtr add_rotation (FormPtr form, const gu::vec3_t& axis, double angle)
{
    return std::make_shared<Rotation> (form, axis, angle);
}
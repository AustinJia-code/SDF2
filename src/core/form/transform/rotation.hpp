/**
 * @file rotation.hpp
 * @brief Rotate form around an axis.
 */

#pragma once

#include <array>
#include "common/form.hpp"

const gu::vec3_t X_AXIS = gu::vec3_t (1, 0, 0);
const gu::vec3_t Y_AXIS = gu::vec3_t (0, 1, 0);
const gu::vec3_t Z_AXIS = gu::vec3_t (0, 0, 1);

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
        return form->dist (gu::rotate (p, axis, -angle));
    }

    BoundingBox bbox () const override
    {
        BoundingBox b = form->bbox();
        
        // All 8 corners
        std::array<gu::vec3_t, 8> corners =
        {{
            {b.min.x, b.min.y, b.min.z},
            {b.max.x, b.min.y, b.min.z},
            {b.min.x, b.max.y, b.min.z},
            {b.max.x, b.max.y, b.min.z},
            {b.min.x, b.min.y, b.max.z},
            {b.max.x, b.min.y, b.max.z},
            {b.min.x, b.max.y, b.max.z},
            {b.max.x, b.max.y, b.max.z}
        }};

        gu::vec3_t new_min = gu::rotate (corners[0], axis, angle);
        gu::vec3_t new_max = new_min;
        
        for (int i = 1; i < 8; i++)
        {
            gu::vec3_t r = gu::rotate (corners[i], axis, angle);
            new_min = gu::min (new_min, r);
            new_max = gu::max (new_max, r);
        }
        
        return {new_min, new_max};
    }
};

/**
 * Add a rotation to a given form.
 */
FormPtr add_rotation (FormPtr form, const gu::vec3_t& axis, double angle)
{
    return std::make_shared<Rotation> (form, axis, angle);
}
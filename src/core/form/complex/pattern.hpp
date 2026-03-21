/**
 * @file pattern.hpp
 * @brief Circular pattern.
 */

#pragma once

#include <vector>
#include "minis/utils/math_utils.hpp"
#include "core/common/form.hpp"
#include "core/form/transform/rotation.hpp"
#include "core/form/transform/translation.hpp"
#include "core/form/boolean/union.hpp"

/**
 * Pattern
 */
class Pattern : public Form
{
private:
    FormPtr form;

public:
    Pattern (FormPtr form_, size_t count_, const gu::vec3_t& axis_,
             gu::dist_t radius_)
    {
        std::vector<FormPtr> instances;

        // Get perpendicular axis for translation w/ arbitrary coplanar vec
        gu::vec3_t copl = (std::abs (axis_.x) < 0.9)
                         ? gu::vec3_t (1, 0, 0)
                         : gu::vec3_t (0, 1, 0);
        gu::vec3_t perp = gu::norm (gu::cross (axis_, copl)) * radius_;
        FormPtr translated = add_translation (form_, perp);

        double angle_step = 360.0 / count_;
        
        for (size_t i = 0; i < count_; i++)
        {
            double angle = i * angle_step;

            instances.push_back
            (
                add_rotation
                (
                    translated,
                    axis_,
                    angle
                )
            );
        }

        form = instances[0];
        for (size_t i = 1; i < count_; i++)
            form = build_union (form, instances[i]);
    }

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        return form->dist (p);
    }

    BoundingBox bbox () const override
    {
        return form->bbox();
    }
};

/**
 * Build a pattern from a given form.
 */
FormPtr build_pattern (FormPtr form, size_t count, const gu::vec3_t& axis,
                       gu::dist_t radius)
{
    return std::make_shared<Pattern> (form, count, axis, radius);
}
/**
 * @file mirror.hpp
 * @brief Mirror form across a plane.
 */

#pragma once

#include <array>
#include "core/common/form.hpp"

/**
 * Mirror
 */
class Mirror : public Form
{
private:
    FormPtr form;
    gu::vec3_t normal;
    gu::vec3_t offset;

public:
    Mirror (FormPtr form_, const gu::vec3_t& normal_, const gu::vec3_t& offset_)
        : form (form_), normal (gu::norm (normal_)), offset (offset_) {}

    gu::dist_t dist (const gu::vec3_t& p) const override
    {
        gu::dist_t d = gu::dot (p - offset, normal);
        gu::vec3_t p_reflected = p - normal * (2.0 * d);
        
        return form->dist (p_reflected);
    }

    BoundingBox bbox () const override
    {
        BoundingBox b = form->bbox ();

        auto reflect = [&] (const gu::vec3_t& v)
        {
            gu::dist_t d = gu::dot (v - offset, normal);
            return v - normal * (2.0 * d);
        };

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

        gu::vec3_t new_min = reflect (corners[0]);
        gu::vec3_t new_max = new_min;

        for (const auto& c : corners)
        {
            gu::vec3_t r = reflect (c);
            new_min = gu::min (new_min, r);
            new_max = gu::max (new_max, r);
        }

        return {new_min, new_max};
    }
};

/**
 * Mirror a form across a plane defined by a normal and an offset point.
 */
FormPtr add_mirror (FormPtr form, const gu::vec3_t& normal,
                    const gu::vec3_t& offset = gu::vec3_t (0, 0, 0))
{
    return std::make_shared<Mirror> (form, normal, offset);
}

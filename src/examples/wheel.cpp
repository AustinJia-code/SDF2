/**
 * @file wheel.cpp
 * @brief Generates a simple spoked wheel.
 *
 * ./wheel -bo ~/Projects/SDF2/out/wheel.stl
 */

#include "tools/script.hpp"

int main (int argc, char* argv[])
{
    return run_script (argc, argv, "wheel", [] ()
    {
        const gu::dist_t rim_r     = 20;
        const gu::dist_t rim_thick = 2;
        const gu::dist_t hub_r     = 4;
        const gu::dist_t width     = 3;
        const size_t     n_spokes  = 6;

        const gu::dist_t spoke_len = rim_r - rim_thick - hub_r;
        const gu::dist_t spoke_mid = hub_r + spoke_len / 2;

        auto rim =          build_difference
                            (
                                make_cylinder (rim_r,             width),
                                make_cylinder (rim_r - rim_thick, width)
                            );

        auto hub =          make_cylinder
                            (
                                hub_r,
                                width
                            );

        auto spoke_shape =  add_translation
                            (
                                make_box   (spoke_len, 1.5, width * 0.8),
                                gu::vec3_t (spoke_mid, 0, 0)
                            );

        auto spokes =       build_pattern
                            (
                                spoke_shape,
                                n_spokes,
                                Z_AXIS,
                                0
                            );

        return build_union
               ({
                   rim,
                   hub,
                   spokes
               });
    });
}

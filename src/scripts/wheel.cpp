/**
 * @file wheel.cpp
 * @brief Generates a simple spoked wheel.
 * 
 * ./wheel -bo ~/Projects/SDF2/out/wheel.stl
 */

#include "minis/arg_parser/arg_parser.hpp"

#include "core/sdf2.hpp"
#include <iostream>

int main (int argc, char *argv[])
{
    // Parse cmd
    argp::arg_parser argp (argc, argv);
    auto out_arg = argp.get ("o");
    if (!out_arg || out_arg->empty () || out_arg->size () > 1)
    {
        std::cerr << "Usage: ./wheel -o <PATH_TO_STL_OUT>" << std::endl;
        return 1;
    }
    std::string out_path = (*out_arg)[0];

    // Build form
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

    auto wheel =        build_union
                        (
                            build_union
                            (
                                rim,
                                hub
                            ),
                            spokes
                        );

    // Output
    form_to_stl (wheel, out_path,
    {
        .cube_size  = 0.2,
        .coalesce   = *argp.get_bool ("c"),
        .binary     = *argp.get_bool ("b"),
    });

    return EXIT_SUCCESS;
}
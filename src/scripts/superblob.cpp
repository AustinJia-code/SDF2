/**
 * @file superblob.cpp
 * @brief Generates a superblob to demonstrate all functionality.
 * 
 * ./superblob -o ~/Projects/SDF2/out/superblob.stl
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
        std::cerr << "Usage: ./superblob -o <PATH_TO_STL_OUT>" << std::endl;
        return 1;
    }
    std::string out_path = (*out_arg)[0];

    // Build form
    auto sphere =   add_translation (
                        make_sphere (28),
                        gu::vec3_t (15, 15, 15));
    auto box =      add_rotation (
                        make_box (40, 40, 40),
                        Z_AXIS, 10);
    auto cylinder = add_rotation (
                        add_rotation (
                            make_cylinder (10, 80),
                            Y_AXIS, 90),
                        Z_AXIS, 45);

    auto form =     build_difference (
                        build_union (
                            sphere,
                            box,
                            gu::dist_t {5}),
                        cylinder,
                        gu::dist_t {5});

    // Output
    form_to_stl (form, out_path,
    {
        .cube_size  = 0.2,
        .coalesce   = argp.get_bool ("c").value_or (false),
        .binary     = argp.get_bool ("b").value_or (true),
    });

    return EXIT_SUCCESS;
}
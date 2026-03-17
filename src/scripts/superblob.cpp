/**
 * @file superblob.cpp
 * @brief Generates a superblob to demonstrate all functionality.
 * 
 * ./superblob -o ~/Projects/SDF2/out/superblob.stl
 */

#include "minis/arg_parser/arg_parser.hpp"

#include "core/core.hpp"
#include <iostream>
#include <fstream>

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
    // auto sphere = add_translation (
    //                 make_sphere (28),
    //                 gu::vec3_t (15, 15, 15));
    auto form = add_rotation (make_box (40, 40, 40), Z_AXIS, 20);

    // auto form = build_union (sphere, box);
    // auto form = build_union (sphere, box, gu::dist_t {5});
    // auto form = build_intersection (sphere, box);
    // auto form = build_difference (sphere, box);

    // Output
    auto stl = form_to_stl ("superblob", form, gu::dist_t {0.5});
    std::ofstream file (out_path);
    file << stl;
    file.close ();

    return 0;
}
/**
 * @file box.cpp
 * @brief Generates a box.
 * 
 * ./box -o ~/Projects/SDF2/out/box.stl
 */

#include "minis/arg_parser/arg_parser.hpp"

#include "core/sdf2.hpp"
#include <iostream>
#include <fstream>

int main (int argc, char *argv[])
{
    // Parse cmd
    argp::arg_parser argp (argc, argv);
    auto out_arg = argp.get ("o");
    if (!out_arg || out_arg->empty () || out_arg->size () > 1)
    {
        std::cerr << "Usage: ./box -o <PATH_TO_STL_OUT>" << std::endl;
        return 1;
    }
    std::string out_path = (*out_arg)[0];

    // Build form
    // auto box = make_box (40, 40, 40);
    auto box = add_rotation (make_box (40, 40, 40), Z_AXIS, 10);

    // Output
    auto stl = form_to_stl ("box", box, gu::dist_t {0.2});
    std::ofstream file (out_path);
    file << stl;
    file.close ();

    return EXIT_SUCCESS;
}
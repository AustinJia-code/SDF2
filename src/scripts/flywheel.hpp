/**
 * @file flywheel.cpp
 * @brief Generates a reaction wheel base.
 * 
 * ./flywheel -o ~/Projects/SDF2/out/flywheel.stl
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
    auto form = make_cylinder (10, 5);

    // Output
    form_to_stl (form, out_path,
    {
        .cube_size  = 0.2,
        .coalesce   = argp.get_bool ("c").value_or (false),
        .binary     = argp.get_bool ("b").value_or (true),
    });

    return EXIT_SUCCESS;
}
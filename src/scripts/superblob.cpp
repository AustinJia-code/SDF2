/**
 * @file superblob.cpp
 * @brief Generates a superblob to demonstrate all functionality.
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
    auto sphere = make_sphere (28);
    auto box = make_box (gu::vec3_t (20, 20, 20));

    auto form = build_union (sphere, box);


    // Output
    auto stl = form_to_stl ("superblob", form);
    std::ofstream file (out_path);
    file << stl;
    file.close ();

    return 0;
}
/**
 * @file script.hpp
 * @brief Boilerplate helper for SDF2 scripts.
 *
 * Usage:
 *   int main (int argc, char* argv[])
 *   {
 *       return run_script (argc, argv, "my_script", [] (argp::arg_parser&)
 *       {
 *           return make_sphere (10);
 *       });
 *   }
 */

#pragma once

#include "minis/arg_parser/arg_parser.hpp"
#include "core/sdf2.hpp"
#include <functional>
#include <iostream>

inline int run_script (int argc, char* argv[], const char* name,
                       std::function<std::shared_ptr<const Form> ()> build)
{
    argp::arg_parser argp (argc, argv);

    auto out_arg = argp.get ("o");
    if (!out_arg || out_arg->empty () || out_arg->size () > 1)
    {
        std::cerr << "Usage: ./" << name << " -o <PATH_TO_STL_OUT>" << std::endl;
        return 1;
    }

    form_to_stl (build (), (*out_arg)[0],
    {
        .cube_size  = 0.2,
        .coalesce   = *argp.get_bool ("c"),
        .binary     = *argp.get_bool ("b"),
    });

    return EXIT_SUCCESS;
}

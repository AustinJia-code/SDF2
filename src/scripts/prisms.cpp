/**
 * @file prisms.cpp
 * @brief Generates a set of prisms.
 * 
 * ./prisms -bo ~/Projects/SDF2/out/prisms.stl
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
        std::cerr << "Usage: ./prisms -o <PATH_TO_STL_OUT>" << std::endl;
        return 1;
    }
    std::string out_path = (*out_arg)[0];

    // Build form
    auto prism3 = make_inscprism (3, 10, 10);
    auto prism4 = make_inscprism (4, 10, 10);
    auto prism5 = make_circprism (5, 10, 10);
    auto prism6 = make_circprism (6, 10, 10);

    // EWWW!!! TODO: Support arbitrary union param count!!
    auto form = build_union
                (
                    add_translation (prism3, gu::vec3_t (-15, 0, 0)),
                    build_union
                    (
                        prism4,
                        build_union
                        (
                            add_translation (prism5, gu::vec3_t (15, 0, 0)),
                            add_translation (prism6, gu::vec3_t (30, 0, 0))
                        )
                    )
                );

    // Output
    form_to_stl (form, out_path,
    {
        .cube_size  = 0.2,
        .coalesce   = *argp.get_bool ("c"),
        .binary     = *argp.get_bool ("b"),
    });

    return EXIT_SUCCESS;
}
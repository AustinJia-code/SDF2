/**
 * @file sdf2_script.cpp
 * @brief SDF2 script interpreter to run a .sdf script without recompiling.
 *
 * Usage:
 *   ./sdf2_script -s <SCRIPT.sdf> -o <OUTPUT.stl> [-b] [-c]
 */

#include "script/lexer.hpp"
#include "script/parser.hpp"
#include "script/evaluator.hpp"
#include "minis/arg_parser/arg_parser.hpp"
#include "core/sdf2.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

int main (int argc, char* argv[])
{
    argp::arg_parser argp (argc, argv);

    auto script_arg = argp.get ("s");
    auto out_arg = argp.get ("o");

    if (!script_arg || script_arg->empty () ||
        !out_arg    || out_arg->empty ())
    {
        std::cerr << "Usage: ./sdf2_script -s <SCRIPT.sdf> -o <OUTPUT.stl> [-b] [-c]"
                  << std::endl;
        return 1;
    }

    std::ifstream file ((*script_arg)[0]);
    if (!file)
    {
        std::cerr << "Error: cannot open '" << (*script_arg)[0] << "'" << std::endl;
        return 1;
    }

    std::ostringstream ss;
    ss << file.rdbuf ();

    try
    {
        std::string src = ss.str ();
        Parser parser (src);
        Evaluator evaluator;
        evaluator.run (parser.parse ());

        form_to_stl (evaluator.result (), (*out_arg)[0],
        {
            .cube_size = 0.2,
            .coalesce  = *argp.get_bool ("c"),
            .binary    = *argp.get_bool ("b"),
        });
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what () << std::endl;
        return 1;
    }

    return 0;
}

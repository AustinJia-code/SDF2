/**
 * @file superblob.cpp
 * @brief Generates a superblob to demonstrate all functionality.
 */

#include "minis/arg_parser/arg_parser.hpp"

#include "core/core.hpp"
#include <iostream>

int main (int argc, char *argv[])
{
    argp::arg_parser argp (argc, argv);
    auto out_path = argp.get ("-o");
    if (!out_path || out_path->empty () || out_path->size () > 1)
    {
        std::cerr << "Usage: ./superblob -o <PATH_TO_STL_OUT>";
        return 1;
    }
    std::string out_path = (*out_path)[0];

    // RUN AND OUTPUT!
    

    return 0;
}
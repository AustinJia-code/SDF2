/**
 * @file box.cpp
 * @brief Generates a box.
 *
 * ./box -bo ~/Projects/SDF2/out/box.stl
 */

#include "tools/script.hpp"

int main (int argc, char* argv[])
{
    return run_script (argc, argv, "box", [] ()
    {
        return add_rotation (make_box (40, 40, 40), Z_AXIS, 10);
    });
}

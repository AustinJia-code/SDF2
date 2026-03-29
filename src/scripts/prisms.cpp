/**
 * @file prisms.cpp
 * @brief Generates a set of prisms.
 *
 * ./prisms -bo ~/Projects/SDF2/out/prisms.stl
 */

#include "tools/script.hpp"

int main (int argc, char* argv[])
{
    return run_script (argc, argv, "prisms", [] ()
    {
        auto prism3 = make_inscprism (3, 10, 10);
        auto prism4 = make_inscprism (4, 10, 10);
        auto prism5 = make_circprism (5, 10, 10);
        auto prism6 = make_circprism (6, 10, 10);

        return build_union
        ({
            add_translation (prism3, gu::vec3_t (-15, 0, 0)),
            prism4,
            add_translation (prism5, gu::vec3_t (15, 0, 0)),
            add_translation (prism6, gu::vec3_t (30, 0, 0))
        }, 1);
    });
}

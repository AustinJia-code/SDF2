/**
 * @file superblob.cpp
 * @brief Generates a superblob to demonstrate all functionality.
 *
 * ./superblob -bo ~/Projects/SDF2/out/superblob.stl
 */

#include "tools/script.hpp"

int main (int argc, char* argv[])
{
    return run_script (argc, argv, "superblob", [] ()
    {
        auto sphere =   add_translation (
                            make_sphere (28),
                            gu::vec3_t (15, 15, 15));
        auto box =      add_rotation (
                            make_box (40, 40, 40),
                            Z_AXIS, 10);
        auto cylinder = add_rotation (
                            add_rotation (
                                make_cylinder (10, 80),
                                Y_AXIS, 90),
                            Z_AXIS, 45);

        return build_difference (
                   build_union (
                       sphere,
                       box,
                       gu::dist_t {5}),
                   cylinder,
                   gu::dist_t {5});
    });
}

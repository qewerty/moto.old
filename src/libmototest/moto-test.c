#include <glib.h>
#include "moto-test.h"
#include "moto-test-mesh.h"
#include "libmoto/moto-bitmask.h"

static void moto_test_bitmask(void)
{
    guint bits_num = 100000;
    MotoBitmask *bm = moto_bitmask_new(bits_num);

    moto_bitmask_set(bm, 123);
    g_assert(moto_bitmask_is_set(bm, 123));
    g_assert(moto_bitmask_is_set_fast(bm, 123));
    g_assert( ! moto_bitmask_is_set(bm, 124));
    g_assert( ! moto_bitmask_is_set_fast(bm, 124));
    g_assert( ! moto_bitmask_is_set(bm, 122));
    g_assert( ! moto_bitmask_is_set_fast(bm, 122));

    moto_bitmask_set(bm, 123);
    moto_bitmask_set(bm, 1123);
    moto_bitmask_set_fast(bm, 9897);

    moto_bitmask_toggle(bm, 9897);
    g_assert(moto_bitmask_get_set_num(bm) == 2);
    moto_bitmask_toggle_fast(bm, 9897);

    moto_bitmask_toggle_fast(bm, 1123);
    g_assert(moto_bitmask_get_set_num(bm) == 2);
    moto_bitmask_toggle(bm, 1123);

    g_assert(moto_bitmask_get_set_num(bm) == 3);
    g_assert(moto_bitmask_get_set_num(bm) == moto_bitmask_calc_set_num(bm));

    moto_bitmask_unset(bm, 1123);
    g_assert(moto_bitmask_get_set_num(bm) == 2);

    moto_bitmask_unset_fast(bm, 1123);
    g_assert(moto_bitmask_get_set_num(bm) == 2);
    g_assert(moto_bitmask_get_set_num(bm) == moto_bitmask_calc_set_num(bm));

    MotoBitmask *bm0 = moto_bitmask_new_copy(bm);

    g_assert(moto_bitmask_get_bits_num(bm) == moto_bitmask_get_bits_num(bm0));
    g_assert(moto_bitmask_calc_set_num(bm) == moto_bitmask_get_set_num(bm0));
    g_assert(moto_bitmask_get_set_num(bm0) == 2);

    moto_bitmask_inverse(bm0);
    g_assert(moto_bitmask_get_set_num(bm0) == (bits_num - 2));
    moto_bitmask_inverse(bm0);
    g_assert(moto_bitmask_get_set_num(bm0) == 2);

    moto_bitmask_unset_all(bm0);
    g_assert(moto_bitmask_get_set_num(bm0) == 0 && moto_bitmask_get_set_num(bm0) == moto_bitmask_calc_set_num(bm0));

    moto_bitmask_set_all(bm0);
    g_assert(moto_bitmask_get_set_num(bm0) == bits_num && moto_bitmask_get_set_num(bm0) == moto_bitmask_calc_set_num(bm0));

    moto_bitmask_free(bm);
    moto_bitmask_free(bm0);
}


void moto_collect_tests(void)
{
    g_test_add_func("/moto/bitmask", moto_test_bitmask);

    moto_collect_mesh_tests();
}

void moto_test_run(int *argc, char **argv[])
{
    g_test_init(argc, argv, NULL);
    moto_collect_tests();
    g_test_run();
}

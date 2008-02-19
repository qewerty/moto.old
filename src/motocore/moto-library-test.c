#include "moto-library.h"
#include "moto-node.h"

int main(int argc, char *argv[])
{
    g_print("Testing \"moto-library.h\" ...\n");

    g_type_init();

    MotoLibrary *lib = moto_library_new();

    moto_library_new_slot(lib, "test", MOTO_TYPE_NODE);

    g_print("%s\n", g_type_name(MOTO_TYPE_LIBRARY));

    // g_assert(moto_library_new_entry(lib, "test", "test", lib));

    g_print("OK\n");

    return 0;
}

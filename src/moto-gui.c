#include <stdlib.h>

#include "libmotoutil/moto-gl.h"

#include "libmoto/moto.h"
#include "libmotogui/moto-test-window.h"

#include "libmoto/moto-node.h"
#include "libmoto/moto-object-node.h"
#include "libmoto/moto-plane-node.h"

#include "libmototest/moto-test.h"

void print_type_tree_indent(GType type, const gchar *indent)
{
    if(G_TYPE_IS_ABSTRACT(type))
        g_print("%s%s (abstract)\n", indent, g_type_name(type));
    else
        g_print("%s%s\n", indent, g_type_name(type));

    GString *indent0 = g_string_new(indent);
    g_string_append(indent0, "    ");

    guint i, num;
    GType *t = g_type_children(type, &num);
    for(i = 0; i < num; i++)
    {
        print_type_tree_indent(t[i], indent0->str);
    }
    g_free(t);
    g_string_free(indent0, TRUE);
}

void print_type_tree(GType type)
{
    print_type_tree_indent(type, "");
}

/*
static gboolean test_idle(gpointer data)
{
    g_print("test idle\n");
}
*/

void print_enum()
{
    GType g_type = MOTO_TYPE_ORIENTATION;
    if(G_TYPE_IS_ENUM(g_type))
    {
        GEnumClass *ec = (GEnumClass *)g_type_class_ref(g_type);

        g_print("Values (%u) of enum type %s:\n", ec->n_values, g_type_name(g_type));
        guint i;
        for(i = 0; i < ec->n_values; i++)
        {
            g_print("\t%s\n", ec->values[i].value_nick);
        }

        g_type_class_unref(ec);
        /*
        */
    }
}

int main(int argc, char *argv[])
{

    gtk_init(& argc, & argv);
    g_thread_init(NULL);

    if(argc > 1 && ((0 == g_strcmp0(argv[1], "--test")) || (0 == g_strcmp0(argv[1], "-t"))))
    {
        g_print("Testing ... \n");
        g_test_init(& argc, & argv, NULL);
        moto_collect_test_cases();
        g_test_run();
        exit(0);
    }

    g_print("Initializing Moto ...\n");

    // g_print("s: %s\n", g_find_program_in_path(argv[0]));

    GtkWindow *win = moto_test_window_new();
    gtk_widget_show_all((GtkWidget *)win);

    // moto_glext_print_list();

    // print_enum();
    // g_print("g_type_name(G_TYPE_STRING): %s\n", g_type_name(G_TYPE_STRING));

    g_print("\nRegistered node types:\n\n");
    print_type_tree(MOTO_TYPE_NODE);
    g_print("\n... OK\n");

    // g_idle_add(test_idle, NULL);

    gtk_main();

    return 0;
}

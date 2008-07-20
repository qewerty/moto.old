#include <GL/gl.h>
#include <gtk/gtk.h>

#include "moto.h"
#include "gui/moto-test-window.h"
#include "common/moto-glext.h"

#include "moto-node.h"

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

int main(int argc, char *argv[])
{
    g_print("Initializing Moto ...\n");

    gtk_init(& argc, & argv);
    g_thread_init(NULL);

    GtkWindow *win = moto_test_window_new();
    gtk_widget_show_all((GtkWidget *)win);

    // moto_glext_print_list();

    g_print("\nRegistered node types:\n\n");
    print_type_tree(MOTO_TYPE_NODE);
    g_print("\n... OK\n");

    gtk_main();

    return 0;
}

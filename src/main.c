#include <gtk/gtk.h>
#include "moto.h"
#include "gui/moto-test-window.h"

int main(int argc, char *argv[])
{
    gtk_init(& argc, & argv);
    g_thread_init(NULL);

    GtkWindow *win = moto_test_window_new();
    gtk_widget_show_all((GtkWidget *)win);
    gtk_main();

    return 0;
}

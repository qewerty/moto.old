#include "stdlib.h"
#include "math.h"

#include <GL/glew.h>
#include <gtk/gtkgl.h>

#include "moto-test-window.h"
#include "moto-main-menu.h"

#include "libmoto/moto-world.h"
#include "libmoto/moto-system.h"
#include "libmoto/moto-node.h"
#include "libmotoutil/numdef.h"

/* forward */

void file_menu_quit(GtkMenuItem *item, gpointer user_data)
{
    gtk_main_quit();
}

/* class MainMenu */

static GObjectClass *main_menu_parent_class = NULL;

struct _MotoMainMenuPriv
{
    gboolean disposed;
};

static void
moto_main_menu_dispose(GObject *obj)
{
    MotoMainMenu *self = (MotoMainMenu *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    main_menu_parent_class->dispose(obj);
}

static void
moto_main_menu_finalize(GObject *obj)
{
    MotoMainMenu *self = (MotoMainMenu *)obj;

    g_slice_free(MotoMainMenuPriv, self->priv);

    main_menu_parent_class->finalize(obj);
}

static void
moto_main_menu_init(MotoMainMenu *self)
{
    GtkMenuBar *menu_bar = (GtkMenuBar *)self;

    self->priv = g_slice_new(MotoMainMenuPriv);

    /* Root menu items. */
    GtkWidget *file = gtk_menu_item_new_with_label("File");
    GtkWidget *project = gtk_menu_item_new_with_label("Project");
    GtkWidget *world = gtk_menu_item_new_with_label("World");
    GtkWidget *node = gtk_menu_item_new_with_label("Node");
    GtkWidget *render = gtk_menu_item_new_with_label("Render");
    GtkWidget *help = gtk_menu_item_new_with_label("Help");
    gtk_menu_bar_append(menu_bar, file);
    gtk_menu_bar_append(menu_bar, project);
    gtk_menu_bar_append(menu_bar, world);
    gtk_menu_bar_append(menu_bar, node);
    gtk_menu_bar_append(menu_bar, render);
    gtk_menu_bar_append(menu_bar, help);

    GtkMenuItem *item;

    /* File menu */
    GtkMenu *file_menu = (GtkMenu *)gtk_menu_new();
    gtk_menu_item_set_submenu((GtkMenuItem *)file, (GtkWidget *)file_menu);

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Open");
    gtk_menu_append(file_menu, (GtkWidget *)item);
    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Save");
    gtk_menu_append(file_menu, (GtkWidget *)item);
    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Save As ...");
    gtk_menu_append(file_menu, (GtkWidget *)item);

    gtk_menu_append(file_menu, gtk_separator_menu_item_new());

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Quit");
    gtk_menu_append(file_menu, (GtkWidget *)item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(file_menu_quit), NULL);

    /* Project menu */
    GtkMenu *project_menu = (GtkMenu *)gtk_menu_new();
    gtk_menu_item_set_submenu((GtkMenuItem *)project, (GtkWidget *)project_menu);

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("New");
    gtk_menu_append(project_menu, (GtkWidget *)item);
    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Set");
    gtk_menu_append(project_menu, (GtkWidget *)item);

    

    // gtk_widget_set_size_request((GtkWidget *)self, 42, 120);
}

static void
moto_main_menu_class_init(MotoMainMenuClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);

    main_menu_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_main_menu_dispose;
    goclass->finalize   = moto_main_menu_finalize;
}

G_DEFINE_TYPE(MotoMainMenu, moto_main_menu, GTK_TYPE_MENU_BAR);

/* methods of class MainMenu */

GtkWidget *moto_main_menu_new()
{
    GtkWidget *self = (GtkWidget *)g_object_new(MOTO_TYPE_MAIN_MENU, NULL);

    return self;
}


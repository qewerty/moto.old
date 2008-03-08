#include "stdlib.h"
#include "math.h"

#include <gtk/gtkgl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "moto-test-window.h"
#include "moto-main-menu.h"

#include "motocore/moto-world.h"
#include "motocore/moto-system.h"
#include "motocore/moto-node.h"
#include "common/numdef.h"

/* forwards */

static void init_gl(GtkWidget *widget, gpointer data);
static gboolean draw(GtkWidget *widget, GdkEventExpose *event, gpointer data);

static gboolean
reshape(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

static gboolean
press_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean
release_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean
mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data);

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

    GtkWidget *project = gtk_menu_item_new_with_label("Project");
    GtkWidget *world = gtk_menu_item_new_with_label("World");
    GtkWidget *node = gtk_menu_item_new_with_label("Node");
    GtkWidget *render = gtk_menu_item_new_with_label("Render");
    GtkWidget *help = gtk_menu_item_new_with_label("Help");

    gtk_menu_bar_append(menu_bar, project);
    gtk_menu_bar_append(menu_bar, world);
    gtk_menu_bar_append(menu_bar, node);
    gtk_menu_bar_append(menu_bar, render);
    gtk_menu_bar_append(menu_bar, help);

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
    GtkWindow *self = (GtkWindow *)g_object_new(MOTO_TYPE_MAIN_MENU, NULL);
    // MotoMainMenu *twin = (MotoMainMenu *)self;

    // MotoWorld *w = twin->priv->world;

    return self;
}


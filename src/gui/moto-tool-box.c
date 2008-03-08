#include "stdlib.h"
#include "math.h"

#include <gtk/gtkgl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "moto-test-window.h"
#include "moto-tool-box.h"

#include "motocore/moto-world.h"
#include "motocore/moto-system.h"
#include "motocore/moto-node.h"
#include "motocore/moto-object-node.h"
#include "motocore/moto-param-data.h"
#include "motocore/moto-float-param-data.h"
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

/* class ToolBox */

static GObjectClass *tool_box_parent_class = NULL;

struct _MotoToolBoxPriv
{
    MotoSystem *system;
    MotoWorld *world;

    GtkDrawingArea *area;

    gboolean disposed;
};

static void
moto_tool_box_dispose(GObject *obj)
{
    MotoToolBox *self = (MotoToolBox *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_object_unref(self->priv->system);

    tool_box_parent_class->dispose(obj);
}

static void
moto_tool_box_finalize(GObject *obj)
{
    MotoToolBox *self = (MotoToolBox *)obj;
    g_slice_free(MotoToolBoxPriv, self->priv);

    tool_box_parent_class->finalize(obj);
}

static void
moto_tool_box_init(MotoToolBox *self)
{
    self->priv = g_slice_new(MotoToolBoxPriv);

    GtkButton *b = (GtkButton *)gtk_button_new();
    gtk_button_set_relief(b, GTK_RELIEF_NONE);

    GtkWidget *im = gtk_image_new_from_file("./resources/icons/moto-object-selection-mode-icon.png");
    gtk_button_set_image(b, im);

    gtk_box_pack_start((GtkBox *)self, (GtkWidget *)b, FALSE, FALSE, 0);
    b = (GtkButton *)gtk_button_new();

    im = gtk_image_new_from_file("./resources/icons/moto-component-selection-mode-icon.png");
    gtk_button_set_image(b, im);
    gtk_button_set_relief(b, GTK_RELIEF_NONE);

    gtk_box_pack_start((GtkBox *)self, (GtkWidget *)b, FALSE, FALSE, 0);

    gtk_widget_set_size_request((GtkWidget *)self, 42, 120);
}

static void
moto_tool_box_class_init(MotoToolBoxClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);

    tool_box_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_tool_box_dispose;
    goclass->finalize   = moto_tool_box_finalize;
}

G_DEFINE_TYPE(MotoToolBox, moto_tool_box, GTK_TYPE_VBOX);

/* methods of class ToolBox */

GtkWidget *moto_tool_box_new()
{
    GtkWindow *self = (GtkWindow *)g_object_new(MOTO_TYPE_TOOL_BOX, NULL);
    // MotoToolBox *twin = (MotoToolBox *)self;

    // MotoWorld *w = twin->priv->world;

    return self;
}


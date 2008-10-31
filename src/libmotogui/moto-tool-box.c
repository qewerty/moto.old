#include "stdlib.h"
#include "math.h"

#include <libmotoutil/moto-gl.h>

#include "moto-test-window.h"
#include "moto-tool-box.h"

#include "libmoto/moto-messager.h"
#include "libmoto/moto-world.h"
#include "libmoto/moto-system.h"
#include "libmoto/moto-node.h"
#include "libmoto/moto-object-node.h"
#include "libmotoutil/numdef.h"

/* forward */

void show_component_selection_mode_menu(GtkButton *button, gpointer user_data);

/* class ToolBox */

static GObjectClass *tool_box_parent_class = NULL;

struct _MotoToolBoxPriv
{
    MotoSystem *system;

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

    self->priv->system = NULL;

    gtk_widget_set_size_request((GtkWidget *)self, 24, 24);

    /* gui */
    GtkButton *b = (GtkButton *)gtk_button_new();
    // gtk_widget_set_size_request((GtkWidget *)b, 24, 24);
    gtk_button_set_relief(b, GTK_RELIEF_NONE);

    gchar *filename = g_build_filename("resources", "icons", "moto-object-selection-mode-icon.png", NULL);
    GtkWidget *im = gtk_image_new_from_file(filename);
    g_free(filename);

    gtk_button_set_image(b, im);

    gtk_box_pack_start((GtkBox *)self, (GtkWidget *)b, FALSE, FALSE, 0);
    b = (GtkButton *)gtk_button_new();
    // gtk_widget_set_size_request((GtkWidget *)b, 24, 24);

    filename = g_build_filename("resources", "icons", "moto-component-selection-mode-icon.png", NULL);
    im = gtk_image_new_from_file(filename);
    g_free(filename);

    gtk_button_set_image(b, im);
    gtk_button_set_relief(b, GTK_RELIEF_NONE);

    g_signal_connect(G_OBJECT(b), "clicked",
            G_CALLBACK(show_component_selection_mode_menu), self);

    gtk_box_pack_start((GtkBox *)self, (GtkWidget *)b, FALSE, FALSE, 0);

    gtk_widget_set_size_request((GtkWidget *)self, 36, 120);
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

GtkWidget *moto_tool_box_new(MotoSystem *system)
{
    GtkWidget *self = (GtkWidget *)g_object_new(MOTO_TYPE_TOOL_BOX, NULL);
    MotoToolBox *tb = (MotoToolBox *)self;

    if(system)
    {
        g_object_add_weak_pointer(G_OBJECT(system), (gpointer *)(& tb->priv->system));
        tb->priv->system = system;
    }

    return self;
}

void moto_tool_box_set_system(MotoToolBox *self, MotoSystem *system)
{
    if( ! system && self->priv->system)
    {
        g_object_remove_weak_pointer(G_OBJECT(system), (gpointer *)(& self->priv->system));
        self->priv->system = NULL;

        return;
    }

    if( ! MOTO_IS_SYSTEM(system))
    {
        GString *msg = g_string_new("You are trying to set not MotoSystem instance as system for ToolBox");
        moto_error(msg->str);
        g_string_free(msg, TRUE);

        return;
    }

    if(self->priv->system)
    {
        g_object_remove_weak_pointer(G_OBJECT(system), (gpointer *)(& self->priv->system));
    }

    g_object_add_weak_pointer(G_OBJECT(system), (gpointer *)(& self->priv->system));
    self->priv->system = system;
}

/* signal callbacks */

static void set_state(GtkMenuItem *item, gpointer user_data)
{
    MotoGeomViewNode *geom = (MotoGeomViewNode *)user_data;
    MotoGeomViewState *state = \
        (MotoGeomViewState *)g_object_get_data(G_OBJECT(item), "moto-geometry-view-state");

    moto_geom_view_node_set_state(geom, moto_geom_view_state_get_name(state));
}

void show_component_selection_mode_menu(GtkButton *button, gpointer user_data)
{
    MotoToolBox *tb = (MotoToolBox *)user_data;

    if( ! tb->priv->system)
    {
        GString *msg = \
            g_string_new("ToolBox has no associated MotoSystem instance. I can do nothing.");
        moto_error(msg->str);
        g_string_free(msg, TRUE);

        return;
    }

    MotoWorld *world = moto_system_get_current_world(tb->priv->system);
    if( ! world)
    {
        GString *msg = \
            g_string_new("MotoSystem instance associated ToolBox has no current world. I can do nothing.");
        moto_error(msg->str);
        g_string_free(msg, TRUE);

        return;
    }

    MotoObjectNode *object = moto_world_get_current_object(world);
    if( ! object)
    {
        GString *msg = \
            g_string_new("MotoWorld instance associated ToolBox has no current object. I can do nothing.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);

        return;
    }

    MotoParam *in_view = moto_node_get_param((MotoNode *)object, "view");
    if(( ! in_view) || (moto_param_get_mode(in_view) == MOTO_PARAM_MODE_OUT))
    {
        GString *msg = \
            g_string_new("Current object has no a view. I can do nothing.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);

        return;
    }

    MotoParam *out_view = moto_param_get_source(in_view);
    if( ! out_view)
    {
        GString *msg = \
            g_string_new("Current object has no a view. I can do nothing.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);

        return;
    }

    MotoGeomViewNode *geom = (MotoGeomViewNode *)moto_param_get_node(out_view);

    GSList *state = moto_geom_view_node_get_state_list(geom);

    /* menu */
    GtkMenu *menu = (GtkMenu *)gtk_menu_new();
    GtkWidget *item;

    for(; state; state = g_slist_next(state))
    {
        item = \
            gtk_menu_item_new_with_label(moto_geom_view_state_get_title((MotoGeomViewState *)state->data));

        g_object_set_data((GObject *)item, "moto-geometry-view-state", state->data);
        g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(set_state), geom);

        gtk_menu_append(menu, item);
    }

    gtk_widget_show_all((GtkWidget *)menu);
    gtk_menu_popup(menu, NULL, NULL, NULL, NULL, 0, 0);
}
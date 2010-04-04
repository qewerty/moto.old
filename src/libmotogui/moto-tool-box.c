#include "stdlib.h"
#include "math.h"

#include <libmotoutil/moto-gl.h>

#include "moto-test-window.h"
#include "moto-tool-box.h"

#include "libmoto/moto-messager.h"
#include "libmoto/moto-scene-node.h"
#include "libmoto/moto-system.h"
#include "libmoto/moto-node.h"
#include "libmoto/moto-object-node.h"
#include "libmotoutil/numdef.h"

/* forward */

void show_draw_mode_menu(GtkButton *button, gpointer user_data);
void show_component_selection_mode_menu(GtkButton *button, gpointer user_data);
void do_fps_test(GtkButton *button, gpointer user_data);

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

    g_signal_connect(G_OBJECT(b), "clicked",
            G_CALLBACK(show_draw_mode_menu), self);

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

    // FPS Test
    b = (GtkButton *)gtk_button_new();
    // gtk_widget_set_size_request((GtkWidget *)b, 24, 24);

    filename = g_build_filename("resources", "icons", "moto-fps-test-icon.png", NULL);
    im = gtk_image_new_from_file(filename);
    g_free(filename);

    gtk_button_set_image(b, im);
    gtk_button_set_relief(b, GTK_RELIEF_NONE);

    g_signal_connect(G_OBJECT(b), "clicked",
            G_CALLBACK(do_fps_test), self);

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

/* Methods of class ToolBox */

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

static void set_draw_mode(GtkMenuItem *item, gpointer user_data)
{
    moto_scene_node_set_draw_mode(MOTO_SCENE_NODE(user_data),
            *((MotoDrawMode*)g_object_get_data(G_OBJECT(item), "moto-draw-mode-enum-value")));
}

static void set_state(GtkMenuItem *item, gpointer user_data)
{
    MotoGeomViewNode *geom = (MotoGeomViewNode *)user_data;
    MotoGeomViewState *state = \
        (MotoGeomViewState *)g_object_get_data(G_OBJECT(item), "moto-geometry-view-state");

    moto_geom_view_node_set_state(geom, moto_geom_view_state_get_name(state));
}

void show_draw_mode_menu(GtkButton *button, gpointer user_data)
{
    MotoToolBox *tb = (MotoToolBox *)user_data;
    if( ! tb->priv->system)
    {
        moto_error("ToolBox has no associated MotoSystem instance. I can do nothing.");

        return;
    }

    MotoSceneNode *scene_node = moto_system_get_current_scene(tb->priv->system);
    if( ! scene_node)
    {
        moto_error("MotoSystem instance associated ToolBox has no current scene_node. I can do nothing.");

        return;
    }


    GEnumClass *ec = G_ENUM_CLASS(g_type_class_ref(MOTO_TYPE_DRAW_MODE));

    GtkMenu *menu = (GtkMenu *)gtk_menu_new();
    GtkWidget *item;

    guint i;
    for(i = 0; i < ec->n_values; i++)
    {
        item = gtk_menu_item_new_with_label(ec->values[i].value_nick);
        g_object_set_data((GObject *)item, "moto-draw-mode-enum-value", & ec->values[i].value);
        g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(set_draw_mode), scene_node);

        gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);
    }
    g_type_class_unref(ec);

    gtk_widget_show_all((GtkWidget *)menu);
    gtk_menu_popup(menu, NULL, NULL, NULL, NULL, 0, 0);
}

void show_component_selection_mode_menu(GtkButton *button, gpointer user_data)
{
    MotoToolBox *tb = (MotoToolBox *)user_data;

    if( ! tb->priv->system)
    {
        moto_error("ToolBox has no associated MotoSystem instance. I can do nothing.");

        return;
    }

    MotoSceneNode *scene_node = moto_system_get_current_scene(tb->priv->system);
    if( ! scene_node)
    {
        moto_error("MotoSystem instance associated ToolBox has no current scene_node. I can do nothing.");

        return;
    }

    MotoObjectNode *object = moto_scene_node_get_current_object(scene_node);
    if( ! object)
    {
        moto_warning("MotoSceneNode instance associated ToolBox has no current object. I can do nothing.");

        return;
    }

    MotoParam *in_view = moto_node_get_param((MotoNode *)object, "view");
    if(( ! in_view) || (moto_param_get_mode(in_view) == MOTO_PARAM_MODE_OUT))
    {
        moto_warning("Current object has no a view. I can do nothing.");

        return;
    }

    MotoParam *out_view = moto_param_get_source(in_view);
    if( ! out_view)
    {
        moto_warning("Current object has no a view. I can do nothing.");

        return;
    }

    MotoGeomViewNode *geom = (MotoGeomViewNode *)moto_param_get_node(out_view);

    /* menu */
    GtkMenu *menu = (GtkMenu *)gtk_menu_new();
    GtkWidget *item;

    GSList *state = moto_geom_view_node_get_state_list(geom);
    for(; state; state = g_slist_next(state))
    {
        item = \
            gtk_menu_item_new_with_label(moto_geom_view_state_get_title((MotoGeomViewState *)state->data));

        g_object_set_data((GObject *)item, "moto-geometry-view-state", state->data);
        g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(set_state), geom);

        gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);
    }

    gtk_widget_show_all((GtkWidget *)menu);
    gtk_menu_popup(menu, NULL, NULL, NULL, NULL, 0, 0);
}

void do_fps_test(GtkButton *button, gpointer user_data)
{
    MotoToolBox *tb = (MotoToolBox *)user_data;

    if( ! tb->priv->system)
    {
        moto_error("ToolBox has no associated MotoSystem instance. I can do nothing.");

        return;
    }

    MotoSceneNode *scene_node = moto_system_get_current_scene(tb->priv->system);
    if( ! scene_node)
    {
        moto_error("MotoSystem instance associated ToolBox has no current scene_node. I can do nothing.");

        return;
    }

    moto_scene_node_draw_fps_test(scene_node);
}

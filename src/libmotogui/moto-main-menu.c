#include "stdlib.h"
#include "math.h"

#include <GL/glew.h>
#include <gtk/gtkgl.h>

#include <glade/glade.h>

#include "moto-test-window.h"
#include "moto-main-menu.h"

#include "libmoto/moto-render-node.h"
#include "libmoto/moto-world.h"
#include "libmoto/moto-system.h"
#include "libmoto/moto-node.h"
#include "libmotoutil/numdef.h"

/* forward */

void file_menu_quit(GtkMenuItem *item, gpointer user_data)
{
    gtk_main_quit();
}

void on_about_dialog_response(GtkDialog *dialog,
                              gint id,
                              gpointer   user_data)
{
    g_object_unref(dialog);
}

void on_help_about_activate(GtkMenuItem *item, gpointer user_data)
{
    gchar *filename = g_build_filename("resources", "glade", "about.glade", NULL);
    GladeXML *glade = glade_xml_new(filename, NULL, NULL);
    g_free(filename);

    GtkWidget *widget = glade_xml_get_widget(glade, "aboutdialog1");
    g_signal_connect(G_OBJECT(widget), "response", G_CALLBACK(on_about_dialog_response), NULL);

    gtk_widget_show_all(widget);

    g_object_unref(glade);
}

void open_script_editor(GtkMenuItem *item, gpointer user_data)
{
    /*
    GtkWidget *se = moto_script_editor_new();
    gtk_widget_show_all(se);
    */
}

typedef struct
{
    MotoSystem* system;
    GType type;
} MotoCreateRenderNodeData;

static void create_render_node(GtkMenuItem* item, MotoCreateRenderNodeData* data)
{
    if(!data->system)
    {
        // TODO: Error.
        return;
    }

    MotoSceneNode* w = moto_system_get_current_scene(data->system);
    if(!w)
    {
        // TODO: Error.
        return;
    }

    MotoNode* node = \
        moto_scene_node_create_node_by_name(w, g_type_name(data->type), "RenderMan", NULL);
}

static void free_MotoCreateRenderNodeData(gpointer data, GClosure *closure)
{
    g_slice_free(MotoCreateRenderNodeData, data);
}

static void on_create_render_node_activate(GtkMenuItem *item, gpointer user_data)
{
    GtkMenuShell *menu = (GtkMenuShell*)gtk_menu_item_get_submenu(item);

    GList* child = gtk_container_get_children((GtkContainer*)menu);
    for(; child; child = g_list_next(child))
    {
        gtk_container_remove((GtkContainer*)menu, child->data);
    }

    guint i, num;
    GType *t = g_type_children(MOTO_TYPE_RENDER_NODE, &num);
    for(i = 0; i < num; ++i)
    {
        GtkWidget *item = gtk_menu_item_new_with_label(g_type_name(t[i]));
        gtk_menu_shell_append(menu, item);

        MotoCreateRenderNodeData* data = g_slice_new(MotoCreateRenderNodeData);
        data->type = t[i];
        data->system = *((MotoSystem**)user_data);
        g_signal_connect_data(G_OBJECT(item), "activate", G_CALLBACK(create_render_node),
                              data, free_MotoCreateRenderNodeData, 0);
    }
    g_free(t);
    gtk_widget_show_all((GtkWidget*)menu);
}

static gboolean render(MotoSceneNode *scene_node, MotoRenderNode *node, gpointer user_data)
{
    g_print("render: %s\n", moto_node_get_name((MotoNode*)node));
    moto_render_node_render(node);

    return TRUE;
}

static void on_start_render_activate(GtkMenuItem *item, gpointer user_data)
{
    MotoSystem* system = *((MotoSystem**)user_data);

    MotoSceneNode* w = moto_system_get_current_scene(system);
    if(!w)
        return;

    g_print("w: %p\n", w);

    moto_scene_node_foreach_node(w, MOTO_TYPE_RENDER_NODE,
        (MotoSceneNodeForeachNodeFunc)render, NULL);
    g_print("---\n");
}

/* class MainMenu */

static GObjectClass *main_menu_parent_class = NULL;

struct _MotoMainMenuPriv
{
    gboolean disposed;
    MotoSystem* system;
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
    GtkMenuShell *menu_bar = (GtkMenuShell *)self;

    self->priv = g_slice_new(MotoMainMenuPriv);
    self->priv->system = NULL;

    /* Root menu items. */
    GtkWidget *file = gtk_menu_item_new_with_label("File");
    GtkWidget *project = gtk_menu_item_new_with_label("Project");
    GtkWidget *scene = gtk_menu_item_new_with_label("Scene");
    GtkWidget *node = gtk_menu_item_new_with_label("Node");
    GtkWidget *render = gtk_menu_item_new_with_label("Render");
    GtkWidget *window = gtk_menu_item_new_with_label("Window");
    GtkWidget *help = gtk_menu_item_new_with_label("Help");
    gtk_menu_shell_append(menu_bar, file);
    gtk_menu_shell_append(menu_bar, project);
    gtk_menu_shell_append(menu_bar, scene);
    gtk_menu_shell_append(menu_bar, node);
    gtk_menu_shell_append(menu_bar, render);
    gtk_menu_shell_append(menu_bar, window);
    gtk_menu_shell_append(menu_bar, help);

    GtkMenuItem *item;

    /* File menu */
    GtkMenuShell *file_menu = (GtkMenuShell *)gtk_menu_new();
    gtk_menu_item_set_submenu((GtkMenuItem *)file, (GtkWidget *)file_menu);

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Open");
    gtk_menu_shell_append(file_menu, (GtkWidget *)item);
    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Save");
    gtk_menu_shell_append(file_menu, (GtkWidget *)item);
    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Save As ...");
    gtk_menu_shell_append(file_menu, (GtkWidget *)item);

    gtk_menu_shell_append(file_menu, gtk_separator_menu_item_new());

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Quit");
    gtk_menu_shell_append(file_menu, (GtkWidget *)item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(file_menu_quit), NULL);

    /* Project menu */
    GtkMenuShell *project_menu = (GtkMenuShell *)gtk_menu_new();
    gtk_menu_item_set_submenu((GtkMenuItem *)project, (GtkWidget *)project_menu);

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("New");
    gtk_menu_shell_append(project_menu, (GtkWidget *)item);
    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Set");
    gtk_menu_shell_append(project_menu, (GtkWidget *)item);

    /* Render menu */
    GtkMenuShell *render_menu = (GtkMenuShell *)gtk_menu_new();
    gtk_menu_item_set_submenu((GtkMenuItem *)render, (GtkWidget *)render_menu);

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Create Render Node");
    gtk_menu_shell_append(render_menu, (GtkWidget *)item);
    gtk_menu_item_set_submenu((GtkMenuItem *)item, gtk_menu_new());
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(on_create_render_node_activate), &self->priv->system);

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Start Render");
    gtk_menu_shell_append(render_menu, (GtkWidget *)item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(on_start_render_activate), &self->priv->system);

    /* Window menu */
    GtkMenuShell *window_menu = (GtkMenuShell *)gtk_menu_new();
    gtk_menu_item_set_submenu((GtkMenuItem *)window, (GtkWidget *)window_menu);

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Script Editor");
    gtk_menu_shell_append(window_menu, (GtkWidget *)item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(open_script_editor), NULL);

    /* Help menu */
    GtkMenuShell *help_menu = (GtkMenuShell *)gtk_menu_new();
    gtk_menu_item_set_submenu((GtkMenuItem *)help, (GtkWidget *)help_menu);

    item = (GtkMenuItem *)gtk_menu_item_new_with_label("Help");
    gtk_menu_shell_append(help_menu, (GtkWidget *)item);
    item = (GtkMenuItem *)gtk_menu_item_new_with_label("About");
    gtk_menu_shell_append(help_menu, (GtkWidget *)item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(on_help_about_activate), NULL);

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

/* Methods of class MainMenu */

GtkWidget *moto_main_menu_new(MotoSystem* system)
{
    GtkWidget *self = (GtkWidget *)g_object_new(MOTO_TYPE_MAIN_MENU, NULL);
    ((MotoMainMenu*)self)->priv->system = system;

    return self;
}


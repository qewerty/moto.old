#include "motocore/moto-messager.h"
#include "motocore/moto-world.h"
#include "moto-shelf.h"

/* Some items */

static void create_mesh_plane(MotoShelf *shelf, MotoSystem *system)
{
    if( ! system)
    {
        moto_error("MotoShelf has no associated system.");
        return;
    }

    MotoWorld *w = moto_system_get_current_world(system);
    if( ! w)
    {
        moto_error("MotoSystem associated with the shelf has no current world.");
        return;
    }

    MotoNode *root_node = (MotoNode *)moto_world_get_root(w);

    MotoNode *obj_node = moto_world_create_node(w, "MotoObjectNode", "CubeObject", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    moto_world_set_object_current(w, (MotoObjectNode *)obj_node);

    MotoNode *view_node = moto_world_create_node(w, "MotoMeshViewNode", "MeshView", NULL);
    MotoNode *plane_node = moto_world_create_node(w, "MotoPlaneNode", "Plane", NULL);
    MotoNode *mat_node = moto_world_create_node(w, "MotoSlerMaterialNode", "Material1", NULL);

    moto_node_link(obj_node, "view", view_node, "view");
    moto_node_link(view_node, "mesh", plane_node, "mesh");
    moto_node_link(obj_node, "material", mat_node, "material");
}

static void create_mesh_cube(MotoShelf *shelf, MotoSystem *system)
{
    if( ! system)
    {
        moto_error("MotoShelf has no associated system.");
        return;
    }

    MotoWorld *w = moto_system_get_current_world(system);
    if( ! w)
    {
        moto_error("MotoSystem associated with the shelf has no current world.");
        return;
    }

    MotoNode *root_node = (MotoNode *)moto_world_get_root(w);

    MotoNode *obj_node = moto_world_create_node(w, "MotoObjectNode", "CubeObject", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    moto_world_set_object_current(w, (MotoObjectNode *)obj_node);

    MotoNode *view_node = moto_world_create_node(w, "MotoMeshViewNode", "MeshView", NULL);
    MotoNode *cube_node = moto_world_create_node(w, "MotoCubeNode", "Cube", NULL);
    MotoNode *mat_node = moto_world_create_node(w, "MotoSlerMaterialNode", "Material1", NULL);

    moto_node_link(obj_node, "view", view_node, "view");
    moto_node_link(view_node, "mesh", cube_node, "mesh");
    moto_node_link(obj_node, "material", mat_node, "material");
}

/* class MotoShelf */

static GObjectClass *shelf_parent_class = NULL;

struct _MotoShelfPriv
{
    gboolean disposed;

    GData *tabs;

    MotoSystem *system;
};

static void
moto_shelf_dispose(GObject *obj)
{
    MotoShelf *self = (MotoShelf *)obj;

    if(self->priv->disposed)
        return;

    self->priv->disposed = TRUE;

    shelf_parent_class->dispose(obj);
}

static void
moto_shelf_finalize(GObject *obj)
{
    g_slice_free(MotoShelfPriv, ((MotoShelf *)obj)->priv);
    shelf_parent_class->finalize(obj);
}

static void
moto_shelf_init(MotoShelf *self)
{
    self->priv = g_slice_new(MotoShelfPriv);
    self->priv->disposed = FALSE;

    self->priv->system = NULL;
    g_datalist_init(& self->priv->tabs);

    gtk_notebook_set_tab_border((GtkNotebook *)self, 0);
}

static void
moto_shelf_class_init(MotoShelfClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    goclass->dispose    = moto_shelf_dispose;
    goclass->finalize   = moto_shelf_finalize;
}

G_DEFINE_TYPE(MotoShelf, moto_shelf, GTK_TYPE_NOTEBOOK);

GtkWidget *moto_shelf_new(MotoSystem *system)
{
    MotoShelf *self = (MotoShelf *)g_object_new(MOTO_TYPE_SHELF, NULL);

    self->priv->system = system;

    moto_shelf_add_tab(self, "Mesh");
    moto_shelf_add_tab(self, "Model");
    moto_shelf_add_tab(self, "Anim");

    moto_shelf_add_item(self, "Mesh", "Plane", create_mesh_plane);
    moto_shelf_add_item(self, "Mesh", "Cube", create_mesh_cube);

    moto_shelf_add_item(self, "Anim", "BlendShape", NULL);

    return (GtkWidget *)self;
}

void moto_shelf_add_tab(MotoShelf *self, const gchar *name)
{
    GtkNotebook *nb = (GtkNotebook *)self;

    GtkBox *hbox = (GtkBox *)gtk_hbox_new(FALSE, 0);

    g_datalist_set_data(& self->priv->tabs, name, hbox);

    gtk_notebook_append_page(nb, (GtkWidget *)hbox, gtk_label_new(name));
}

typedef struct _ShelfItemData
{
    MotoShelf *shelf;
    MotoSystem *system;
    MotoShelfFunc func;
} ShelfItemData;

static void on_item(GtkButton *button, ShelfItemData *data)
{
    if( ! data || ! data->func || ! data->shelf || ! data->system)
        return;

    data->func(data->shelf, data->system);
}

void moto_shelf_add_item(MotoShelf *self, const gchar *tab_name, const gchar *name, MotoShelfFunc func)
{
    GtkBox *tab = g_datalist_get_data(& self->priv->tabs, tab_name);
    if( ! tab)
        return;

    ShelfItemData *data = g_slice_new(ShelfItemData);
    data->shelf = self;
    data->system = self->priv->system;
    data->func = func;

    GtkButton *b = gtk_button_new_with_label(name);
    g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(on_item), data);
    gtk_box_pack_start(tab, b, FALSE, FALSE, 0);

    gtk_widget_show_all((GtkWidget *)tab);
}

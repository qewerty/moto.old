#include "libmoto/moto-messager.h"
#include "libmoto/moto-world.h"
#include "libmoto/moto-mesh-view-node.h"
#include "libmoto/moto-extrude-node.h"
#include "libmoto/moto-remove-faces-node.h"

#include "moto-shelf.h"
#include "moto-test-window.h"

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

    MotoNode *obj_node = moto_world_create_node_by_name(w, "MotoObjectNode", "CubeObject", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    moto_world_set_object_current(w, (MotoObjectNode *)obj_node);

    MotoNode *view_node = moto_world_create_node_by_name(w, "MotoMeshViewNode", "MeshView", NULL);
    MotoNode *plane_node = moto_world_create_node_by_name(w, "MotoPlaneNode", "Plane", NULL);
    MotoNode *mat_node = moto_world_create_node_by_name(w, "MotoSlerMaterialNode", "Material1", NULL);

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

    MotoNode *obj_node = moto_world_create_node_by_name(w, "MotoObjectNode", "CubeObject", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    moto_world_set_object_current(w, (MotoObjectNode *)obj_node);

    MotoNode *view_node = moto_world_create_node_by_name(w, "MotoMeshViewNode", "MeshView", NULL);
    MotoNode *cube_node = moto_world_create_node_by_name(w, "MotoCubeNode", "Cube", NULL);
    // MotoNode *mod_node = moto_world_create_node_by_name(w, "MotoExtrudeNode", "Extrude", NULL);
    MotoNode *mat_node = moto_world_create_node_by_name(w, "MotoSlerMaterialNode", "Material1", NULL);

    moto_node_link(obj_node,  "view", view_node, "view");
    // moto_node_link(mod_node,  "in_mesh", cube_node, "mesh");
    // moto_node_link(view_node, "mesh", mod_node, "out_mesh");
    moto_node_link(view_node, "mesh", cube_node, "mesh");
    moto_node_link(obj_node,  "material", mat_node, "material");
}

static void create_mesh_sphere(MotoShelf *shelf, MotoSystem *system)
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

    MotoNode *obj_node = moto_world_create_node_by_name(w, "MotoObjectNode", "CubeObject", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    moto_world_set_object_current(w, (MotoObjectNode *)obj_node);

    MotoNode *view_node = moto_world_create_node_by_name(w, "MotoMeshViewNode", "MeshView", NULL);
    MotoNode *sphere_node = moto_world_create_node_by_name(w, "MotoSphereNode", "Cube", NULL);
    MotoNode *mat_node = moto_world_create_node_by_name(w, "MotoSlerMaterialNode", "Material1", NULL);

    moto_node_link(obj_node, "view", view_node, "view");
    moto_node_link(view_node, "mesh", sphere_node, "mesh");
    moto_node_link(obj_node, "material", mat_node, "material");
}

static void create_mesh_cylinder(MotoShelf *shelf, MotoSystem *system)
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

    MotoNode *obj_node = moto_world_create_node_by_name(w, "MotoObjectNode", "CubeObject", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    moto_world_set_object_current(w, (MotoObjectNode *)obj_node);

    MotoNode *view_node = moto_world_create_node_by_name(w, "MotoMeshViewNode", "MeshView", NULL);
    MotoNode *cylinder_node = moto_world_create_node_by_name(w, "MotoCylinderNode", "Cube", NULL);
    MotoNode *mat_node = moto_world_create_node_by_name(w, "MotoSlerMaterialNode", "Material1", NULL);

    moto_node_link(obj_node, "view", view_node, "view");
    moto_node_link(view_node, "mesh", cylinder_node, "mesh");
    moto_node_link(obj_node, "material", mat_node, "material");
}

static void create_mesh_file(MotoShelf *shelf, MotoSystem *system)
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

    MotoNode *obj_node = moto_world_create_node_by_name(w, "MotoObjectNode", "CubeObject", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    moto_world_set_object_current(w, (MotoObjectNode *)obj_node);

    MotoNode *view_node = moto_world_create_node_by_name(w, "MotoMeshViewNode", "MeshView", NULL);
    MotoNode *mf_node = moto_world_create_node_by_name(w, "MotoMeshFileNode", "Cube", NULL);
    MotoNode *mat_node = moto_world_create_node_by_name(w, "MotoSlerMaterialNode", "Material1", NULL);
    MotoNode *mod = moto_world_create_node_by_name(w, "MotoBendNode", "Bend", NULL);

    moto_node_link(obj_node, "view", view_node, "view");
    moto_node_link(mod, "in_pc", mf_node, "mesh");
    moto_node_link(view_node, "mesh", mod, "out_pc");
    moto_node_link(obj_node, "material", mat_node, "material");
}

static void create_normal_move(MotoShelf *shelf, MotoSystem *system)
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

    MotoNode *node = moto_world_create_node_by_name(w, "MotoDisplaceNode", "NormalMove", NULL);
}

static void perform_mesh_op(MotoShelf *shelf, MotoSystem *system, const gchar *op_node_name)
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

    MotoObjectNode *obj = moto_world_get_current_object(w);
    if(!obj)
    {
        moto_error("No current object to apply extrude.");
        return;
    }

    MotoMeshViewNode *view;
    moto_node_get_param_object((MotoNode *)obj, "view", (GObject**)&view);

    if( ! view)
    {
        moto_error("Current object has no view.");
        return;
    }

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(view), MOTO_TYPE_MESH_VIEW_NODE))
    {
        moto_error("View of current object is not MotoMeshViewNode.");
        return;
    }

    MotoParam *param  = moto_node_get_param((MotoNode*)view, "mesh");
    MotoParam *source = moto_param_get_source(param);
    if( ! source)
    {
        moto_error("View of current object has not associated geometry.");
        return;
    }

    MotoRemoveFacesNode *op = \
        (MotoRemoveFacesNode*)moto_world_create_node_by_name(w, op_node_name, "Op", NULL);
    if( ! op || ! g_type_is_a(G_TYPE_FROM_INSTANCE(op), MOTO_TYPE_MESH_OP_NODE))
        return;

    MotoGeomViewState *state = moto_geom_view_node_get_state((MotoGeomViewNode*)view);
    if(state)
        moto_geom_view_state_leave(state, (MotoGeomViewNode*)view);

    MotoMeshSelection *selection = moto_mesh_view_node_get_selection(view);
    moto_mesh_op_node_set_selection((MotoMeshOpNode*)op, selection);
    moto_mesh_selection_deselect_all(selection);

    MotoParam *in_mesh = moto_node_get_param((MotoNode*)op, "in_mesh");
    moto_param_link(in_mesh, source);

    MotoParam *out_mesh = moto_node_get_param((MotoNode*)op, "out_mesh");
    moto_param_link(param, out_mesh);
}

static void perform_pc_op(MotoShelf *shelf, MotoSystem *system, const gchar *op_node_name)
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

    MotoObjectNode *obj = moto_world_get_current_object(w);
    if(!obj)
    {
        moto_error("No current object to apply extrude.");
        return;
    }

    MotoMeshViewNode *view;
    moto_node_get_param_object((MotoNode *)obj, "view", (GObject**)&view);

    if( ! view)
    {
        moto_error("Current object has no view.");
        return;
    }

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(view), MOTO_TYPE_MESH_VIEW_NODE))
    {
        moto_error("View of current object is not MotoMeshViewNode.");
        return;
    }

    MotoParam *param  = moto_node_get_param((MotoNode*)view, "mesh");
    MotoParam *source = moto_param_get_source(param);
    if( ! source)
    {
        moto_error("View of current object has not associated geometry.");
        return;
    }

    MotoRemoveFacesNode *op = \
        (MotoRemoveFacesNode*)moto_world_create_node_by_name(w, op_node_name, "Op", NULL);
    if( ! op)
        return;

    MotoParam *in = moto_node_get_param((MotoNode*)op, "in_pc");
    moto_param_link(in, source);

    MotoParam *out = moto_node_get_param((MotoNode*)op, "out_pc");
    moto_param_link(param, out);
}

static void perform_extrude(MotoShelf *shelf, MotoSystem *system)
{
    perform_mesh_op(shelf, system, "MotoExtrudeNode");
}

static void perform_remove_faces(MotoShelf *shelf, MotoSystem *system)
{
    perform_mesh_op(shelf, system, "MotoRemoveFacesNode");
}

static void perform_twist(MotoShelf *shelf, MotoSystem *system)
{
    perform_pc_op(shelf, system, "MotoTwistNode");
}

static void perform_bend(MotoShelf *shelf, MotoSystem *system)
{
    perform_pc_op(shelf, system, "MotoBendNode");
}

static void perform_displace(MotoShelf *shelf, MotoSystem *system)
{
    perform_pc_op(shelf, system, "MotoDisplaceNode");
}

/* class MotoShelf */

static GObjectClass *shelf_parent_class = NULL;

struct _MotoShelfPriv
{
    gboolean disposed;

    GData *tabs;

    MotoSystem *system;
    GtkWindow *window;
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

    self->priv->window = NULL;

    gtk_notebook_set_tab_border((GtkNotebook *)self, 0);
}

static void
moto_shelf_class_init(MotoShelfClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    shelf_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_shelf_dispose;
    goclass->finalize   = moto_shelf_finalize;
}

G_DEFINE_TYPE(MotoShelf, moto_shelf, GTK_TYPE_NOTEBOOK);

GtkWidget *moto_shelf_new(MotoSystem *system, GtkWindow *window)
{
    MotoShelf *self = (MotoShelf *)g_object_new(MOTO_TYPE_SHELF, NULL);

    self->priv->system = system;
    self->priv->window = window;

    moto_shelf_add_tab(self, "Geom");

    moto_shelf_add_item(self, "Geom", "Plane",    create_mesh_plane);
    moto_shelf_add_item(self, "Geom", "Cube",     create_mesh_cube);
    moto_shelf_add_item(self, "Geom", "Sphere",   create_mesh_sphere);
    moto_shelf_add_item(self, "Geom", "Cylinder", create_mesh_cylinder);
    moto_shelf_add_item(self, "Geom", "File",     create_mesh_file);

    moto_shelf_add_tab(self,  "Model");
    moto_shelf_add_item(self, "Model", "Remove",  perform_remove_faces);
    moto_shelf_add_item(self, "Model", "Extrude",  perform_extrude);
    moto_shelf_add_item(self, "Model", "Bevel",    NULL);
    moto_shelf_add_item(self, "Model", "Collapse", NULL);

    moto_shelf_add_tab(self, "Anim");
    moto_shelf_add_item(self, "Anim", "Morph", NULL);
    moto_shelf_add_item(self, "Anim", "Lattice", NULL);

    moto_shelf_add_tab(self, "Render");
    moto_shelf_add_item(self, "Render", "Camera", NULL);
    moto_shelf_add_item(self, "Render", "Light", NULL);

    moto_shelf_add_tab(self, "Deform");
    moto_shelf_add_item(self, "Deform", "Twist", perform_twist);
    moto_shelf_add_item(self, "Deform", "Bend", perform_bend);
    moto_shelf_add_item(self, "Deform", "Displace", perform_displace);

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
    if( ! data || ! data->func || ! data->shelf || ! data->shelf->priv->window || ! data->system)
        return;

    data->func(data->shelf, data->system);
    moto_test_window_redraw_3dview(data->shelf->priv->window);
    moto_test_window_update_param_editor(data->shelf->priv->window);
    moto_test_window_update_outliner(data->shelf->priv->window);
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
    gtk_button_set_relief(b, GTK_RELIEF_NONE);
    g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(on_item), data);
    gtk_box_pack_start(tab, b, FALSE, FALSE, 0);

    gtk_widget_show_all((GtkWidget *)tab);
}

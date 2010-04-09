#include "libmoto/moto-messager.h"
#include "libmoto/moto-scene-node.h"
#include "libmoto/moto-object-node.h"
#include "libmoto/moto-shape-node.h"
#include "libmoto/moto-extrude-node.h"
#include "libmoto/moto-remove-node.h"
#include "moto-test-window.h"
#include "moto-shelf.h"

/* Some items */

static void 
create_shape(MotoShelf* shelf, MotoSystem* system,
                const char* type_name, const char* name)
{
    if(!system)
    {
        moto_error("MotoShelf has no associated system.");
        return;
    }

    MotoSceneNode *scene = moto_system_get_current_scene(system);
    if(!scene)
    {
        moto_error("System associated with the shelf has no current scene_node.");
        return;
    }

    MotoNode *obj = moto_node_create_child_by_name((MotoNode*)scene, "MotoObjectNode", name);
    moto_node_set_scene_node(obj, scene); // TODO: Remove function 'moto_node_set_scene_node' later.
    moto_scene_node_set_object_current(scene, (MotoObjectNode*)obj);

    MotoNode *plane = moto_node_create_child_by_name(obj, type_name, name);
    MotoNode *mat = moto_node_create_child_by_name(obj,  "MotoMaterialNode", "mat");

    moto_node_link(obj, "shape", plane, "self");
    moto_node_link(obj, "material", mat, "material");
}

static void create_mesh_plane(MotoShelf *shelf, MotoSystem *system)
{
    create_shape(shelf, system, "MotoPlaneNode", "plane");
}

static void create_mesh_cube(MotoShelf *shelf, MotoSystem *system)
{
    create_shape(shelf, system, "MotoCubeNode", "cube");
}

static void create_mesh_sphere(MotoShelf *shelf, MotoSystem *system)
{
    create_shape(shelf, system, "MotoSphereNode", "sphere");
}

static void create_mesh_cylinder(MotoShelf *shelf, MotoSystem *system)
{
    create_shape(shelf, system, "MotoCylinderNode", "cylinder");
}

static void create_mesh_file(MotoShelf *shelf, MotoSystem *system)
{
    create_shape(shelf, system, "MotoMeshFileNode", "mesh_file");
}

static void
perform_op(MotoShelf *shelf, MotoSystem *system,
                const gchar *op_node_name, const char* name)
{
    if(!system)
    {
        moto_error("MotoShelf has no associated system.");
        return;
    }

    MotoSceneNode *w = moto_system_get_current_scene(system);
    if(!w)
    {
        moto_error("MotoSystem associated with the shelf has no current scene_node.");
        return;
    }

    MotoObjectNode *obj = moto_scene_node_get_current_object(w);
    if(!obj)
    {
        moto_error("No current object to apply extrude.");
        return;
    }

    MotoShapeNode* shape = moto_object_node_get_shape(obj);

    if(!shape)
    {
        moto_error("Current object has no shape.");
        return;
    }

    MotoNode *op = \
        moto_node_create_child_by_name((MotoNode*)obj, op_node_name, name);
    if(!op || !g_type_is_a(G_TYPE_FROM_INSTANCE(op), MOTO_TYPE_OP_NODE))
        return;

    moto_node_link(op, "in", shape, "out");
    moto_node_link(obj, "shape", op, "self");

    MotoShapeSelection *selection = moto_object_node_get_selection(obj);
    moto_op_node_set_selection((MotoOpNode*)op, selection);
    moto_shape_selection_deselect_all(selection);
}

static void perform_extrude(MotoShelf *shelf, MotoSystem *system)
{
    perform_op(shelf, system, "MotoExtrudeNode", "extrude");
}

static void perform_remove(MotoShelf *shelf, MotoSystem *system)
{
    perform_op(shelf, system, "MotoRemoveNode", "remove");
}

static void perform_twist(MotoShelf *shelf, MotoSystem *system)
{
    perform_op(shelf, system, "MotoTwistNode", "twist");
}

static void perform_bend(MotoShelf *shelf, MotoSystem *system)
{
    perform_op(shelf, system, "MotoBendNode", "bend");
}

static void perform_displace(MotoShelf *shelf, MotoSystem *system)
{
    perform_op(shelf, system, "MotoDisplaceNode", "displace");
}

static void create_light(MotoShelf *shelf, MotoSystem *system)
{
    if(!system)
    {
        moto_error("MotoShelf has no associated system.");
        return;
    }

    MotoSceneNode *w = moto_system_get_current_scene(system);
    if( ! w)
    {
        moto_error("MotoSystem associated with the shelf has no current scene_node.");
        return;
    }

    MotoNode *object = moto_node_create_child_by_name((MotoNode*)w, "MotoObjectNode", "light");
    MotoNode *light = moto_node_create_child_by_name(object, "MotoLightNode", "model");

    moto_node_set_param_3f(object, "r", 90, 0, 0);
}

static void create_rman_node(MotoShelf *shelf, MotoSystem *system)
{
    if(!system)
    {
        moto_error("MotoShelf has no associated system.");
        return;
    }

    MotoSceneNode *w = moto_system_get_current_scene(system);
    if( ! w)
    {
        moto_error("MotoSystem associated with the shelf has no current scene_node.");
        return;
    }

    moto_node_create_child_by_name((MotoNode*)w, "MotoRManNode", "rman");
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
    moto_shelf_add_item(self, "Model", "Remove",  perform_remove);
    moto_shelf_add_item(self, "Model", "Extrude",  perform_extrude);
    moto_shelf_add_item(self, "Model", "Bevel",    NULL);
    moto_shelf_add_item(self, "Model", "Collapse", NULL);

    moto_shelf_add_tab(self, "Deform");
    moto_shelf_add_item(self, "Deform", "Twist", perform_twist);
    moto_shelf_add_item(self, "Deform", "Bend", perform_bend);
    moto_shelf_add_item(self, "Deform", "Displace", perform_displace);

    moto_shelf_add_tab(self,  "Paint");
    moto_shelf_add_item(self, "Paint", "Instance", NULL);

    moto_shelf_add_tab(self, "Anim");
    moto_shelf_add_item(self, "Anim", "Morph", NULL);
    moto_shelf_add_item(self, "Anim", "Lattice", NULL);

    moto_shelf_add_tab(self, "Render");
    moto_shelf_add_item(self, "Render", "Camera", NULL);
    moto_shelf_add_item(self, "Render", "Light", create_light);
    moto_shelf_add_item(self, "Render", "RenderMan", create_rman_node);

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


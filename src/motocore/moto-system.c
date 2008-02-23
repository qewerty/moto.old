#include "moto-system.h"

/* nodes */
#include "moto-object-node.h"
#include "moto-mesh-view-node.h"
#include "moto-nurbs-view-node.h"
#include "moto-cube-node.h"
#include "moto-sphere-node.h"
#include "moto-curve-node.h"
#include "moto-mesh-file-node.h"
#include "moto-revolve-node.h"
#include "moto-extrude-node.h"
#include "moto-bevel-node.h"
#include "moto-time-node.h"

/* image loaders */
#include "moto-png-image-loader.h"
#include "moto-tif-image-loader.h"
#include "moto-jpg-image-loader.h"
#include "moto-exr-image-loader.h"

/* mesh loaders*/
#include "moto-mbm-mesh-loader.h"
#include "moto-wobj-mesh-loader.h"
#include "moto-rib-mesh-loader.h"

/* class System */

static GObjectClass *system_parent_class = NULL;

struct _MotoSystemPriv
{
    MotoLibrary *library;

    GSList *worlds;
    MotoWorld *current_world;
};

static void
moto_system_dispose(GObject *obj)
{
    MotoSystem *self = (MotoSystem *)obj;

    g_string_free(self->priv->filename, TRUE);
    g_slice_free(MotoSystemPriv, self->priv);

    G_OBJECT_CLASS(system_parent_class)->dispose(obj);
}

static void
moto_system_finalize(GObject *obj)
{
    system_parent_class->finalize(obj);
}

static void
moto_system_init(MotoSystem *self)
{
    self->priv = g_slice_new(MotoSystemPriv);

    self->priv->filename = g_string_new("");
    self->priv->changed = FALSE;
}

static void
moto_system_class_init(MotoSystemClass *klass)
{
    system_parent_class = g_type_class_peek_parent(klass);

    system_parent_class->dispose = moto_system_dispose;
    system_parent_class->finalize = moto_system_finalize;
}

G_DEFINE_TYPE(MotoSystem, moto_system, G_TYPE_OBJECT);

MotoSystem *moto_system_new(const gchar *name, MotoLibrary *lib)
{
    MotoSystem *self = (MotoSystem *)g_object_new(MOTO_TYPE_SYSTEM, NULL);

    MotoLibrary *lib = self->priv->library = moto_library_new();

    moto_library_new_slot(lib, "node", MOTO_TYPE_NODE_FACTORY);
    moto_library_new_slot(lib, "image-loader", MOTO_TYPE_IMAGE_LOADER);
    moto_library_new_slot(lib, "mesh-loader", MOTO_TYPE_MESH_LOADER);

    /* built-in node types */

    MotoNodeFactory *fac;

    fac = moto_object_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_mesh_view_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_nurbs_view_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_cube_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_sphere_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_mesh_file_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_curve_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_revolve_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_mesh_extrude_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    fac = moto_mesh_bevel_node_factory_new();
    moto_library_new_entry(lib, "node",
            g_type_name(moto_node_factory_get_node_type(fac)), fac);

    /* built-in image loaders */

    MotoImageLoader *image_loader;

    image_loader = moto_png_image_loader_new(); /* Portable Network Graphics (*.png) */
    moto_library_new_entry(lib, "image-loader",
            g_type_name(G_TYPE_FROM_INSTANCE(image_loader)), image_loader);

    image_loader = moto_jpg_image_loader_new(); /* JPEG (*.jpg|*.jpeg) */
    moto_library_new_entry(lib, "image-loader",
            g_type_name(G_TYPE_FROM_INSTANCE(image_loader)), image_loader);

    image_loader = moto_tif_image_loader_new(); /* TIFF (*.tif|*.tiff) */
    moto_library_new_entry(lib, "image-loader",
            g_type_name(G_TYPE_FROM_INSTANCE(image_loader)), image_loader);

    image_loader = moto_exr_image_loader_new(); /* ILM OpenEXR HDR Format (*.exr) */
    moto_library_new_entry(lib, "image-loader",
            g_type_name(G_TYPE_FROM_INSTANCE(image_loader)), image_loader);

    /* built-in mesh loaders */

    MotoMeshLoader *mesh_loader;

    mesh_loader = moto_mbm_mesh_loader_new(); /* Moto Binary Mesh (*.mbm) */
    moto_library_new_entry(lib, "mesh-loader",
            g_type_name(G_TYPE_FROM_INSTANCE(mesh_loader)), mesh_loader);

    mesh_loader = moto_wobj_mesh_loader_new(); /* Wavefront Object (*.obj) */
    moto_library_new_entry(lib, "mesh-loader",
            g_type_name(G_TYPE_FROM_INSTANCE(mesh_loader)), mesh_loader);

    mesh_loader = moto_rib_mesh_loader_new(); /* RenderMan Interface Bytestream (*.rib) */
    moto_library_new_entry(lib, "mesh-loader",
            g_type_name(G_TYPE_FROM_INSTANCE(mesh_loader)), mesh_loader);

    return NULL;
}

MotoWorld *moto_system_get_world(MotoSystem *self, const gchar *name)
{
    GSList *w = self->priv->worlds;
    for(; w; w = g_slist_next(w))
        if(g_utf8_collate(moto_world_get_name((MotoWorld *)w->data), name) == 0)
            return (MotoWorld *)w->data;
    return NULL;
}

MotoWorld *moto_system_get_current_world(MotoSystem *self)
{
    return self->priv->current_world;
}

void moto_system_add_world(MotoSystem *self, MotoWorld *world, gboolean set_current)
{
    if( ! moto_system_get_world(self, moto_world_get_name(world)))
        self->priv->worlds = g_slist_append(self->priv->worlds, world);

    if(set_current)
        self->priv->current_world = world;
}

void moto_system_set_world_current(MotoSystem *self, MotoWorld *world)
{
    GSList *w = self->priv->worlds;
    for(; w; w = g_slist_next(w))
    {
        if(w->data == world)
        {
            self->priv->current_world = world;
            return;
        }
    }

    GString *msg = g_string_new("World \"");
    g_string_append(msg, moto_world_get_name(world));
    g_string_append(msg, "\" is not within the system. I won't set it current.");
    moto_warning(msg->str);
    g_string_free(msg, TRUE);
}

MotoLibrary *moto_system_get_library(MotoSystem *self)
{
    return self->priv->library;
}

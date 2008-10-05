#include "moto-system.h"
#include "moto-world.h"
#include "moto-library.h"
#include "moto-node.h"
#include "moto-messager.h"

#include "moto-image-loader.h"
#include "moto-mesh-loader.h"

/* nodes */
#include "moto-object-node.h"
#include "moto-camera-node.h"
#include "moto-sler-material-node.h"
#include "moto-mesh-view-node.h"
#include "moto-ray-view-node.h"
#include "moto-grid-view-node.h"
#include "moto-axes-view-node.h"
// #include "moto-nurbs-view-node.h"
#include "moto-plane-node.h"
#include "moto-cube-node.h"
#include "moto-sphere-node.h"
#include "moto-cylinder-node.h"
// #include "moto-curve-node.h"
#include "moto-mesh-file-node.h"
// #include "moto-revolve-node.h"
// #include "moto-extrude-node.h"
// #include "moto-bevel-node.h"
// #include "moto-time-node.h"

/* image loaders */
/*
#include "moto-png-image-loader.h"
#include "moto-tif-image-loader.h"
#include "moto-jpg-image-loader.h"
#include "moto-exr-image-loader.h"
*/

/* mesh loaders*/

#ifdef MOTO_MBM_MESH_LOADER
#include "moto-mbm-mesh-loader.h"
#endif

#ifdef MOTO_WOBJ_MESH_LOADER
#include "moto-wobj-mesh-loader.h"
#endif

#ifdef MOTO_RIB_MESH_LOADER
#include "moto-rib-mesh-loader.h"
#endif

/* utils */

static void unref_gobject(gpointer data, gpointer user_data)
{
    g_object_unref(G_OBJECT(data));
}

/* class System */

static GObjectClass *system_parent_class = NULL;

struct _MotoSystemPriv
{
    MotoLibrary *library;

    GSList *worlds;
    MotoWorld *current_world;

    GMutex *world_list_mutex;
    GMutex *current_world_mutex;
    GMutex *library_mutex;
};

static void
moto_system_dispose(GObject *obj)
{
    MotoSystem *self = (MotoSystem *)obj;

    // FIXME: Not thread-safe access to self->priv->worlds!!!
    g_slist_foreach(self->priv->worlds, unref_gobject, NULL);
    g_slist_free(self->priv->worlds);
    g_object_unref(self->priv->library);
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

    self->priv->library = moto_library_new();

    self->priv->world_list_mutex    = g_mutex_new();
    self->priv->current_world_mutex = g_mutex_new();
    self->priv->library_mutex       = g_mutex_new();
}

static void
moto_system_class_init(MotoSystemClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    system_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_system_dispose;
    goclass->finalize   = moto_system_finalize;

    /* init node types */
    MOTO_TYPE_NODE;
        MOTO_TYPE_OBJECT_NODE;
        MOTO_TYPE_GEOMETRY_NODE;
            MOTO_TYPE_PLANE_NODE;
            MOTO_TYPE_CUBE_NODE;
            MOTO_TYPE_SPHERE_NODE;
            MOTO_TYPE_CYLINDER_NODE;
            MOTO_TYPE_MESH_FILE_NODE;
        MOTO_TYPE_GEOMETRY_VIEW_NODE;
            MOTO_TYPE_MESH_VIEW_NODE;
            MOTO_TYPE_GRID_VIEW_NODE;
            MOTO_TYPE_AXES_VIEW_NODE;
            MOTO_TYPE_RAY_VIEW_NODE;
        MOTO_TYPE_MATERIAL_NODE;
            MOTO_TYPE_SLER_MATERIAL_NODE;
}

G_DEFINE_TYPE(MotoSystem, moto_system, G_TYPE_OBJECT);

MotoSystem *moto_system_new()
{
    MotoSystem *self = (MotoSystem *)g_object_new(MOTO_TYPE_SYSTEM, NULL);

    g_mutex_lock(self->priv->library_mutex);
    MotoLibrary *lib = self->priv->library;
    g_mutex_unlock(self->priv->library_mutex);

    return self;
}

MotoWorld *moto_system_get_world(MotoSystem *self, const gchar *name)
{
    g_mutex_lock(self->priv->world_list_mutex);
    MotoWorld *world = NULL;
    GSList *w = self->priv->worlds;
    for(; w; w = g_slist_next(w))
    {
        if(g_utf8_collate(moto_world_get_name((MotoWorld *)w->data), name) == 0)
        {
            world = (MotoWorld *)w->data;
            break;
        }
    }
    g_mutex_unlock(self->priv->world_list_mutex);
    return world;
}

MotoWorld *moto_system_get_current_world(MotoSystem *self)
{
    g_mutex_lock(self->priv->current_world_mutex);
    MotoWorld *world = self->priv->current_world;
    g_mutex_unlock(self->priv->current_world_mutex);
    return world;
}

void moto_system_add_world(MotoSystem *self, MotoWorld *world, gboolean set_current)
{
    if(moto_system_get_world(self, moto_world_get_name(world)))
    {
        moto_warning("World \"%s\" is already in the system. I won't add it again.", moto_world_get_name(world));

        /* If set_current is TRUE, world will be set as current in any case. */
        if(set_current)
            moto_system_set_world_current(self, world);

        return;
    }

    g_mutex_lock(self->priv->world_list_mutex);
    g_object_ref(G_OBJECT(world));
    self->priv->worlds = g_slist_append(self->priv->worlds, world);
    g_mutex_unlock(self->priv->world_list_mutex);

    if(set_current)
        moto_system_set_world_current(self, world);
}

void moto_system_delete_world(MotoSystem *self, MotoWorld *world)
{
    g_mutex_lock(self->priv->world_list_mutex);
    if( ! g_slist_find(self->priv->worlds, world))
    {
        moto_warning("World \"%s\" is not in the system. Nothing to delete.", moto_world_get_name(world));

        g_mutex_unlock(self->priv->world_list_mutex);
        return;
    }
    self->priv->worlds = g_slist_remove(self->priv->worlds, world);
    g_mutex_unlock(self->priv->world_list_mutex);

    g_object_unref(G_OBJECT(world));
}

void moto_system_delete_world_by_name(MotoSystem *self, const gchar *world_name)
{
    MotoWorld *world = moto_system_get_world(self, world_name);

    if( ! world)
    {
        moto_error("System has no world with name \"%s\". Nothing to delete.", world_name);
        return;
    }

    moto_system_delete_world(self, world);
}

void moto_system_set_world_current(MotoSystem *self, MotoWorld *world)
{
    g_mutex_lock(self->priv->world_list_mutex);
    if( ! g_slist_find(self->priv->worlds, world))
    {
        g_mutex_unlock(self->priv->world_list_mutex);
        moto_warning("World \"%s\" is not in the system. I won't set it current.", moto_world_get_name(world));
        return;
    }
    g_mutex_unlock(self->priv->world_list_mutex);

    g_mutex_lock(self->priv->current_world_mutex);
    self->priv->current_world = world;
    g_mutex_unlock(self->priv->current_world_mutex);
}

MotoLibrary *moto_system_get_library(MotoSystem *self)
{
    g_mutex_lock(self->priv->library_mutex);
    MotoLibrary *lib = self->priv->library;
    g_mutex_unlock(self->priv->library_mutex);
    return lib;
}

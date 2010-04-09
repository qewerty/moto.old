#include <Python.h>

#include "moto-system.h"
#include "moto-scene-node.h"
#include "moto-library.h"
#include "moto-node.h"
#include "moto-messager.h"

// #include "moto-image-loader.h"
#include "moto-mesh-loader.h"

/* nodes */
#include "moto-render-node.h"
#include "moto-rman-node.h"
#include "moto-displace-node.h"
#include "moto-twist-node.h"
#include "moto-bend-node.h"
#include "moto-extrude-node.h"
#include "moto-remove-node.h"
#include "moto-object-node.h"
#include "moto-material-node.h"
#include "moto-grid-node.h"
#include "moto-axes-node.h"
#include "moto-light-node.h"
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

#ifdef MOTO_WITH_MBM_MESH_LOADER
#include "moto-mbm-mesh-loader.h"
#endif

#ifdef MOTO_WITH_WOBJ_MESH_LOADER
#include "moto-wobj-mesh-loader.h"
#endif

#ifdef MOTO_WOTH_RIB_MESH_LOADER
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

    GSList *scenes;
    MotoSceneNode *current_scene;
};

static void
moto_system_dispose(GObject *obj)
{
    MotoSystem *self = (MotoSystem *)obj;

    // FIXME: Not thread-safe access to self->priv->scenes!!!
    g_slist_foreach(self->priv->scenes, unref_gobject, NULL);
    g_slist_free(self->priv->scenes);
    g_object_unref(self->priv->library);
    g_slice_free(MotoSystemPriv, self->priv);

    G_OBJECT_CLASS(system_parent_class)->dispose(obj);
}

static void
moto_system_finalize(GObject *obj)
{
    Py_Finalize();

    system_parent_class->finalize(obj);
}

static void
moto_system_init(MotoSystem *self)
{
    Py_Initialize();

    self->priv = g_slice_new(MotoSystemPriv);

    self->priv->library = moto_library_new();

    moto_library_new_slot(self->priv->library, "mesh-loader", MOTO_TYPE_MESH_LOADER);

#ifdef MOTO_WITH_WOBJ_MESH_LOADER
    moto_library_new_entry(self->priv->library, "mesh-loader",
            g_type_name(MOTO_TYPE_WOBJ_MESH_LOADER), moto_wobj_mesh_loader_new());
#endif
}

static void
moto_system_class_init(MotoSystemClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    system_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_system_dispose;
    goclass->finalize   = moto_system_finalize;

    GType node_created_types[] = {MOTO_TYPE_NODE};
    klass->node_created_signal_id = \
        g_signal_newv ("node-created",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE /* return_type */,
                 1     /* n_params */,
                 node_created_types  /* param_types */);

    /* init node types */
    MOTO_TYPE_NODE;
        MOTO_TYPE_RENDER_NODE;
            MOTO_TYPE_RMAN_NODE;
        MOTO_TYPE_OBJECT_NODE;
        MOTO_TYPE_SHAPE_NODE;
            MOTO_TYPE_PLANE_NODE;
            MOTO_TYPE_CUBE_NODE;
            MOTO_TYPE_SPHERE_NODE;
            MOTO_TYPE_CYLINDER_NODE;
            MOTO_TYPE_MESH_FILE_NODE;
            MOTO_TYPE_GRID_NODE;
            MOTO_TYPE_AXES_NODE;
            MOTO_TYPE_LIGHT_NODE;
        MOTO_TYPE_MATERIAL_NODE;
        MOTO_TYPE_DISPLACE_NODE;
        MOTO_TYPE_TWIST_NODE;
        MOTO_TYPE_BEND_NODE;
        MOTO_TYPE_EXTRUDE_NODE;
        MOTO_TYPE_REMOVE_NODE;
}

G_DEFINE_TYPE(MotoSystem, moto_system, G_TYPE_OBJECT);

MotoSystem *moto_system_new()
{
    MotoSystem *self = (MotoSystem *)g_object_new(MOTO_TYPE_SYSTEM, NULL);

    // MotoLibrary *lib = self->priv->library;

    return self;
}

MotoSceneNode *moto_system_get_scene_node(MotoSystem *self, const gchar *name)
{
    MotoSceneNode *scene_node = NULL;
    GSList *w = self->priv->scenes;
    for(; w; w = g_slist_next(w))
    {
        if(g_utf8_collate(moto_scene_node_get_name((MotoSceneNode *)w->data), name) == 0)
        {
            scene_node = (MotoSceneNode *)w->data;
            break;
        }
    }
    return scene_node;
}

MotoSceneNode *moto_system_get_current_scene(MotoSystem *self)
{
    MotoSceneNode *scene_node = self->priv->current_scene;
    return scene_node;
}

void moto_system_add_scene(MotoSystem *self, MotoSceneNode *scene_node, gboolean set_current)
{
    if(moto_system_get_scene_node(self, moto_scene_node_get_name(scene_node)))
    {
        moto_warning("SceneNode \"%s\" is already in the system. I won't add it again.", moto_scene_node_get_name(scene_node));

        /* If set_current is TRUE, scene_node will be set as current in any case. */
        if(set_current)
            moto_system_set_scene_current(self, scene_node);

        return;
    }

    g_object_ref(G_OBJECT(scene_node));
    self->priv->scenes = g_slist_append(self->priv->scenes, scene_node);

    if(set_current)
        moto_system_set_scene_current(self, scene_node);
}

void moto_system_delete_scene_node(MotoSystem *self, MotoSceneNode *scene_node)
{
    if( ! g_slist_find(self->priv->scenes, scene_node))
    {
        moto_warning("SceneNode \"%s\" is not in the system. Nothing to delete.", moto_scene_node_get_name(scene_node));

        return;
    }
    self->priv->scenes = g_slist_remove(self->priv->scenes, scene_node);

    g_object_unref(G_OBJECT(scene_node));
}

void moto_system_delete_scene_node_by_name(MotoSystem *self, const gchar *scene_node_name)
{
    MotoSceneNode *scene_node = moto_system_get_scene_node(self, scene_node_name);

    if( ! scene_node)
    {
        moto_error("System has no scene_node with name \"%s\". Nothing to delete.", scene_node_name);
        return;
    }

    moto_system_delete_scene_node(self, scene_node);
}

void moto_system_set_scene_current(MotoSystem *self, MotoSceneNode *scene_node)
{
    if( ! g_slist_find(self->priv->scenes, scene_node))
    {
        moto_warning("SceneNode \"%s\" is not in the system. I won't set it current.", moto_scene_node_get_name(scene_node));
        return;
    }

    self->priv->current_scene = scene_node;
}

MotoLibrary *moto_system_get_library(MotoSystem *self)
{
    MotoLibrary *lib = self->priv->library;
    return lib;
}

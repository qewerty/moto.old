#include <stdlib.h>
#include <string.h>

#include <gio/gio.h>

#include "moto-point-cloud.h"
#include "moto-filename.h"
#include "moto-mesh-file-node.h"
#include "moto-library.h"
#include "moto-messager.h"
#include "moto-mesh-loader.h"
#include "libmotoutil/xform.h"

/* forwards */

static MotoBound *moto_mesh_file_node_get_bound(MotoShapeNode *self);

/*
static MotoMesh *moto_mesh_file_node_get_mesh(MotoMeshFileNode *self);
*/

static MotoMesh *moto_mesh_file_node_load(MotoMeshFileNode *self, const gchar *filename);
static void moto_mesh_file_node_update(MotoNode *self);

/* class MeshFileNode */

typedef struct _MotoMeshFileNodePriv MotoMeshFileNodePriv;

#define MOTO_MESH_FILE_NODE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_MESH_FILE_NODE, MotoMeshFileNodePriv)

static GObjectClass *mesh_file_node_parent_class = NULL;

struct _MotoMeshFileNodePriv
{
    MotoMesh *mesh;

    MotoBound *bound;
    gboolean bound_calculated;
};

static void
moto_mesh_file_node_dispose(GObject *obj)
{
    MotoMeshFileNodePriv *priv = MOTO_MESH_FILE_NODE_GET_PRIVATE(obj);

    if(priv->mesh)
        g_object_unref((GObject *)priv->mesh);

    mesh_file_node_parent_class->dispose(obj);
}

static void
moto_mesh_file_node_finalize(GObject *obj)
{
    mesh_file_node_parent_class->finalize(obj);
}

static void
moto_mesh_file_node_init(MotoMeshFileNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoMeshFileNodePriv *priv = MOTO_MESH_FILE_NODE_GET_PRIVATE(self);

    priv->mesh = NULL;

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "filename", "Filename", MOTO_TYPE_FILENAME, MOTO_PARAM_MODE_INOUT, "", pspec, "General",
            "lock",     "Lock", MOTO_TYPE_BOOL, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "General",
            "watch",    "Watch", MOTO_TYPE_BOOL, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "General",
            NULL);

    priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    priv->bound_calculated = FALSE;
}

static void
moto_mesh_file_node_class_init(MotoMeshFileNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoMeshFileNodePriv));

    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoShapeNodeClass *gnclass = (MotoShapeNodeClass *)klass;

    mesh_file_node_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    gnclass->get_bound = moto_mesh_file_node_get_bound;

    goclass->dispose = moto_mesh_file_node_dispose;
    goclass->finalize = moto_mesh_file_node_finalize;

    nclass->update = moto_mesh_file_node_update;
}

G_DEFINE_TYPE(MotoMeshFileNode, moto_mesh_file_node, MOTO_TYPE_SHAPE_NODE);

/* Methods of class MeshFileNode */

MotoMeshFileNode *moto_mesh_file_node_new()
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)g_object_new(MOTO_TYPE_MESH_FILE_NODE, NULL);
    // MotoNode *node = (MotoNode *)self;

    return self;
}

typedef struct _TryMeshLoaderData
{
    const gchar *filename;
    MotoMesh *mesh;
} TryMeshLoaderData;

static gboolean try_mesh_loader(gpointer data, gpointer user_data)
{
    MotoMeshLoader *loader  = (MotoMeshLoader *)data;
    TryMeshLoaderData *tmld = (TryMeshLoaderData *)user_data;

    if(moto_mesh_loader_can(loader, tmld->filename))
    {
        tmld->mesh = moto_mesh_loader_load(loader, tmld->filename);
        if(tmld->mesh)
            return TRUE;
    }
    return FALSE;
}

static MotoMesh *moto_mesh_file_node_load(MotoMeshFileNode *self, const gchar *filename)
{
    if( ! strlen(filename))
        return NULL;

    MotoLibrary *lib = moto_node_get_library((MotoNode *)self);
    if( ! lib)
    {
        moto_error("I have no library and can't load meshes. :(");
        return NULL;
    }

    TryMeshLoaderData tmld;
    tmld.filename = filename;
    tmld.mesh = NULL;

    moto_library_foreach(lib, "mesh-loader", try_mesh_loader, & tmld);

    if(tmld.mesh)
    {
        moto_info("Mesh \"%s\" successfully loaded.", tmld.filename);
    }
    else
    {
        moto_error("Error while loading mesh \"%s\". I can't load it. ;(", tmld.filename);
    }

    return tmld.mesh;
}

static void moto_mesh_file_node_update_mesh(MotoMeshFileNode *self)
{
    MotoNode *node = (MotoNode*)self;
    MotoMeshFileNodePriv *priv = MOTO_MESH_FILE_NODE_GET_PRIVATE(self);

    const gchar *filename;
    moto_node_get_param_string((MotoNode *)self, "filename", &filename);

    if(priv->mesh)
        g_object_unref(priv->mesh);

    priv->mesh = moto_mesh_file_node_load(self, filename);
    if(priv->mesh)
    {
        moto_info("Mesh '%s' loaded successfully", filename);
        moto_shape_prepare((MotoShape*)priv->mesh);
    }

    priv->bound_calculated = FALSE;
    moto_node_set_param_object(node, "out", (GObject*)priv->mesh);
}

static void moto_mesh_file_node_update(MotoNode *self)
{
    ((MotoNodeClass*)mesh_file_node_parent_class)->update(self);

    MotoMeshFileNode *mesh_file = (MotoMeshFileNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "out");
    if(param)
        moto_mesh_file_node_update_mesh(mesh_file);
}

static MotoBound *moto_mesh_file_node_get_bound(MotoShapeNode *self)
{
    MotoMeshFileNodePriv *priv = MOTO_MESH_FILE_NODE_GET_PRIVATE(self);

    if( ! priv->bound_calculated)
    {
        if(priv->mesh)
            moto_mesh_calc_bound(priv->mesh, priv->bound);
        else
            moto_bound_set(priv->bound, 0, 0, 0, 0, 0, 0);

        priv->bound_calculated = TRUE;
    }

    return priv->bound;
}


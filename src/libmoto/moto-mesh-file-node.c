#include <stdlib.h>
#include <string.h>

#include <gio/gio.h>

#include "moto-point-cloud.h"
#include "moto-filename.h"
#include "moto-mesh-file-node.h"
#include "moto-library.h"
#include "moto-messager.h"
#include "moto-mesh-loader.h"
#include "libmotoutil/matrix.h"

/* forwards */

static MotoBound *moto_mesh_file_node_get_bound(MotoGeometryNode *self);

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
            "filename", "Filename", MOTO_TYPE_FILENAME, MOTO_PARAM_MODE_INOUT, "", pspec, "General", "General",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, priv->mesh, pspec, "Geometry", "Geometry",
            NULL);

    priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    priv->bound_calculated = FALSE;
}

static void
moto_mesh_file_node_class_init(MotoMeshFileNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoGeometryNodeClass *gnclass = (MotoGeometryNodeClass *)klass;

    mesh_file_node_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    gnclass->get_bound = moto_mesh_file_node_get_bound;

    goclass->dispose = moto_mesh_file_node_dispose;
    goclass->finalize = moto_mesh_file_node_finalize;

    nclass->update = moto_mesh_file_node_update;
}

G_DEFINE_TYPE(MotoMeshFileNode, moto_mesh_file_node, MOTO_TYPE_GEOMETRY_NODE);

/* methods of class MeshFileNode */

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
        GString *msg = g_string_new("I have no library and can't load meshes. :(");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
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
    MotoMeshFileNodePriv *priv = MOTO_MESH_FILE_NODE_GET_PRIVATE(self);

    const gchar *filename = moto_node_get_param_string((MotoNode *)self, "filename");

    if(priv->mesh)
        g_object_unref(priv->mesh);

    priv->mesh = moto_mesh_file_node_load(self, filename);

    priv->bound_calculated = FALSE;

    MotoParam *pm = moto_node_get_param((MotoNode *)self, "mesh");
    moto_param_set_object(pm, G_OBJECT(priv->mesh));
    moto_param_update_dests(pm);

}

static void moto_mesh_file_node_update(MotoNode *self)
{
    MotoMeshFileNode *mesh_file = (MotoMeshFileNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && 1)//moto_param_has_dests(param)) /* FIXME */
        moto_mesh_file_node_update_mesh(mesh_file);
}

static void calc_bound(MotoMeshFileNode *self)
{
    MotoMeshFileNodePriv *priv = MOTO_MESH_FILE_NODE_GET_PRIVATE(self);

    priv->bound->bound[0] = 0;
    priv->bound->bound[1] = 0;
    priv->bound->bound[2] = 0;
    priv->bound->bound[3] = 0;
    priv->bound->bound[4] = 0;
    priv->bound->bound[5] = 0;

    if( ! priv->mesh)
        return;

    MotoMesh *mesh = priv->mesh;
    gfloat min_x = 0, max_x = 0, min_y = 0, max_y = 0, min_z = 0, max_z = 0;
    guint i;
    for(i = 0; i < mesh->v_num; i++)
    {
        if(mesh->v_coords[i].x < min_x)
            min_x = mesh->v_coords[i].x;
        if(mesh->v_coords[i].x > max_x)
            max_x = mesh->v_coords[i].x;

        if(mesh->v_coords[i].y < min_y)
            min_y = mesh->v_coords[i].y;
        if(mesh->v_coords[i].y > max_y)
            max_y = mesh->v_coords[i].y;

        if(mesh->v_coords[i].z < min_z)
            min_z = mesh->v_coords[i].z;
        if(mesh->v_coords[i].z > max_z)
            max_z = mesh->v_coords[i].z;
    }

    priv->bound->bound[0] = min_x;
    priv->bound->bound[1] = max_y;
    priv->bound->bound[2] = min_y;
    priv->bound->bound[3] = max_y;
    priv->bound->bound[4] = min_z;
    priv->bound->bound[5] = max_z;
}

static MotoBound *moto_mesh_file_node_get_bound(MotoGeometryNode *self)
{
    MotoMeshFileNode *mf = (MotoMeshFileNode *)self;
    MotoMeshFileNodePriv *priv = MOTO_MESH_FILE_NODE_GET_PRIVATE(self);

    if( ! priv->bound_calculated)
        calc_bound(mf);

    return priv->bound;
}


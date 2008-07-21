#include "moto-mesh-file-node.h"
#include "moto-library.h"
#include "moto-messager.h"
#include "moto-mesh-loader.h"

/* forwards */

static gchar *moto_mesh_file_node_get_filename(MotoMeshFileNode *self);
static void moto_mesh_file_node_set_filename(MotoMeshFileNode *self, const gchar *filename);

/*
static MotoMesh *moto_mesh_file_node_get_mesh(MotoMeshFileNode *self);
*/

static void moto_mesh_file_node_load(MotoMeshFileNode *self, const gchar *filename);

static void moto_mesh_file_node_update(MotoNode *self);

/* class MeshFileNode */

static GObjectClass *mesh_file_node_parent_class = NULL;

struct _MotoMeshFileNodePriv
{
    GString *filename;
    GString **filename_ptr;

    GString *loaded_filename;

    MotoMesh *mesh;
    MotoMesh **mesh_ptr;
};

static void
moto_mesh_file_node_dispose(GObject *obj)
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)obj;

    g_string_free(self->priv->filename, TRUE);
    g_object_unref((GObject *)self->priv->mesh);
    g_slice_free(MotoMeshFileNodePriv, self->priv);

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
    self->priv = g_slice_new(MotoMeshFileNodePriv);

    self->priv->filename = g_string_new("");
    self->priv->filename_ptr = & self->priv->filename;

    self->priv->loaded_filename = g_string_new("");

    self->priv->mesh = NULL;
    self->priv->mesh_ptr = & self->priv->mesh;
}

static void
moto_mesh_file_node_class_init(MotoMeshFileNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    mesh_file_node_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

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

static gchar *moto_mesh_file_node_get_filename(MotoMeshFileNode *self)
{
    return self->priv->filename->str;
}

static void moto_mesh_file_node_set_filename(MotoMeshFileNode *self, const gchar *filename)
{
    g_string_assign(self->priv->filename, filename);
}

/*
static MotoMesh *moto_mesh_file_node_get_mesh(MotoMeshFileNode *self)
{
    return self->priv->mesh;
}
*/

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

static void moto_mesh_file_node_load(MotoMeshFileNode *self, const gchar *filename)
{
    MotoLibrary *lib = moto_node_get_library((MotoNode *)self);
    if( ! lib)
    {
        GString *msg = g_string_new("I have no library and can't load meshes. :(");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    TryMeshLoaderData tmld;
    tmld.filename = filename;
    tmld.mesh = NULL;

    moto_library_foreach(lib, "mesh-loader", try_mesh_loader, & tmld);

    if(tmld.mesh)
    {
        g_string_assign(self->priv->loaded_filename, tmld.filename);

        GString *msg = g_string_new("Mesh \"");
        g_string_append(msg, tmld.filename);
        g_string_append(msg, "\" successfully loaded.");
        moto_info(msg->str);
        g_string_free(msg, TRUE);
    }
    else
    {
        GString *msg = g_string_new("Error while loading mesh \"");
        g_string_append(msg, tmld.filename);
        g_string_append(msg, "\". I can't load it. ;(");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
    }
}

static void moto_mesh_file_node_update_mesh(MotoMeshFileNode *self)
{

}

static void moto_mesh_file_node_update(MotoNode *self)
{
    MotoMeshFileNode *mesh_file = (MotoMeshFileNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && 1)//moto_param_has_dests(param)) /* FIXME */
        moto_mesh_file_node_update_mesh(mesh_file);
}


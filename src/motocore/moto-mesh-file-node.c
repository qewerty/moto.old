#include "moto-mesh-file-node.h"
#include "moto-mesh-param-data.h"
#include "moto-filename-param-data.h"

/* forwards */

static gchar *moto_mesh_file_node_get_filename(MotoMeshFileNode *self);
static void moto_mesh_file_node_set_filename(MotoMeshFileNode *self, const gchar *filename);

static MotoMesh *moto_mesh_file_node_get_mesh(MotoMeshFileNode *self);

/* class MeshFileNode */

static GObjectClass *mesh_file_node_parent_class = NULL;

struct _MotoMeshFileNodePriv
{
    GString *filename;
    GString **filename_ptr;

    MotoMesh *mesh;
    MotoMesh **mesh_ptr;
};

static void
moto_mesh_file_node_dispose(GObject *obj)
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)obj;

    g_string_free(self->priv->filename, TRUE);
    moto_mesh_unref(self->priv->mesh);
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

    mesh_file_node_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    goclass->dispose = moto_mesh_file_node_dispose;
    goclass->finalize = moto_mesh_file_node_finalize;
}

G_DEFINE_TYPE(MotoMeshFileNode, moto_mesh_file_node, MOTO_TYPE_GEOMETRY_NODE);

/* methods of class MeshFileNode */

static void point_filename(MotoParam *param, gpointer p)
{
    MotoMeshFileNode *node = (MotoMeshFileNode *)moto_param_get_node(param);

    self->priv->filename_ptr = p;
}

static void update_filename(MotoParam *param)
{
    MotoMeshFileNode *node = (MotoMeshFileNode *)moto_param_get_node(param);

    GString *filename = *(node->priv->filename_ptr);

    if(filename && (g_utf8_collate(filename->str, node->priv->loaded_filename->str) != 0))
        moto_mesh_file_node_load(node, filename->str);
}

static gpointer get_filename(MotoParam *param)
{
    return moto_mesh_file_node_get_filename((MotoMeshFileNode *)moto_param_get_node(param));
}

static void set_filename(MotoParam *param, gpointer p)
{
    moto_mesh_file_node_set_filename((MotoMeshFileNode *)moto_param_get_node(param),
            (gchar *)p);
}

static gpointer get_mesh(MotoParam *param)
{
    MotoMeshFileNode *node = (MotoMeshFileNode *)moto_param_get_node(param);

    return & node->priv->mesh;
}

MotoMeshFileNode *moto_mesh_file_node_new()
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)g_object_new(MOTO_TYPE_MESH_FILE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    /* params */

    MotoParamBlock *pb;
    MotoParamData *pdata;

    /* main block */
    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("filename", "Filename", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_filename_param_data_new(""));
    moto_param_data_set_cbs(pdata, point_filename, update_filename, get_filename, set_filename);

    moto_param_new("mesh", "Polygonal Mesh", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_mesh_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, get_mesh, NULL);

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

static MotoMesh *moto_mesh_file_node_get_mesh(MotoMeshFileNode *self)
{
    return self->priv->mesh;
}

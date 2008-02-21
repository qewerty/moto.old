#include "moto-mesh-file-node.h"
#include "moto-mesh-param-data.h"
#include "moto-filename-param-data.h"

/* class MeshFileNode */

static GObjectClass *mesh_file_node_parent_class = NULL;

struct _MotoMeshFileNodePriv
{
    MotoMesh *mesh;
};

static void
moto_mesh_file_node_dispose(GObject *obj)
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)obj;

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

    self->priv->mesh = NULL;
}

static void
moto_mesh_file_node_class_init(MotoMeshFileNodeClass *klass)
{
    mesh_file_node_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));
}

G_DEFINE_TYPE(MotoMeshFileNode, moto_mesh_file_node, MOTO_TYPE_GEOMETRY_NODE);

/* methods of class MeshFileNode */

MotoMeshFileNode *moto_mesh_file_node_new()
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)g_object_new(MOTO_TYPE_MESH_FILE_NODE, NULL);

    /* params */

    MotoParamBlock *pb;

    /* main block */
    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("filename", "Filename", MOTO_PARAM_MODE_INOUT, pb,
            moto_filename_param_data_new(get_filename, set_filename, ""));
    moto_param_new("mesh", "Polygonal Mesh", MOTO_PARAM_MODE_OUT, pb,
            moto_mesh_param_data_new(get_mesh, NULL, NULL));

    return self;
}

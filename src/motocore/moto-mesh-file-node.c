#include "moto-mesh-file-node.h"
#include "moto-mesh.h"

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

G_DEFINE_ABSTRACT_TYPE(MotoMeshFileNode, moto_mesh_file_node, G_TYPE_OBJECT);

/* methods of class MeshFileNode */

MotoMeshFileNode *moto_mesh_file_node_new()
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)g_object_new(MOTO_TYPE_MESH_FILE_NODE, NULL);

    /* params */



    return self;
}

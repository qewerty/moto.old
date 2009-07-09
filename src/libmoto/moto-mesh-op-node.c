#include <math.h>
#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-mesh.h"
#include "moto-mesh-op-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static void moto_mesh_op_node_update(MotoNode *self);

/* class MotoMeshOpNode */

typedef struct _MotoMeshOpNodePriv MotoMeshOpNodePriv;
#define MOTO_MESH_OP_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_MESH_OP_NODE, MotoMeshOpNodePriv)

static GObjectClass *mesh_op_node_parent_class = NULL;

struct _MotoMeshOpNodePriv
{
    MotoMeshSelection *selection;
};

static void
moto_mesh_op_node_dispose(GObject *obj)
{
    MotoMeshOpNodePriv *priv = MOTO_MESH_OP_NODE_GET_PRIVATE(obj);

    if(priv->selection)
        moto_mesh_selection_free(priv->selection);

    mesh_op_node_parent_class->dispose(obj);
}

static void
moto_mesh_op_node_finalize(GObject *obj)
{
    mesh_op_node_parent_class->finalize(obj);
}

static void
moto_mesh_op_node_init(MotoMeshOpNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoMeshOpNodePriv *priv = MOTO_MESH_OP_NODE_GET_PRIVATE(self);

    priv->selection = NULL;

    moto_node_add_params(node,
            "active",   "Active",      MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "in_mesh",  "Input Mesh",  MOTO_TYPE_MESH,    MOTO_PARAM_MODE_IN,    NULL, NULL, "Geometry",
            "out_mesh", "Output Mesh", MOTO_TYPE_MESH,    MOTO_PARAM_MODE_OUT,   NULL, NULL, "Geometry",
            NULL);
}

static void
moto_mesh_op_node_class_init(MotoMeshOpNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoMeshOpNodePriv));

    mesh_op_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    goclass->dispose    = moto_mesh_op_node_dispose;
    goclass->finalize   = moto_mesh_op_node_finalize;

    klass->perform = NULL;

    nclass->update = moto_mesh_op_node_update;
}

G_DEFINE_ABSTRACT_TYPE(MotoMeshOpNode, moto_mesh_op_node, MOTO_TYPE_NODE);

/* Methods of class MotoMeshOpNode */

MotoMeshOpNode *moto_mesh_op_node_new(const gchar *name)
{
    MotoMeshOpNode *self = (MotoMeshOpNode *)g_object_new(MOTO_TYPE_MESH_OP_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

void moto_mesh_op_node_set_selection(MotoMeshOpNode *self,
    MotoMeshSelection *selection)
{
    MotoMeshOpNodePriv *priv = MOTO_MESH_OP_NODE_GET_PRIVATE(self);

    if(priv->selection)
        moto_mesh_selection_free(priv->selection);

    priv->selection = moto_mesh_selection_copy(selection);
}

MotoMeshSelection *moto_mesh_op_node_get_selection(MotoMeshOpNode *self)
{
    return MOTO_MESH_OP_NODE_GET_PRIVATE(self)->selection;
}

static void moto_mesh_op_node_update(MotoNode *self)
{
    MotoMeshOpNodePriv *priv = MOTO_MESH_OP_NODE_GET_PRIVATE(self);

    MotoMesh *in_mesh;
    moto_node_get_param_object(self, "in_mesh", (GObject**)&in_mesh);

    MotoMesh *old_mesh;
    moto_node_get_param_object((MotoNode *)self, "out_mesh", (GObject**)&old_mesh);
    if(old_mesh && old_mesh != in_mesh)
        g_object_unref(old_mesh);

    if( ! in_mesh)
    {
        moto_node_set_param_object(self, "out_mesh", NULL);
        MotoParam *param = moto_node_get_param(self, "out_mesh");
        moto_param_update_dests(param);
        return;
    }

    if( ! priv->selection)
    {
        moto_node_set_param_object(self, "out_mesh", (GObject*)in_mesh);
        MotoParam *param = moto_node_get_param(self, "out_mesh");
        moto_param_update_dests(param);
        return;
    }

    MotoMesh *mesh = in_mesh;

    gboolean active;
    moto_node_get_param_boolean(self, "active", &active);
    if(active)
        mesh = moto_mesh_op_perform((MotoMeshOpNode*)self, in_mesh);

    MotoParam *param = moto_node_get_param(self, "out_mesh");
    moto_param_set_object(param, (GObject *)mesh);
    moto_param_update_dests(param);
}

MotoMesh *moto_mesh_op_perform(MotoMeshOpNode *self, MotoMesh *in_mesh)
{
    MotoMeshOpNodeClass *klass = MOTO_MESH_OP_NODE_GET_CLASS(self);

    if(klass->perform)
        return klass->perform(self, in_mesh);

    return NULL;
}

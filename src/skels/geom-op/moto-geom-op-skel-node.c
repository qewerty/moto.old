#include <math.h>
#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-shape-op-skel-node.h"

/* forwards */

static MotoMesh *moto_op_skel_node_perform(MotoOpNode *self, MotoShape *in);

/* class MotoOpSkelNode */

static GObjectClass *op_skel_node_parent_class = NULL;

static void
moto_op_skel_node_init(MotoOpSkelNode *self)
{
    MotoNode *node = (MotoNode*)self;

    // Add your custom parameters here.
    // moto_node_add_params(node, ...);
}

static void
moto_op_skel_node_class_init(MotoOpSkelNodeClass *klass)
{
    op_skel_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoOpNodeClass *op_class = (MotoOpNodeClass *)klass;
    op_class->perform = moto_op_skel_node_perform;
}

G_DEFINE_TYPE(MotoOpSkelNode, moto_op_skel_node, MOTO_TYPE_OP_NODE);

/* Methods of class MotoOpSkelNode */

MotoOpSkelNode *moto_op_skel_node_new(const gchar *name)
{
    MotoOpSkelNode *self = (MotoOpSkelNode *)g_object_new(MOTO_TYPE_OP_SKEL_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static MotoMesh *moto_op_skel_node_perform(MotoOpNode *self, MotoShape *in)
{
    // Implement your operator here.

    return NULL;
}

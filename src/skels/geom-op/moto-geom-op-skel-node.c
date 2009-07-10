#include <math.h>
#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-geom-op-skel-node.h"

/* forwards */

static MotoMesh *moto_geom_op_skel_node_perform(MotoGeomOpNode *self, MotoGeom *in);

/* class MotoGeomOpSkelNode */

static GObjectClass *geom_op_skel_node_parent_class = NULL;

static void
moto_geom_op_skel_node_init(MotoGeomOpSkelNode *self)
{
    MotoNode *node = (MotoNode*)self;

    // Add your custom parameters here.
    // moto_node_add_params(node, ...);
}

static void
moto_geom_op_skel_node_class_init(MotoGeomOpSkelNodeClass *klass)
{
    geom_op_skel_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoGeomOpNodeClass *geom_op_class = (MotoGeomOpNodeClass *)klass;
    geom_op_class->perform = moto_geom_op_skel_node_perform;
}

G_DEFINE_TYPE(MotoGeomOpSkelNode, moto_geom_op_skel_node, MOTO_TYPE_GEOM_OP_NODE);

/* Methods of class MotoGeomOpSkelNode */

MotoGeomOpSkelNode *moto_geom_op_skel_node_new(const gchar *name)
{
    MotoGeomOpSkelNode *self = (MotoGeomOpSkelNode *)g_object_new(MOTO_TYPE_GEOM_OP_SKEL_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static MotoMesh *moto_geom_op_skel_node_perform(MotoGeomOpNode *self, MotoGeom *in)
{
    // Implement your operator here.

    return NULL;
}

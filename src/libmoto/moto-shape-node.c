#include <GL/gl.h>

#include "moto-shape-node.h"
#include "moto-geom.h"

/* class ShapeNode */

typedef struct _MotoShapeNodePriv MotoShapeNodePriv;

#define MOTO_SHAPE_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_SHAPE_NODE, MotoShapeNodePriv)

static GObjectClass *shape_node_parent_class = NULL;

struct _MotoShapeNodePriv
{
    gboolean prepared;
};

/*
static void
moto_shape_node_finalize(GObject *obj)
{
    shape_node_parent_class->finalize(obj);
}
*/

static void
moto_shape_node_init(MotoShapeNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoShapeNodePriv *priv = MOTO_SHAPE_NODE_GET_PRIVATE(self);

    moto_node_add_params(node,
            "active", "Active", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "blocked", "Blocked", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "out", "Output Shape", MOTO_TYPE_GEOM, MOTO_PARAM_MODE_OUT,   NULL, NULL, "Shape",
            NULL);

    priv->prepared = FALSE;
}

static void
moto_shape_node_class_init(MotoShapeNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    shape_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    // goclass->finalize = moto_shape_node_finalize;

    klass->get_bound = NULL;

    g_type_class_add_private(klass, sizeof(MotoShapeNodePriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoShapeNode, moto_shape_node, MOTO_TYPE_NODE);

/* Methods of class ShapeNode */

MotoBound *moto_shape_node_get_bound(MotoShapeNode *self)
{
    MotoShapeNodeClass *klass = MOTO_SHAPE_NODE_GET_CLASS(self);

    if(klass->get_bound)
        return klass->get_bound(self);

    return NULL;
}


#include <GL/gl.h>

#include "moto-geometry-node.h"

/* class GeometryNode */

typedef struct _MotoGeometryNodePriv MotoGeometryNodePriv;

#define MOTO_GEOMETRY_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_GEOMETRY_NODE, MotoGeometryNodePriv)

static GObjectClass *geometry_node_parent_class = NULL;

struct _MotoGeometryNodePriv
{
    gboolean prepared;
};

/*
static void
moto_geometry_node_finalize(GObject *obj)
{
    geometry_node_parent_class->finalize(obj);
}
*/

static void
moto_geometry_node_init(MotoGeometryNode *self)
{
    MotoGeometryNodePriv *priv = MOTO_GEOMETRY_NODE_GET_PRIVATE(self);

    priv->prepared = FALSE;
}

static void
moto_geometry_node_class_init(MotoGeometryNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    geometry_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    // goclass->finalize = moto_geometry_node_finalize;

    klass->get_bound = NULL;

    g_type_class_add_private(klass, sizeof(MotoGeometryNodePriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoGeometryNode, moto_geometry_node, MOTO_TYPE_NODE);

/* methods of class GeometryNode */

MotoBound *moto_geometry_node_get_bound(MotoGeometryNode *self)
{
    MotoGeometryNodeClass *klass = MOTO_GEOMETRY_NODE_GET_CLASS(self);

    if(klass->get_bound)
        return klass->get_bound(self);

    return NULL;
}


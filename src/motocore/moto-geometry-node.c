#include <GL/gl.h>

#include "moto-geometry-node.h"

/* class GeometryNode */

static GObjectClass *geometry_node_parent_class = NULL;

struct _MotoGeometryNodePriv
{
    gboolean prepared;
};

static void
moto_geometry_node_dispose(GObject *obj)
{
    MotoGeometryNode *self = (MotoGeometryNode *)obj;

    g_slice_free(MotoGeometryNodePriv, self->priv);

    G_OBJECT_CLASS(geometry_node_parent_class)->dispose(obj);
}

static void
moto_geometry_node_finalize(GObject *obj)
{
    geometry_node_parent_class->finalize(obj);
}

static void
moto_geometry_node_init(MotoGeometryNode *self)
{
    self->priv = g_slice_new(MotoGeometryNodePriv);

    self->priv->prepared = FALSE;
}

static void
moto_geometry_node_class_init(MotoGeometryNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    geometry_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    goclass->dispose = moto_geometry_node_dispose;
    goclass->finalize = moto_geometry_node_finalize;

    klass->get_bound = NULL;
}

G_DEFINE_ABSTRACT_TYPE(MotoGeometryNode, moto_geometry_node, MOTO_TYPE_NODE);

/* methods of class GeometryNode */

const MotoBound *moto_geometry_node_get_bound(MotoGeometryNode *self)
{
    MotoGeometryNodeClass *klass = MOTO_GEOMETRY_NODE_GET_CLASS(self);

    if(klass->get_bound)
        return klass->get_bound(self);

    return NULL;
}


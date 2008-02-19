#include "moto-object-node.h"
#include "common/matrix.h"

/* class ObjectNode */

static GObjectClass *object_node_parent_class = NULL;

struct _MotoObjectNodePriv
{
    MotoGeometryView *view;

    gfloat transform[16];

    MotoObjectNode *parent;
    GSList *children;

    MotoManipulatorStorage *manip_storage;
};

static void
moto_object_node_dispose(GObject *instance)
{
    MotoObjectNode *self = (MotoObjectNode *)instance;

    g_slice_free(MotoObjectNodePriv, self->priv);
    G_OBJECT_CLASS(object_node_parent_class)->dispose(G_OBJECT(self));
}

static void
moto_object_node_finalize(MotoObjectNode *self)
{
    G_OBJECT_CLASS(object_node_parent_class)->finalize(G_OBJECT(self));
}

static void
moto_object_node_init(MotoObjectNode *self)
{
    self->priv = g_slice_new(MotoObjectNodePriv);

    self->priv->parent = NULL;
    self->priv->children = NULL;
}

static void
moto_object_node_class_init(MotoObjectNodeClass *klass)
{
    object_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    object_node_parent_class->dispose = moto_object_node_dispose;
    object_node_parent_class->finalize = moto_object_node_finalize;
}

G_DEFINE_TYPE(MotoObjectNode, moto_object_node, MOTO_TYPE_NODE);

/* methods class ObjectNode */

MotoObject *moto_object_node_new()
{
    MotoObjectNode *self = \
        (MotoObjectNode *)g_object_new(MOTO_TYPE_OBJECT_NODE, NULL);

    /* params */

    return self;
}

MotoObjectNode *moto_object_node_get_parent(MotoObjectNode *self)
{
    return self->priv->parent;
}

void moto_object_node_set_parent(MotoObjectNode *self, MotoObjectNode *parent, gboolean keep_trasform)
{
    if(parent == self->priv->parent)
        return;

    if(!self->priv->parent)
    {
        self->priv->parent->priv->children = \
            g_slist_remove(self->priv->parent->priv->children, self);
    }

    self->priv->parent = parent;

    if(parent && ! g_slist_find(parent->priv->children, self))
        parent->priv->children = g_slist_append(parent->priv->children, child);
}

static void calc_bound(MotoObjectNode *self)
{

}

const MotoBound *moto_object_node_get_bound(MotoObjectNode *self)
{
    if( ! self->priv->calced_bound)
       calc_bound(self);
    return self->priv->bound;
}

void moto_object_node_draw(MotoObjectNode *self)
{
    if( ! moto_object_node_is_visible(self))
        return;

    GSList *child = g_slist_first(self->priv->children);
    for(; child; child = g_slist_next(child))
    {
        moto_object_node_draw((MotoObjectNode *)child->data);
    }

    if(moto_object_node_is_geomety_visible(self))
        moto_geometry_view_draw(self->priv->view);
}

MotoTrasformOrder moto_object_node_get_transform_order(MotoObjectNode *self)
{
    return self->priv->trasform_order;
}

void moto_object_node_set_transform_order(MotoObjectNode *self, MotoTrasformOrder order)
{
    self->priv->trasform_order = order;
}

MotoRotateOrder moto_object_node_get_rotate_order(MotoObjectNode *self)
{
    return self->priv->rotate_order;
}

void moto_object_node_set_rotate_order(MotoObjectNode *self, MotoRotateOrder order)
{
    self->priv->rotate_order =  order;
}

MotoTransformStrategy moto_object_node_get_trasform_strategy(MotoObjectNode *self)
{
    return self->priv->trasform_strategy;
}

void moto_object_node_get_trasform_strategy(MotoObjectNode *self, MotoTransformStrategy ts)
{
    self->priv->trasform_strategy = ts;
}

MotoManipulator*
moto_object_node_get_manipulator_for_current_node(MotoObjectNode *self)
{
    return
}

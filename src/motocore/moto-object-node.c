#include "moto-object-node.h"
#include "common/matrix.h"

/* class ObjectNode */

static GObjectClass *object_node_parent_class = NULL;

struct _MotoObjectNodePriv
{
    MotoGeometryView *view;

    gfloat translate[3];
    gfloat rotate[3];
    gfloat scale[3];

    gfloat matrix[16];
    gfloat inverse_matrix[16];
    gfloat parent_inverse_matrix[16];
    gfloat translate_matrix[16];
    gfloat rotate_matrix[16];
    gfloat scale_matrix[16];

    gboolean translate_calculated,
             rotate_calculated,
             scale_calculated,
             transform_calculated,
             inverse_calculated;

    gboolean keep_transform;

    gboolean visible;
    gboolean draw_children;

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

/* translate */

void moto_object_node_set_translate(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    self->priv->translate[0] = x;
    self->priv->translate[1] = y;
    self->priv->translate[2] = z;

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    self->priv->translate[0] = xyz[0];
    self->priv->translate[1] = xyz[1];
    self->priv->translate[2] = xyz[2];

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_x(MotoObjectNode *self, gfloat x)
{
    self->priv->translate[0] = x;

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_y(MotoObjectNode *self, gfloat y)
{
    self->priv->translate[1] = y;

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_z(MotoObjectNode *self, gfloat z)
{
    self->priv->translate[2] = z;

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/* rotate */

void moto_object_node_set_rotate(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    self->priv->rotate[0] = x;
    self->priv->rotate[1] = y;
    self->priv->rotate[2] = z;

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    self->priv->rotate[0] = xyz[0];
    self->priv->rotate[1] = xyz[1];
    self->priv->rotate[2] = xyz[2];

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_x(MotoObjectNode *self, gfloat x)
{
    self->priv->rotate[0] = x;

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_y(MotoObjectNode *self, gfloat y)
{
    self->priv->rotate[1] = y;

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_z(MotoObjectNode *self, gfloat z)
{
    self->priv->rotate[2] = z;

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/* scale */

void moto_object_node_set_scale(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    self->priv->scale[0] = x;
    self->priv->scale[1] = y;
    self->priv->scale[2] = z;

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    self->priv->scale[0] = xyz[0];
    self->priv->scale[1] = xyz[1];
    self->priv->scale[2] = xyz[2];

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_x(MotoObjectNode *self, gfloat x)
{
    self->priv->scale[0] = x;

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_y(MotoObjectNode *self, gfloat y)
{
    self->priv->scale[1] = y;

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_z(MotoObjectNode *self, gfloat z)
{
    self->priv->scale[2] = z;

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/*  */

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

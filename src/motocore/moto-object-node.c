#include "gl.h"
#include "glu.h"

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

    MotoTransformOrder  transform_order;
    MotoRotateOrder     rotate_order;

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
    gboolean geometry_visible;

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

static gpointer get_parent(MotoNode *node, MotoParam *param)
{
    return node->priv->parent;
}

static void set_parent(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_parent(node, (MotoObjectNode *)p);
}

static gpointer get_tx(MotoNode *node, MotoParam *param)
{
    return & node->priv->translate[0];
}

static void set_tx(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_x(node, *((gfloat *)p));
}

static gpointer get_ty(MotoNode *node, MotoParam *param)
{
    return & node->priv->translate[1];
}

static void set_ty(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_y(node, *((gfloat *)p));
}

static gpointer get_tz(MotoNode *node, MotoParam *param)
{
    return & node->priv->translate[2];
}

static void set_tz(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_z(node, *((gfloat *)p));
}

static gpointer get_rx(MotoNode *node, MotoParam *param)
{
    return & node->priv->rotate[0];
}

static void set_rx(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_x(node, *((gfloat *)p));
}

static gpointer get_ry(MotoNode *node, MotoParam *param)
{
    return & node->priv->rotate[1];
}

static void set_ry(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_y(node, *((gfloat *)p));
}

static gpointer get_rz(MotoNode *node, MotoParam *param)
{
    return & node->priv->rotate[2];
}

static void set_rz(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_z(node, *((gfloat *)p));
}

static gpointer get_sx(MotoNode *node, MotoParam *param)
{
    return & node->priv->scale[0];
}

static void set_sx(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_x(node, *((gfloat *)p));
}

static gpointer get_sy(MotoNode *node, MotoParam *param)
{
    return & node->priv->scale[1];
}

static void set_sy(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_y(node, *((gfloat *)p));
}

static gpointer get_sz(MotoNode *node, MotoParam *param)
{
    return & node->priv->scale[2];
}

static void set_sz(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_z(node, *((gfloat *)p));
}

static gpointer get_to(MotoNode *node, MotoParam *param)
{
    return & node->priv->transform_order;
}

static void set_to(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_transform_order(node, *((MotoTransformOrder *)p));
}

static gpointer get_ro(MotoNode *node, MotoParam *param)
{
    return & node->priv->rotate_order;
}

static void set_ro(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_order(node, *((MotoRotateOrder *)p));
}

static gpointer get_kt(MotoNode *node, MotoParam *param)
{
    return & node->priv->keep_transform;
}

static void set_kt(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_keep_transform(node, *((gboolean *)p));
}

static gpointer get_visible(MotoNode *node, MotoParam *param)
{
    return & node->priv->visible;
}

static void set_visible(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_visible(node, *((gboolean *)p));
}

static gpointer get_view(MotoNode *node, MotoParam *param)
{
    return node->priv->view;
}

static void set_show_view(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_view(node, (MotoGeometryViewNode *)p);
}

static gpointer get_show_view(MotoNode *node, MotoParam *param)
{
    return & node->priv->visible;
}

static void set_show_view(MotoNode *node, MotoParam *param, gpointer p)
{
    moto_object_node_set_show_view(node, *((gboolean *)p));
}

MotoObjectNode *moto_object_node_new()
{
    MotoObjectNode *self = \
        (MotoObjectNode *)g_object_new(MOTO_TYPE_OBJECT_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    /* params */
    /* WARNING: Implementation of *_param_data_* may be changed in future!  */

    MotoParamBlock *pb;

    /* main block */
    pb = moto_param_block_new("main", "Main", self);
    moto_node_add_param_block(node, pb);
    moto_param_new("parent", "Parent", MOTO_PARAM_MODE_IN, pb,
            moto_object_param_data_new(get_parent, set_parent, NULL));

    moto_param_new("tx", "Translate X", MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_tx, set_tx, 0));
    moto_param_new("ty", "Translate Y", MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_ty, set_ty, 0));
    moto_param_new("tz", "Translate Z", MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_tz, set_tz, 0));
    moto_param_new("rx", "Rotate X",    MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_rx, set_rx, 0));
    moto_param_new("ry", "Rotate Y",    MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_ry, set_ry, 0));
    moto_param_new("rz", "Rotate Z",    MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_rz, set_rz, 0));
    moto_param_new("sx", "Scale X",     MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_sx, set_sx, 1));
    moto_param_new("sy", "Scale Y",     MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_sy, set_sy, 1));
    moto_param_new("sz", "Scale Z",     MOTO_PARAM_MODE_INOUT, pb,
            moto_float_param_data_new(get_sz, set_sz, 1));

    moto_param_new("to", "Transform Order", MOTO_PARAM_MODE_INOUT, pb,
            moto_transform_order_param_data_new(get_to, set_to, MOTO_TRANSFORM_ORDER_TRS));
    moto_param_new("ro", "Rotate Order",    MOTO_PARAM_MODE_INOUT, pb,
            moto_rotate_order_param_data_new(get_ro, set_ro, MOTO_ROTATE_ORDER_XYZ));

    moto_param_new("kt", "Keep Transform", MOTO_PARAM_MODE_INOUT, pb,
            moto_bool_param_data_new(get_kt, set_kt, TRUE));

    /* view block */
    pb = moto_param_block_new("view", "View", self);

    moto_param_new("visible", "Visible", MOTO_PARAM_MODE_INOUT, pb,
            moto_bool_param_data_new(get_visible, set_visible, TRUE));

    moto_param_new("view", "View", MOTO_PARAM_MODE_IN, pb,
            moto_geometry_view_param_data_new(get_view, set_view, NULL));
    moto_param_new("show_view", "Show View", MOTO_PARAM_MODE_INOUT, pb,
            moto_bool_param_data_new(get_show_view, set_show_view, TRUE));

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

void moto_object_node_clear_parent_inverse(MotoObjectNode *self)
{
    matrix44_identity(self->priv->parent_inverse_matrix);
}

void moto_object_node_update_parent_inverse(MotoObjectNode *self)
{
    if(self->priv->parent == NULL)
    {
        moto_object_node_clear_parent_inverse(self);
        return;
    }

    matrix44_copy(self->priv->parent_inverse_matrix,
                  self->priv->parent->priv->inverse_matrix);
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
    if( ! self->priv->visible)
        return;

    GSList *child = g_slist_first(self->priv->children);
    for(; child; child = g_slist_next(child))
    {
        moto_object_node_draw((MotoObjectNode *)child->data);
    }

    if(self->priv->geomety_visible)
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

#include "GL/gl.h"
#include "GL/glu.h"

#include "moto-object-node.h"
#include "moto-float-param-data.h"
#include "moto-bool-param-data.h"
#include "moto-geometry-view-param-data.h"
#include "moto-transform-strategy-param-data.h"
#include "moto-transform-order-param-data.h"
#include "moto-rotate-order-param-data.h"
#include "moto-object-param-data.h"

#include "common/matrix.h"

/* forward */

MotoObjectNode *moto_object_node_get_parent(MotoObjectNode *self);
void moto_object_node_set_parent(MotoObjectNode *self, MotoObjectNode *parent);

gfloat moto_object_node_get_translate_x(MotoObjectNode *self);
void moto_object_node_set_translate_x(MotoObjectNode *self, gfloat x);

gfloat moto_object_node_get_translate_y(MotoObjectNode *self);
void moto_object_node_set_translate_y(MotoObjectNode *self, gfloat y);

gfloat moto_object_node_get_translate_z(MotoObjectNode *self);
void moto_object_node_set_translate_z(MotoObjectNode *self, gfloat z);

gfloat moto_object_node_get_rotate_x(MotoObjectNode *self);
void moto_object_node_set_rotate_x(MotoObjectNode *self, gfloat x);

gfloat moto_object_node_get_rotate_y(MotoObjectNode *self);
void moto_object_node_set_rotate_y(MotoObjectNode *self, gfloat y);

gfloat moto_object_node_get_rotate_z(MotoObjectNode *self);
void moto_object_node_set_rotate_z(MotoObjectNode *self, gfloat z);

gfloat moto_object_node_get_scale_x(MotoObjectNode *self);
void moto_object_node_set_scale_x(MotoObjectNode *self, gfloat x);

gfloat moto_object_node_get_scale_y(MotoObjectNode *self);
void moto_object_node_set_scale_y(MotoObjectNode *self, gfloat y);

gfloat moto_object_node_get_scale_z(MotoObjectNode *self);
void moto_object_node_set_scale_z(MotoObjectNode *self, gfloat z);

MotoGeometryViewNode *moto_object_node_get_view(MotoObjectNode *self);
void moto_object_node_set_view(MotoObjectNode *self, MotoGeometryViewNode *view);

MotoTransformStrategy moto_object_node_get_transform_strategy(MotoObjectNode *self);
void moto_object_node_set_transform_strategy(MotoObjectNode *self, MotoTransformStrategy ts);

MotoTransformOrder moto_object_node_get_transform_order(MotoObjectNode *self);
void moto_object_node_set_transform_order(MotoObjectNode *self, MotoTransformOrder ts);

MotoRotateOrder moto_object_node_get_rotate_order(MotoObjectNode *self);
void moto_object_node_set_rotate_order(MotoObjectNode *self, MotoRotateOrder ts);

gboolean moto_object_node_get_keep_transform(MotoObjectNode *self);
void moto_object_node_set_keep_transform(MotoObjectNode *self, gboolean kt);

gboolean moto_object_node_get_visible(MotoObjectNode *self);
void moto_object_node_set_visible(MotoObjectNode *self, gboolean visible);

gboolean moto_object_node_get_show_view(MotoObjectNode *self);
void moto_object_node_set_show_view(MotoObjectNode *self, gboolean show_view);

/* class ObjectNode */

static GObjectClass *object_node_parent_class = NULL;

struct _MotoObjectNodePriv
{
    MotoGeometryViewNode *view;

    gfloat translate[3];
    gfloat rotate[3];
    gfloat scale[3];

    MotoTransformStrategy   transform_strategy;
    MotoTransformOrder      transform_order;
    MotoRotateOrder         rotate_order;

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
    gboolean show_view;

    MotoObjectNode *parent;
    GSList *children;

    MotoBound *bound;
    gboolean calced_bound;

    /* MotoManipulatorStorage *manip_storage; */
};

static void
moto_object_node_dispose(GObject *obj)
{
    MotoObjectNode *self = (MotoObjectNode *)obj;

    g_slice_free(MotoObjectNodePriv, self->priv);
    G_OBJECT_CLASS(object_node_parent_class)->dispose(G_OBJECT(self));
}

static void
moto_object_node_finalize(GObject *obj)
{
    object_node_parent_class->finalize(obj);
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

/* methods of class ObjectNode */

static gpointer get_parent(MotoParam *param)
{
    return ((MotoObjectNode *)moto_param_get_node(param))->priv->parent;
}

static void set_parent(MotoParam *param, gpointer p)
{
    moto_object_node_set_parent((MotoObjectNode *)moto_param_get_node(param), (MotoObjectNode *)p);
}

static gpointer get_tx(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->translate[0];
}

static void set_tx(MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_x((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_ty(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->translate[1];
}

static void set_ty(MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_y((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_tz(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->translate[2];
}

static void set_tz(MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_z((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_rx(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->rotate[0];
}

static void set_rx(MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_x((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_ry(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->rotate[1];
}

static void set_ry(MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_y((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_rz(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->rotate[2];
}

static void set_rz(MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_z((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_sx(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->scale[0];
}

static void set_sx(MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_x((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_sy(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->scale[1];
}

static void set_sy(MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_y((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_sz(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->scale[2];
}

static void set_sz(MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_z((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static gpointer get_ts(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->transform_strategy;
}

static void set_ts(MotoParam *param, gpointer p)
{
    moto_object_node_set_transform_strategy((MotoObjectNode *)moto_param_get_node(param), *((MotoTransformStrategy *)p));
}

static gpointer get_to(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->transform_order;
}

static void set_to(MotoParam *param, gpointer p)
{
    moto_object_node_set_transform_order((MotoObjectNode *)moto_param_get_node(param), *((MotoTransformOrder *)p));
}

static gpointer get_ro(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->rotate_order;
}

static void set_ro(MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_order((MotoObjectNode *)moto_param_get_node(param), *((MotoRotateOrder *)p));
}

static gpointer get_kt(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->keep_transform;
}

static void set_kt(MotoParam *param, gpointer p)
{
    moto_object_node_set_keep_transform((MotoObjectNode *)moto_param_get_node(param), *((gboolean *)p));
}

static gpointer get_visible(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->visible;
}

static void set_visible(MotoParam *param, gpointer p)
{
    moto_object_node_set_visible((MotoObjectNode *)moto_param_get_node(param), *((gboolean *)p));
}

static gpointer get_view(MotoParam *param)
{
    return ((MotoObjectNode *)moto_param_get_node(param))->priv->view;
}

static void set_view(MotoParam *param, gpointer p)
{
    moto_object_node_set_view((MotoObjectNode *)moto_param_get_node(param), (MotoGeometryViewNode *)p);
}

static gpointer get_show_view(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->show_view;
}

static void set_show_view(MotoParam *param, gpointer p)
{
    moto_object_node_set_show_view((MotoObjectNode *)moto_param_get_node(param), *((gboolean *)p));
}

static gpointer get_transform(MotoParam *param)
{
    return moto_param_get_node(param);
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
    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    if( ! moto_node_add_param_block(node, pb))
    {
        /* Error! =\ */
        g_object_unref((GObject *)pb);
        return self;
    }

    moto_param_new("parent", "Parent", MOTO_PARAM_MODE_IN, pb,
            moto_object_param_data_new(get_parent, set_parent, NULL));
    moto_param_new("transform", "Transform", MOTO_PARAM_MODE_OUT, pb,
            moto_object_param_data_new(get_transform, NULL, NULL));

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

    moto_param_new("ts", "Transform Strategy", MOTO_PARAM_MODE_INOUT, pb,
            moto_transform_strategy_param_data_new(get_ts, set_ts, MOTO_TRANSFORM_STRATEGY_SOFTWARE));
    moto_param_new("to", "Transform Order", MOTO_PARAM_MODE_INOUT, pb,
            moto_transform_order_param_data_new(get_to, set_to, MOTO_TRANSFORM_ORDER_TRS));
    moto_param_new("ro", "Rotate Order",    MOTO_PARAM_MODE_INOUT, pb,
            moto_rotate_order_param_data_new(get_ro, set_ro, MOTO_ROTATE_ORDER_XYZ));

    moto_param_new("kt", "Keep Transform", MOTO_PARAM_MODE_INOUT, pb,
            moto_bool_param_data_new(get_kt, set_kt, TRUE));

    /* view block */
    pb = moto_param_block_new("view", "View", (MotoNode *)self);
    if( ! moto_node_add_param_block(node, pb))
    {
        /* Error! =\ */
        g_object_unref((GObject *)pb);
        return self;
    }

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

void moto_object_node_set_parent(MotoObjectNode *self, MotoObjectNode *parent)
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
        parent->priv->children = g_slist_append(parent->priv->children, self);
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

    GSList *child = self->priv->children;
    for(; child; child = g_slist_next(child))
    {
        moto_object_node_draw((MotoObjectNode *)child->data);
    }

    if(self->priv->show_view)
        moto_geometry_view_draw(self->priv->view);
}

MotoTransformOrder moto_object_node_get_transform_order(MotoObjectNode *self)
{
    return self->priv->transform_order;
}

void moto_object_node_set_transform_order(MotoObjectNode *self, MotoTransformOrder order)
{
    self->priv->transform_order = order;
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
    return self->priv->transform_strategy;
}

void moto_object_node_set_transform_strategy(MotoObjectNode *self, MotoTransformStrategy ts)
{
    self->priv->transform_strategy = ts;
}

MotoManipulator*
moto_object_node_get_manipulator_for_current_node(MotoObjectNode *self)
{
    return NULL;
}

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
#include "common/numdef.h"

/* forward */

static void moto_object_node_calc_transform(MotoObjectNode *self);
static void moto_object_node_calc_inverse_transform(MotoObjectNode *self);

MotoObjectNode *moto_object_node_get_parent(MotoObjectNode *self);
void moto_object_node_set_parent(MotoObjectNode *self, MotoObjectNode *parent);

void moto_object_node_set_translate(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);

gfloat moto_object_node_get_translate_x(MotoObjectNode *self);
void moto_object_node_set_translate_x(MotoObjectNode *self, gfloat x);

gfloat moto_object_node_get_translate_y(MotoObjectNode *self);
void moto_object_node_set_translate_y(MotoObjectNode *self, gfloat y);

gfloat moto_object_node_get_translate_z(MotoObjectNode *self);
void moto_object_node_set_translate_z(MotoObjectNode *self, gfloat z);

void moto_object_node_set_rotate(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);

gfloat moto_object_node_get_rotate_x(MotoObjectNode *self);
void moto_object_node_set_rotate_x(MotoObjectNode *self, gfloat x);

gfloat moto_object_node_get_rotate_y(MotoObjectNode *self);
void moto_object_node_set_rotate_y(MotoObjectNode *self, gfloat y);

gfloat moto_object_node_get_rotate_z(MotoObjectNode *self);
void moto_object_node_set_rotate_z(MotoObjectNode *self, gfloat z);

void moto_object_node_set_scale(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);

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

    gfloat tx, ty, tz;
    gfloat rx, ry, rz;
    gfloat sx, sy, sz;
    gfloat *tx_ptr, *ty_ptr, *tz_ptr;
    gfloat *rx_ptr, *ry_ptr, *rz_ptr;
    gfloat *sx_ptr, *sy_ptr, *sz_ptr;

    MotoTransformStrategy   transform_strategy;
    MotoTransformOrder      transform_order;
    MotoRotateOrder         rotate_order;
    MotoTransformStrategy   *transform_strategy_ptr;
    MotoTransformOrder      *transform_order_ptr;
    MotoRotateOrder         *rotate_order_ptr;

    gboolean translate_calculated,
             rotate_calculated,
             scale_calculated,
             transform_calculated,
             inverse_calculated;

    gboolean keep_transform;
    gboolean *keep_transform_ptr;

    gboolean visible;
    gboolean show_view;
    gboolean *visible_ptr;
    gboolean *show_view_ptr;

    MotoObjectNode *parent;
    GSList *children;

    MotoBound *bound;
    gboolean bound_calculated;

    gfloat matrix[16];
    gfloat inverse_matrix[16];
    gfloat parent_inverse_matrix[16];
    gfloat translate_matrix[16];
    gfloat rotate_matrix[16];
    gfloat scale_matrix[16];
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

    self->priv->transform_strategy  = MOTO_TRANSFORM_STRATEGY_SOFTWARE;
    self->priv->transform_order     = MOTO_TRANSFORM_ORDER_TRS;
    self->priv->rotate_order        = MOTO_ROTATE_ORDER_XYZ;

    self->priv->keep_transform  = TRUE;
    self->priv->visible         = TRUE;
    self->priv->show_view       = TRUE;

    moto_object_node_set_translate(self, 0, 0, 0);
    moto_object_node_set_rotate(self, 0, 0, 0);
    moto_object_node_set_scale(self, 1, 1, 1);

    self->priv->parent = NULL;
    self->priv->children = NULL;

    /* optimizators */

    self->priv->inverse_calculated  = FALSE;
    self->priv->bound_calculated    = FALSE;

    /* pointers */

    self->priv->tx_ptr = & self->priv->tx;
    self->priv->ty_ptr = & self->priv->ty;
    self->priv->tz_ptr = & self->priv->tz;
    self->priv->rx_ptr = & self->priv->rx;
    self->priv->ry_ptr = & self->priv->ry;
    self->priv->rz_ptr = & self->priv->rz;
    self->priv->sx_ptr = & self->priv->sx;
    self->priv->sy_ptr = & self->priv->sy;
    self->priv->sz_ptr = & self->priv->sz;

    self->priv->transform_strategy_ptr = & self->priv->transform_strategy;
    self->priv->transform_order_ptr = & self->priv->transform_order;
    self->priv->rotate_order_ptr = & self->priv->rotate_order;

    self->priv->keep_transform_ptr = & self->priv->keep_transform;

    self->priv->visible_ptr = & self->priv->visible;
    self->priv->show_view_ptr = & self->priv->show_view;
}

static void
moto_object_node_class_init(MotoObjectNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    object_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    goclass->dispose = moto_object_node_dispose;
    goclass->finalize = moto_object_node_finalize;
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

static void update_parent(MotoParam *param)
{
}

static void point_parent(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->parent = (MotoObjectNode *)p;
}

static gpointer get_tx(MotoParam *param)
{
    return ((MotoObjectNode *)moto_param_get_node(param))->priv->tx_ptr;
}

static void set_tx(MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_x((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_tx(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->translate_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_tx(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->tx_ptr = (gfloat *)p;
}

static gpointer get_ty(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->ty;
}

static void set_ty(MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_y((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_ty(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->translate_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_ty(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->ty_ptr = (gfloat *)p;
}

static gpointer get_tz(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->tz;
}

static void set_tz(MotoParam *param, gpointer p)
{
    moto_object_node_set_translate_z((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_tz(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->translate_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_tz(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->tz_ptr = (gfloat *)p;
}

static gpointer get_rx(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->rx;
}

static void set_rx(MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_x((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_rx(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->rotate_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_rx(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->rx_ptr = (gfloat *)p;
}

static gpointer get_ry(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->ry;
}

static void set_ry(MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_y((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_ry(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->rotate_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_ry(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->ry_ptr = (gfloat *)p;
}

static gpointer get_rz(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->rz;
}

static void set_rz(MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_z((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_rz(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->rotate_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_rz(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->rz_ptr = (gfloat *)p;
}

static gpointer get_sx(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->sx;
}

static void set_sx(MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_x((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_sx(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->scale_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_sx(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->sx_ptr = (gfloat *)p;
}

static gpointer get_sy(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->sy;
}

static void set_sy(MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_y((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_sy(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->scale_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_sy(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->sy_ptr = (gfloat *)p;
}

static gpointer get_sz(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->sz;
}

static void set_sz(MotoParam *param, gpointer p)
{
    moto_object_node_set_scale_z((MotoObjectNode *)moto_param_get_node(param), *((gfloat *)p));
}

static void update_sz(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->scale_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_sz(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->sz_ptr = (gfloat *)p;
}

static gpointer get_ts(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->transform_strategy;
}

static void set_ts(MotoParam *param, gpointer p)
{
    moto_object_node_set_transform_strategy((MotoObjectNode *)moto_param_get_node(param), *((MotoTransformStrategy *)p));
}

static void point_ts(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->transform_strategy_ptr = (MotoTransformStrategy *)p;
}

static gpointer get_to(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->transform_order;
}

static void set_to(MotoParam *param, gpointer p)
{
    moto_object_node_set_transform_order((MotoObjectNode *)moto_param_get_node(param), *((MotoTransformOrder *)p));
}

static void update_to(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_to(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->transform_order_ptr = (MotoTransformOrder *)p;
}

static gpointer get_ro(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->rotate_order;
}

static void set_ro(MotoParam *param, gpointer p)
{
    moto_object_node_set_rotate_order((MotoObjectNode *)moto_param_get_node(param), *((MotoRotateOrder *)p));
}

static void update_ro(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->rotate_calculated = FALSE;
    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_ro(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->rotate_order_ptr = (MotoRotateOrder *)p;
}

static gpointer get_kt(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->keep_transform;
}

static void set_kt(MotoParam *param, gpointer p)
{
    moto_object_node_set_keep_transform((MotoObjectNode *)moto_param_get_node(param), *((gboolean *)p));
}

static void update_kt(MotoParam *param)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);

    obj->priv->transform_calculated = FALSE;

    moto_node_update((MotoNode *)obj);
}

static void point_kt(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->keep_transform_ptr = (gboolean *)p;
}

static gpointer get_visible(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->visible;
}

static void set_visible(MotoParam *param, gpointer p)
{
    moto_object_node_set_visible((MotoObjectNode *)moto_param_get_node(param), *((gboolean *)p));
}

static void point_visible(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->visible_ptr = (gboolean *)p;
}

static gpointer get_view(MotoParam *param)
{
    return ((MotoObjectNode *)moto_param_get_node(param))->priv->view;
}

static void set_view(MotoParam *param, gpointer p)
{
    moto_object_node_set_view((MotoObjectNode *)moto_param_get_node(param), (MotoGeometryViewNode *)p);
}

static void point_view(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->view = (MotoGeometryViewNode *)p;
}

static gpointer get_show_view(MotoParam *param)
{
    return & ((MotoObjectNode *)moto_param_get_node(param))->priv->show_view;
}

static void set_show_view(MotoParam *param, gpointer p)
{
    moto_object_node_set_show_view((MotoObjectNode *)moto_param_get_node(param), *((gboolean *)p));
}

static void point_show_view(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->show_view_ptr = (gboolean *)p;
}

static gpointer get_transform(MotoParam *param)
{
    return moto_param_get_node(param);
}

MotoObjectNode *moto_object_node_new(const gchar *name)
{
    MotoObjectNode *self = \
        (MotoObjectNode *)g_object_new(MOTO_TYPE_OBJECT_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    /* params */
    /* WARNING: Implementation of *_param_data_* may be changed in future!  */

    moto_node_set_name(node, name);

    MotoParamBlock *pb;
    MotoParamData *pdata;

    /* main block */
    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("parent", "Parent", MOTO_PARAM_MODE_IN, pb,
            pdata = moto_object_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, point_parent, update_parent, get_parent, set_parent);

    moto_param_new("transform", "Transform", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_object_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, get_transform, NULL);

    moto_param_new("tx", "Translate X", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(0));
    moto_param_data_set_cbs(pdata, point_tx, update_tx, get_tx, set_tx);

    moto_param_new("ty", "Translate Y", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(0));
    moto_param_data_set_cbs(pdata, point_ty, update_ty, get_ty, set_ty);

    moto_param_new("tz", "Translate Z", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(0));
    moto_param_data_set_cbs(pdata, point_tz, update_tz, get_tz, set_tz);

    moto_param_new("rx", "Rotate X",    MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(0));
    moto_param_data_set_cbs(pdata, point_rx, update_rx, get_rx, set_rx);

    moto_param_new("ry", "Rotate Y",    MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(0));
    moto_param_data_set_cbs(pdata, point_ry, update_ry, get_ry, set_ry);

    moto_param_new("rz", "Rotate Z",    MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(0));
    moto_param_data_set_cbs(pdata, point_rz, update_rz, get_rz, set_rz);

    moto_param_new("sx", "Scale X",     MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(1));
    moto_param_data_set_cbs(pdata, point_sx, update_sx, get_sx, set_sx);

    moto_param_new("sy", "Scale Y",     MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(1));
    moto_param_data_set_cbs(pdata, point_sy, update_sy, get_sy, set_sy);

    moto_param_new("sz", "Scale Z",     MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_float_param_data_new(1));
    moto_param_data_set_cbs(pdata, point_sz, update_sz, get_sz, set_sz);

    moto_param_new("ts", "Transform Strategy", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_transform_strategy_param_data_new(MOTO_TRANSFORM_STRATEGY_SOFTWARE));
    moto_param_data_set_cbs(pdata, point_ts, NULL, get_ts, set_ts);

    moto_param_new("to", "Transform Order", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_transform_order_param_data_new(MOTO_TRANSFORM_ORDER_TRS));
    moto_param_data_set_cbs(pdata, point_to, update_to, get_to, set_to);

    moto_param_new("ro", "Rotate Order",    MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_rotate_order_param_data_new(MOTO_ROTATE_ORDER_XYZ));
    moto_param_data_set_cbs(pdata, point_ro, update_ro, get_ro, set_ro);

    moto_param_new("kt", "Keep Transform", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_bool_param_data_new(TRUE));
    moto_param_data_set_cbs(pdata, point_kt, update_kt, get_kt, set_kt);

    /* view block */
    pb = moto_param_block_new("view", "View", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("visible", "Visible", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_bool_param_data_new(TRUE));
    moto_param_data_set_cbs(pdata, point_visible, NULL, get_visible, set_visible);

    moto_param_new("view", "View", MOTO_PARAM_MODE_IN, pb,
            pdata = moto_geometry_view_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, point_view, NULL, get_view, set_view);

    moto_param_new("show_view", "Show View", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_bool_param_data_new(TRUE));
    moto_param_data_set_cbs(pdata, point_show_view, NULL, get_show_view, set_show_view);

    return self;
}

/* translate */

void moto_object_node_set_translate(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    self->priv->tx = x;
    self->priv->ty = y;
    self->priv->tz = z;

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    self->priv->tx = xyz[0];
    self->priv->ty = xyz[1];
    self->priv->tz = xyz[2];

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_x(MotoObjectNode *self, gfloat x)
{
    self->priv->tx = x;

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_y(MotoObjectNode *self, gfloat y)
{
    self->priv->ty = y;

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_z(MotoObjectNode *self, gfloat z)
{
    self->priv->tz = z;

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/* rotate */

void moto_object_node_set_rotate(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    self->priv->rx = x;
    self->priv->ry = y;
    self->priv->rz = z;

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    self->priv->rx = xyz[0];
    self->priv->ry = xyz[1];
    self->priv->rz = xyz[2];

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_x(MotoObjectNode *self, gfloat x)
{
    self->priv->rx = x;

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_y(MotoObjectNode *self, gfloat y)
{
    self->priv->ry = y;

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_z(MotoObjectNode *self, gfloat z)
{
    self->priv->rz = z;

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/* scale */

void moto_object_node_set_scale(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    self->priv->sx = x;
    self->priv->sy = y;
    self->priv->sz = z;

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    self->priv->sx = xyz[0];
    self->priv->sy = xyz[1];
    self->priv->sz = xyz[2];

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_x(MotoObjectNode *self, gfloat x)
{
    self->priv->sx = x;

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_y(MotoObjectNode *self, gfloat y)
{
    self->priv->sy = y;

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_z(MotoObjectNode *self, gfloat z)
{
    self->priv->sz = z;

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/*  */

static void moto_object_node_calc_inverse_transform(MotoObjectNode *self)
{
    moto_object_node_calc_transform(self);

    if(self->priv->inverse_calculated)
        return;

    gfloat ambuf[16], detbuf;
    matrix44_inverse(self->priv->inverse_matrix, self->priv->matrix, ambuf, detbuf);
    if(!detbuf)
    {
        // Exception?
    }

    self->priv->inverse_calculated = TRUE;
}

gfloat *moto_object_node_get_inverse_matrix(MotoObjectNode *self)
{
    moto_object_node_calc_inverse_transform(self);
    return self->priv->inverse_matrix;
}

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
                  moto_object_node_get_inverse_matrix(self->priv->parent));
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

    moto_object_node_update_parent_inverse(self);
}

static void calc_bound(MotoObjectNode *self)
{

}

const MotoBound *moto_object_node_get_bound(MotoObjectNode *self)
{
    if( ! self->priv->bound_calculated)
       calc_bound(self);
    return self->priv->bound;
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

gboolean moto_object_node_get_keep_transform(MotoObjectNode *self)
{
    return self->priv->keep_transform;
}

void moto_object_node_set_keep_transform(MotoObjectNode *self, gboolean kt)
{
    if(kt != self->priv->transform_strategy)
    {
        self->priv->transform_calculated = FALSE;
        self->priv->keep_transform = kt;
    }
}

static void moto_object_node_calc_translate(MotoObjectNode *self)
{
    if(self->priv->translate_calculated)
        return;

    matrix44_translate(self->priv->translate_matrix,
            *(self->priv->tx_ptr), *(self->priv->ty_ptr), *(self->priv->tz_ptr));

    self->priv->translate_calculated = TRUE;
}

static void moto_object_node_calc_rotate(MotoObjectNode *self)
{
    if(self->priv->rotate_calculated)
        return;

    gfloat tmp[16], tmp2[16];

    switch(*(self->priv->rotate_order_ptr))
    {
     case MOTO_ROTATE_ORDER_XYZ:
        matrix44_rotate_x(tmp, *(self->priv->rx_ptr));
        matrix44_rotate_y(self->priv->rotate_matrix, *(self->priv->ry_ptr));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_z(tmp, *(self->priv->rz_ptr));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_XZY:
        matrix44_rotate_x(tmp, *(self->priv->rx_ptr));
        matrix44_rotate_z(self->priv->rotate_matrix, *(self->priv->rz_ptr));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_y(tmp, *(self->priv->ry_ptr));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_YXZ:
        matrix44_rotate_y(tmp, *(self->priv->ry_ptr));
        matrix44_rotate_x(self->priv->rotate_matrix, *(self->priv->rx_ptr));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_z(tmp, *(self->priv->rz_ptr));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_YZX:
        matrix44_rotate_y(tmp, *(self->priv->ry_ptr));
        matrix44_rotate_z(self->priv->rotate_matrix, *(self->priv->rz_ptr));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_x(tmp, *(self->priv->rx_ptr));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_ZXY:
        matrix44_rotate_z(tmp, *(self->priv->rz_ptr));
        matrix44_rotate_x(self->priv->rotate_matrix, *(self->priv->rx_ptr));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_y(tmp, *(self->priv->ry_ptr));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_ZYX:
        matrix44_rotate_z(tmp, *(self->priv->rz_ptr));
        matrix44_rotate_y(self->priv->rotate_matrix, *(self->priv->ry_ptr));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_x(tmp, *(self->priv->rx_ptr));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
    }

    self->priv->rotate_calculated = TRUE;
}

static void moto_object_node_calc_scale(MotoObjectNode *self)
{
    if(self->priv->scale_calculated)
        return;

    gfloat sx = *(self->priv->sx_ptr);
    gfloat sy = *(self->priv->sy_ptr);
    gfloat sz = *(self->priv->sz_ptr);

    matrix44_scale(self->priv->scale_matrix, sx, sy, sz);

    self->priv->scale_calculated = TRUE;
}

static void moto_object_node_calc_transform(MotoObjectNode *self)
{
    if(self->priv->transform_calculated)
        return;

    gfloat tmp[16];

    moto_object_node_calc_translate(self);
    moto_object_node_calc_scale(self);
    moto_object_node_calc_rotate(self);

    matrix44_identity(self->priv->matrix);
    switch(*(self->priv->transform_order_ptr))
    {
        case MOTO_TRANSFORM_ORDER_TRS:
            matrix44_mult(tmp, self->priv->translate_matrix, self->priv->rotate_matrix);
            matrix44_mult(self->priv->matrix, tmp, self->priv->scale_matrix);
        break;
        case MOTO_TRANSFORM_ORDER_TSR:
            matrix44_mult(tmp, self->priv->translate_matrix, self->priv->scale_matrix);
            matrix44_mult(self->priv->matrix, tmp, self->priv->rotate_matrix);
        break;
        case MOTO_TRANSFORM_ORDER_RTS:
            matrix44_mult(tmp, self->priv->rotate_matrix, self->priv->translate_matrix);
            matrix44_mult(self->priv->matrix, tmp, self->priv->scale_matrix);
        break;
        case MOTO_TRANSFORM_ORDER_RST:
            matrix44_mult(tmp, self->priv->rotate_matrix, self->priv->scale_matrix);
            matrix44_mult(self->priv->matrix, tmp, self->priv->translate_matrix);
        break;
        case MOTO_TRANSFORM_ORDER_STR:
            matrix44_mult(tmp, self->priv->scale_matrix, self->priv->translate_matrix);
            matrix44_mult(self->priv->matrix, tmp, self->priv->rotate_matrix);
        break;
        case MOTO_TRANSFORM_ORDER_SRT:
            matrix44_mult(tmp, self->priv->scale_matrix, self->priv->rotate_matrix);
            matrix44_mult(self->priv->matrix, tmp, self->priv->translate_matrix);
        break;
    }

    self->priv->transform_calculated = TRUE;
    self->priv->inverse_calculated = FALSE;
}

static gfloat *moto_object_node_get_matrix(MotoObjectNode *self)
{
    moto_object_node_calc_transform(self);
    return self->priv->matrix;
}

#define rotate(rotate_order) switch(rotate_order)\
{\
 case MOTO_ROTATE_ORDER_XYZ:\
    glRotatef(*(self->priv->rx_ptr)*DEG_PER_RAD, 1, 0, 0);\
    glRotatef(*(self->priv->ry_ptr)*DEG_PER_RAD, 0, 1, 0);\
    glRotatef(*(self->priv->rz_ptr)*DEG_PER_RAD, 0, 0, 1);\
 break;\
 case MOTO_ROTATE_ORDER_XZY:\
    glRotatef(*(self->priv->rx_ptr)*DEG_PER_RAD, 1, 0, 0);\
    glRotatef(*(self->priv->rz_ptr)*DEG_PER_RAD, 0, 0, 1);\
    glRotatef(*(self->priv->ry_ptr)*DEG_PER_RAD, 0, 1, 0);\
 break;\
 case MOTO_ROTATE_ORDER_YXZ:\
    glRotatef(*(self->priv->ry_ptr)*DEG_PER_RAD, 0, 1, 0);\
    glRotatef(*(self->priv->rx_ptr)*DEG_PER_RAD, 1, 0, 0);\
    glRotatef(*(self->priv->rz_ptr)*DEG_PER_RAD, 0, 0, 1);\
 break;\
 case MOTO_ROTATE_ORDER_YZX:\
    glRotatef(*(self->priv->ry_ptr)*DEG_PER_RAD, 0, 1, 0);\
    glRotatef(*(self->priv->rz_ptr)*DEG_PER_RAD, 0, 0, 1);\
    glRotatef(*(self->priv->rx_ptr)*DEG_PER_RAD, 1, 0, 0);\
 break;\
 case MOTO_ROTATE_ORDER_ZXY:\
    glRotatef(*(self->priv->rz_ptr)*DEG_PER_RAD, 0, 0, 1);\
    glRotatef(*(self->priv->rx_ptr)*DEG_PER_RAD, 1, 0, 0);\
    glRotatef(*(self->priv->ry_ptr)*DEG_PER_RAD, 0, 1, 0);\
 break;\
 case MOTO_ROTATE_ORDER_ZYX:\
    glRotatef(*(self->priv->rz_ptr)*DEG_PER_RAD, 0, 0, 1);\
    glRotatef(*(self->priv->ry_ptr)*DEG_PER_RAD, 0, 1, 0);\
    glRotatef(*(self->priv->rx_ptr)*DEG_PER_RAD, 1, 0, 0);\
    break;\
}

static void apply_hardware_transform(MotoObjectNode *self)
{
    if(self->priv->keep_transform)
    {
        glMultMatrixf(self->priv->parent_inverse_matrix);
    }

    switch(*(self->priv->transform_order_ptr))
    {
     case MOTO_TRANSFORM_ORDER_SRT:
        glScalef(*(self->priv->sx_ptr), *(self->priv->sy_ptr), *(self->priv->sz_ptr));
        rotate(*(self->priv->rotate_order_ptr));
        glTranslatef(*(self->priv->tx_ptr), *(self->priv->ty_ptr), *(self->priv->tz_ptr));
     break;
     case MOTO_TRANSFORM_ORDER_STR:
        glScalef(*(self->priv->sx_ptr), *(self->priv->sy_ptr), *(self->priv->sz_ptr));
        glTranslatef(*(self->priv->tx_ptr), *(self->priv->ty_ptr), *(self->priv->tz_ptr));
        rotate(*(self->priv->rotate_order_ptr));
     break;
     case MOTO_TRANSFORM_ORDER_RST:
        rotate(*(self->priv->rotate_order_ptr));
        glScalef(*(self->priv->sx_ptr), *(self->priv->sy_ptr), *(self->priv->sz_ptr));
        glTranslatef(*(self->priv->tx_ptr), *(self->priv->ty_ptr), *(self->priv->tz_ptr));
     break;
     case MOTO_TRANSFORM_ORDER_RTS:
        rotate(*(self->priv->rotate_order_ptr));
        glTranslatef(*(self->priv->tx_ptr), *(self->priv->ty_ptr), *(self->priv->tz_ptr));
        glScalef(*(self->priv->sx_ptr), *(self->priv->sy_ptr), *(self->priv->sz_ptr));
     break;
     case MOTO_TRANSFORM_ORDER_TSR:
        glTranslatef(*(self->priv->tx_ptr), *(self->priv->ty_ptr), *(self->priv->tz_ptr));
        glScalef(*(self->priv->sx_ptr), *(self->priv->sy_ptr), *(self->priv->sz_ptr));
        rotate(*(self->priv->rotate_order_ptr));
     break;
     case MOTO_TRANSFORM_ORDER_TRS:
        glTranslatef(*(self->priv->tx_ptr), *(self->priv->ty_ptr), *(self->priv->tz_ptr));
        rotate(*(self->priv->rotate_order_ptr));
        glScalef(*(self->priv->sx_ptr), *(self->priv->sy_ptr), *(self->priv->sz_ptr));
     break;
    }
}

static void apply_transform(MotoObjectNode *self)
{
    switch(*(self->priv->transform_strategy_ptr))
    {
        case MOTO_TRANSFORM_STRATEGY_HARDWARE:
            apply_hardware_transform(self);
        break;
        case MOTO_TRANSFORM_STRATEGY_SOFTWARE:
            glMultMatrixf(moto_object_node_get_matrix(self));
        break;
    }
}

void moto_object_node_draw(MotoObjectNode *self)
{
    if( ! *(self->priv->visible_ptr))
        return;

    glPushMatrix();

    apply_transform(self);

    GSList *child = self->priv->children;
    for(; child; child = g_slist_next(child))
    {
        moto_object_node_draw((MotoObjectNode *)child->data);
    }

    if(*(self->priv->show_view_ptr))
        moto_geometry_view_node_draw(self->priv->view);

    glPopMatrix();
}

MotoGeometryViewNode *moto_object_node_get_view(MotoObjectNode *self)
{
    return self->priv->view;
}

void moto_object_node_set_view(MotoObjectNode *self, MotoGeometryViewNode *view)
{
    self->priv->view = view;
}

gboolean moto_object_node_get_show_view(MotoObjectNode *self)
{
    return self->priv->show_view;
}

void moto_object_node_set_show_view(MotoObjectNode *self, gboolean show_view)
{
    self->priv->show_view = show_view;
}

gboolean moto_object_node_get_visible(MotoObjectNode *self)
{
    return self->priv->visible;
}

void moto_object_node_set_visible(MotoObjectNode *self, gboolean visible)
{
    self->priv->visible = visible;
}

/* class ObjectNodeFactory */

MotoNode *
moto_object_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name);

static GType moto_object_node_factory_get_node_type(MotoNodeFactory *self);

static GObjectClass *object_node_factory_parent_class = NULL;

static void
moto_object_node_factory_dispose(GObject *obj)
{
    G_OBJECT_CLASS(object_node_factory_parent_class)->dispose(obj);
}

static void
moto_object_node_factory_finalize(GObject *obj)
{
    object_node_factory_parent_class->finalize(obj);
}

static void
moto_object_node_factory_init(MotoObjectNodeFactory *self)
{

}

static void
moto_object_node_factory_class_init(MotoObjectNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeFactoryClass *nfclass = (MotoNodeFactoryClass *)klass;

    object_node_factory_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_object_node_factory_dispose;
    goclass->finalize   = moto_object_node_factory_finalize;

    nfclass->get_node_type  = moto_object_node_factory_get_node_type;
    nfclass->create_node    = moto_object_node_factory_create_node;
}

G_DEFINE_TYPE(MotoObjectNodeFactory, moto_object_node_factory, MOTO_TYPE_NODE_FACTORY);

/* methods of class ObjectNodeFactory */

static MotoNodeFactory *object_node_factory = NULL;

MotoNodeFactory *moto_object_node_factory_new()
{
    if( ! object_node_factory)
        object_node_factory = \
            (MotoNodeFactory *)g_object_new(MOTO_TYPE_OBJECT_NODE_FACTORY, NULL);

    return object_node_factory;
}

MotoNode *moto_object_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name)
{
    return (MotoNode *)moto_object_node_new(name);
}

static GType moto_object_node_factory_get_node_type(MotoNodeFactory *self)
{
    return MOTO_TYPE_OBJECT_NODE;
}

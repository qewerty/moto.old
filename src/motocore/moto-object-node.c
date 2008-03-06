#include "math.h"

#include "GL/gl.h"
#include "GL/glu.h"

#include "moto-world.h"
#include "moto-messager.h"
#include "moto-object-node.h"
#include "moto-material-node.h"
#include "moto-float-param-data.h"
#include "moto-bool-param-data.h"
#include "moto-geometry-view-param-data.h"
#include "moto-transform-strategy-param-data.h"
#include "moto-transform-order-param-data.h"
#include "moto-rotate-order-param-data.h"
#include "moto-object-param-data.h"
#include "moto-material-param-data.h"
#include "moto-camera-param-data.h"

#include "common/matrix.h"
#include "common/numdef.h"
#include "common/print-array.h"

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

void moto_object_node_set_material(MotoObjectNode *self, MotoMaterialNode *material);
void moto_object_node_set_camera(MotoObjectNode *self, MotoCameraNode *camera);

// static void moto_object_node_convert_camera_transform(MotoObjectNode *self);

/* class ObjectNode */

static GObjectClass *object_node_parent_class = NULL;

struct _MotoObjectNodePriv
{
    MotoGeometryViewNode *view;
    MotoMaterialNode *material;
    MotoCameraNode *camera;

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
    gfloat global_matrix[16];
    gfloat inverse_matrix[16];
    gfloat global_inverse_matrix[16];
    gfloat parent_inverse_matrix[16];
    gfloat translate_matrix[16];
    gfloat rotate_matrix[16];
    gfloat scale_matrix[16];

    /* camera */
    gfloat up[3];
    gfloat target[3]; // in world coords
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
    self->priv->rotate_order        = MOTO_ROTATE_ORDER_YZX;

    self->priv->keep_transform  = TRUE;
    self->priv->visible         = TRUE;
    self->priv->show_view       = TRUE;

    moto_object_node_set_translate(self, 0, 0, 0);
    moto_object_node_set_rotate(self, 0, 0, 0);
    moto_object_node_set_scale(self, 1, 1, 1);

    self->priv->parent = NULL;
    self->priv->children = NULL;

    /* optimizations */

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

    self->priv->view = NULL;
    self->priv->material = NULL;

    /* camera */
    self->priv->target[0] = 0;
    self->priv->target[1] = 0;
    self->priv->target[2] = 0;
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
    //MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    //obj->priv->parent = (MotoObjectNode *)p;
    moto_object_node_set_parent((MotoObjectNode *)moto_param_get_node(param), (MotoObjectNode *)p);
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

/*
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
*/

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

static gpointer get_material(MotoParam *param)
{
    return ((MotoObjectNode *)moto_param_get_node(param))->priv->material;
}

static void set_material(MotoParam *param, gpointer p)
{
    moto_object_node_set_material((MotoObjectNode *)moto_param_get_node(param), (MotoMaterialNode *)p);
}

static void point_material(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->material = (MotoMaterialNode *)p;
}

static gpointer get_camera(MotoParam *param)
{
    return ((MotoObjectNode *)moto_param_get_node(param))->priv->camera;
}

static void set_camera(MotoParam *param, gpointer p)
{
    moto_object_node_set_camera((MotoObjectNode *)moto_param_get_node(param), (MotoCameraNode *)p);
}

static void point_camera(MotoParam *param, gpointer p)
{
    MotoObjectNode *obj = (MotoObjectNode *)moto_param_get_node(param);
    obj->priv->camera = (MotoCameraNode *)p;
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

    /* TODO: Temporary disabled!
    moto_param_new("to", "Transform Order", MOTO_PARAM_MODE_INOUT, pb,
            pdata = moto_transform_order_param_data_new(MOTO_TRANSFORM_ORDER_TRS));
    moto_param_data_set_cbs(pdata, point_to, update_to, get_to, set_to);
    */

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

    /* shading block */
    pb = moto_param_block_new("shading", "Shading", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("material", "Material", MOTO_PARAM_MODE_IN, pb,
            pdata = moto_material_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, point_material, NULL, get_material, set_material);

    /* shading block */
    pb = moto_param_block_new("camera", "Camera", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("camera", "Camera", MOTO_PARAM_MODE_IN, pb,
            pdata = moto_camera_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, point_camera, NULL, get_camera, set_camera);

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

static gfloat *moto_object_node_get_matrix(MotoObjectNode *self, gboolean global)
{
    moto_object_node_calc_transform(self);

    if(global)
    {
        if( ! self->priv->parent)
            return self->priv->matrix;

        gfloat *parent = \
            moto_object_node_get_matrix(self->priv->parent, TRUE);

        matrix44_mult(self->priv->global_matrix,
                parent, self->priv->matrix);

        return self->priv->global_matrix;
    }

    return self->priv->matrix;
}

gfloat *moto_object_node_get_inverse_matrix(MotoObjectNode *self, gboolean global)
{
    moto_object_node_calc_inverse_transform(self);

    if(global)
    {
        if( ! self->priv->parent)
            return self->priv->inverse_matrix;

        gfloat *parent_inverse = \
            moto_object_node_get_inverse_matrix(self->priv->parent, TRUE);

        matrix44_mult(self->priv->global_inverse_matrix,
                parent_inverse, self->priv->inverse_matrix);

        return self->priv->global_inverse_matrix;
    }

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
                  moto_object_node_get_inverse_matrix(self->priv->parent, FALSE));
}

MotoObjectNode *moto_object_node_get_parent(MotoObjectNode *self)
{
    return self->priv->parent;
}

void moto_object_node_set_parent(MotoObjectNode *self, MotoObjectNode *parent)
{
    if(parent == self->priv->parent)
        return;

    if(self->priv->parent)
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
    gfloat to_target[3], eye[3];
    gfloat loc_pos[] = {0, 0, 0};
    gfloat *matrix = moto_object_node_get_matrix(self, TRUE);
    point3_transform(eye, matrix, loc_pos);
    vector3_dif(to_target, self->priv->target, eye);
    gfloat dist = vector3_length(to_target);

    gfloat az[] = {0, 0, 1};
    gfloat n[3];
    vector3_transform(n, matrix, az);
    vector3_mult(to_target, n, -dist);

    vector3_copy(self->priv->target, eye);
    point3_move(self->priv->target, to_target, 1);

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
            glMultMatrixf(moto_object_node_get_matrix(self, FALSE));
        break;
    }
}

void moto_object_node_draw(MotoObjectNode *self)
{
    if( ! *(self->priv->visible_ptr))
        return;

    glPushMatrix();

    apply_transform(self);

    if(self->priv->material)
        moto_material_node_use(self->priv->material);

    GSList *child = self->priv->children;
    for(; child; child = g_slist_next(child))
    {
        moto_object_node_draw((MotoObjectNode *)child->data);
    }

    if(*(self->priv->show_view_ptr) && self->priv->view)
        moto_geometry_view_node_draw(self->priv->view);

    glPopMatrix();
}

void moto_object_node_look_at(MotoObjectNode *self, gfloat eye[3], gfloat look[3], gfloat up[3])
{}

void moto_object_node_slide(MotoObjectNode *self,
        gfloat dx, gfloat dy, gfloat dz)
{
    gfloat ax[] = {1, 0, 0};
    gfloat ay[] = {0, 1, 0};
    gfloat az[] = {0, 0, 1};
    gfloat tax[3], tay[3], taz[3];
    gfloat *matrix = moto_object_node_get_matrix(self, FALSE);

    vector3_transform(tax, matrix, ax);
    vector3_transform(tay, matrix, ay);
    vector3_transform(taz, matrix, az);

    gfloat pos[3] = \
        {self->priv->tx, self->priv->ty, self->priv->tz};
    point3_move(pos, tax, dx);
    point3_move(pos, tay, dy);
    point3_move(pos, taz, dz);
    point3_move(self->priv->target, tax, dx);
    point3_move(self->priv->target, tay, dy);
    point3_move(self->priv->target, taz, dz);

    moto_object_node_set_translate_array(self, pos);
}

void moto_object_node_zoom(MotoObjectNode *self, gfloat val)
{
    gfloat to_target[3], eye[3];
    gfloat loc_pos[] = {0, 0, 0};
    gfloat *matrix = moto_object_node_get_matrix(self, TRUE);

    point3_transform(eye, matrix, loc_pos);
    vector3_dif(to_target, self->priv->target, eye);

    // g_print("1\n");
    if(vector3_length(to_target) < MICRO)
    {
        if(val > 0)
            return;

        gfloat az[] = {0, 0, 1};
        gfloat n[3];
        vector3_transform(n, matrix, az);

        point3_move(eye, n, 0.5);
    }
    else
        point3_move(eye, to_target, val);

    if(self->priv->parent)
    {
        gfloat *inverse = moto_object_node_get_inverse_matrix(self->priv->parent, TRUE);
        point3_transform(loc_pos, inverse, eye);
    }
    else
    {
        vector3_copy(loc_pos, eye);
    }

    moto_object_node_set_translate_array(self, loc_pos);
}

void moto_object_node_tumble(MotoObjectNode *self, gfloat dha, gfloat dva)
{
    gfloat ax[] = {1, 0, 0};
    gfloat ay[] = {0, 1, 0};
    gfloat az[] = {0, 0, 1};
    gfloat u[3], v[3], n[3], t[3];
    gfloat to_u[3], to_v[3], to_n[3],
           pu[3], pv[3], pn[3];

    gfloat to_eye[3], eye[3];
    gfloat loc_pos[] = {0, 0, 0};
    gfloat tumble_h_axis[3];
    gfloat tumble_v_axis[] = {0, 1, 0};
    gfloat target[3];
    vector3_copy(target, self->priv->target);

    // vector3_set(target, 0, 0, 0); // TEMP

    gfloat *matrix = moto_object_node_get_matrix(self, TRUE);

    vector3_transform(u, matrix, ax);
    vector3_transform(v, matrix, ay);
    vector3_transform(n, matrix, az);

    g_print("_u.l: %f\n", vector3_length(u));
    g_print("_v.l: %f\n", vector3_length(v));
    g_print("_n.l: %f\n", vector3_length(n));

    vector3_copy(tumble_h_axis, u);
    // vector3_copy(tumble_v_axis, v);

    point3_transform(eye, matrix, loc_pos);
    vector3_sum(pu, eye, u);
    vector3_sum(pv, eye, v);
    vector3_sum(pn, eye, n);

    vector3_dif(to_eye, eye, target);
    vector3_dif(to_u, pu, target);
    vector3_dif(to_v, pv, target);
    vector3_dif(to_n, pn, target);

    /*
    gfloat hc = cos(RAD_PER_DEG*dha);
    gfloat hs = sin(RAD_PER_DEG*dha);
    gfloat vc = cos(RAD_PER_DEG*dva);
    gfloat vs = sin(RAD_PER_DEG*dva);
    */

    gfloat hrm[16], vrm[16], rm[16];
    matrix44_rotate_from_axis(hrm, dha, tumble_h_axis[0], tumble_h_axis[1], tumble_h_axis[2]);
    matrix44_rotate_from_axis(vrm, dva, tumble_v_axis[0], tumble_v_axis[1], tumble_v_axis[2]);
    matrix44_mult(rm, hrm, vrm);

    /* eye rotation */
    gfloat to_eye2[3];
    vector3_transform(to_eye2, rm, to_eye);

    /* u rotation */
    gfloat to_u2[3];
    vector3_transform(to_u2, rm, to_u);

    /* v rotation */
    gfloat to_v2[3];
    vector3_transform(to_v2, rm, to_v);

    /* n rotation */
    gfloat to_n2[3];
    vector3_transform(to_n2, rm, to_n);

    /* new eye, u, v, n */
    vector3_sum(eye, to_eye2, target);
    vector3_sum(pu, to_u2, target);
    vector3_dif(u, pu, eye);
    vector3_sum(pv, to_v2, target);
    vector3_dif(v, pv, eye);
    vector3_sum(pn, to_n2, target);
    vector3_dif(n, pn, eye);

    /*
    vector3_normalize(u, t[0]);
    vector3_normalize(v, t[0]);
    vector3_normalize(n, t[0]);
    */

    g_print("u.l: %f\n", vector3_length(u));
    g_print("v.l: %f\n", vector3_length(v));
    g_print("n.l: %f\n", vector3_length(n));

    /* inverse global matrix */
    gfloat igm[16];
    matrix44_camera_inverse(igm, eye, u, v, n);

    /* global matrix */
    gfloat gm[16], ambuf[16], detbuf;
    matrix44_inverse(gm, igm, ambuf, detbuf);
    if(fabs(detbuf) < MICRO)
    {
        moto_error("(moto_object_node_tumble) determinant is zero");
        return;
    }

    /* local matrix */
    gfloat lm[16];
    gfloat *lmp = gm;
    if(self->priv->parent)
    {
        gfloat *parent_inverse = moto_object_node_get_inverse_matrix(self->priv->parent, TRUE);
        matrix44_mult(lm, parent_inverse, gm);

        lmp = lm;
    }

    gfloat translate[3];
    translate_from_matrix44(translate, lmp);
    moto_object_node_set_translate_array(self, translate);

    gfloat euler[3], cosbuf;
    switch(self->priv->rotate_order)
    {
        case MOTO_ROTATE_ORDER_XYZ:
            euler_xyz_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_XZY:
            euler_xzy_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_YXZ:
            euler_yxz_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_YZX:
            euler_yzx_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_ZXY:
            euler_zxy_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_ZYX:
            euler_zyx_from_matrix44(euler, lmp, cosbuf);
        break;
    }
    moto_object_node_set_rotate_array(self, euler);
}

void moto_object_node_roll(MotoObjectNode *self, gfloat da)
{
    gfloat ax[] = {1, 0, 0};
    gfloat ay[] = {0, 1, 0};
    gfloat az[] = {0, 0, 1};
    gfloat u[3], v[3], n[3], t[3];
    gfloat eye[3];
    gfloat loc_pos[] = {0, 0, 0};
    gfloat *matrix = moto_object_node_get_matrix(self, TRUE);
    point3_transform(eye, matrix, loc_pos);

    vector3_transform(u, matrix, ax);
    vector3_transform(v, matrix, ay);
    vector3_transform(n, matrix, az);

    gfloat c = cos(RAD_PER_DEG*da);
    gfloat s = sin(RAD_PER_DEG*da);

    vector3_copy(t, u);
    vector3_set(u, c*t[0] - s*v[0], c*t[1] - s*v[1], c*t[2] - s*v[2]);
    vector3_set(v, s*t[0] + c*v[0], s*t[1] + c*v[1], s*t[2] + c*v[2]);

    /* inverse global matrix */
    gfloat igm[16];
    matrix44_camera_inverse(igm, eye, u, v, n);

    /* global matrix */
    gfloat gm[16], ambuf[16], detbuf;
    matrix44_inverse(gm, igm, ambuf, detbuf);
    if(fabs(detbuf) < MICRO)
    {
        moto_error("(moto_object_node_roll) determinant is zero");
        return;
    }

    /* local matrix */
    gfloat lm[16];
    gfloat *lmp = gm;
    if(self->priv->parent)
    {
        gfloat *parent_inverse = moto_object_node_get_inverse_matrix(self->priv->parent, TRUE);
        matrix44_mult(lm, parent_inverse, gm);

        lmp = lm;
    }

    gfloat euler[3], cosbuf;
    switch(self->priv->rotate_order)
    {
        case MOTO_ROTATE_ORDER_XYZ:
            euler_xyz_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_XZY:
            euler_xzy_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_YXZ:
            euler_yxz_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_YZX:
            euler_yzx_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_ZXY:
            euler_zxy_from_matrix44(euler, lmp, cosbuf);
        break;
        case MOTO_ROTATE_ORDER_ZYX:
            euler_zyx_from_matrix44(euler, lmp, cosbuf);
        break;
    }

    moto_object_node_set_rotate_array(self, euler);
}

void moto_object_node_pitch(MotoObjectNode *self, gfloat da)
{}

void moto_object_node_yaw(MotoObjectNode *self, gfloat da)
{}

void moto_object_node_apply_camera_transform(MotoObjectNode *self, gint width, gint height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(self->priv->camera)
    {
        moto_camera_node_apply(self->priv->camera, width, height);
    }
    else
    {
        MotoWorld *world = moto_node_get_world((MotoNode *)self);
        if( ! world)
        {
            GString *msg = g_string_new("Object \"");
             g_string_append(msg, moto_node_get_name((MotoNode *)self));
            g_string_append(msg, "\" that is used as a camera has no associated world.");
            moto_error(msg->str);
            g_string_free(msg, TRUE);
        }

        moto_world_apply_default_camera(world, width, height);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    glMultMatrixf(moto_object_node_get_inverse_matrix(self, TRUE));
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

void moto_object_node_set_material(MotoObjectNode *self, MotoMaterialNode *material)
{
    self->priv->material = material;
}

void moto_object_node_set_camera(MotoObjectNode *self, MotoCameraNode *camera)
{
    self->priv->camera = camera;
}

/*
static void
moto_object_node_convert_camera_transform(MotoObjectNode *self)
{
    gfloat translate[3];
    gfloat tmpvec[3], rotate[3];
    gfloat rx, ry, rz;

    translate_from_matrix44(translate, self->priv->camera_transform);
    rotate_x_from_matrix44(rotate, self->priv->camera_transform, tmpvec);
    rx = rotate[0];
    rotate_y_from_matrix44(rotate, self->priv->camera_transform, tmpvec);
    ry = rotate[0];
    rotate_z_from_matrix44(rotate, self->priv->camera_transform, tmpvec);
    rz = rotate[0];

    moto_object_node_set_translate_array(self, translate);
    moto_object_node_set_rotate(self, rx, ry, rz);
}
*/

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

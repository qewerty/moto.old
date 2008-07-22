#include "math.h"

#include "GL/gl.h"
#include "GL/glu.h"

#include "moto-world.h"
#include "moto-messager.h"
#include "moto-object-node.h"
#include "moto-material-node.h"
#include "moto-camera-node.h"

#include "common/matrix.h"
#include "common/numdef.h"
#include "common/print-array.h"

/* forward */

static void moto_object_node_calc_transform(MotoObjectNode *self);
static void moto_object_node_calc_inverse_transform(MotoObjectNode *self);

void moto_object_node_set_translate(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);
void moto_object_node_set_rotate(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);
void moto_object_node_set_scale(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);

// static void moto_object_node_convert_camera_transform(MotoObjectNode *self);

/* enums */

GType moto_transform_order_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        GEnumValue values[] = {
            {MOTO_TRANSFORM_ORDER_TRS, "TRANSFORM_ORDER_TRS", "TRANSFORM_ORDER_TRS"},
            {MOTO_TRANSFORM_ORDER_TSR, "TRANSFORM_ORDER_TSR", "TRANSFORM_ORDER_TSR"},
            {MOTO_TRANSFORM_ORDER_RTS, "TRANSFORM_ORDER_RTS", "TRANSFORM_ORDER_RTS"},
            {MOTO_TRANSFORM_ORDER_RST, "TRANSFORM_ORDER_RST", "TRANSFORM_ORDER_RST"},
            {MOTO_TRANSFORM_ORDER_STR, "TRANSFORM_ORDER_STR", "TRANSFORM_ORDER_STR"},
            {MOTO_TRANSFORM_ORDER_SRT, "TRANSFORM_ORDER_SRT", "TRANSFORM_ORDER_SRT"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoTransformOrder", values);
    }
    return type;
}

GType moto_rotate_order_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        GEnumValue values[] = {
            {MOTO_ROTATE_ORDER_XYZ, "ROTATE_ORDER_XYZ", "ROTATE_ORDER_XYZ"},
            {MOTO_ROTATE_ORDER_XZY, "ROTATE_ORDER_XZY", "ROTATE_ORDER_XZY"},
            {MOTO_ROTATE_ORDER_YXZ, "ROTATE_ORDER_YXZ", "ROTATE_ORDER_YXZ"},
            {MOTO_ROTATE_ORDER_YZX, "ROTATE_ORDER_YZX", "ROTATE_ORDER_YZX"},
            {MOTO_ROTATE_ORDER_ZXY, "ROTATE_ORDER_ZXY", "ROTATE_ORDER_ZXY"},
            {MOTO_ROTATE_ORDER_ZYX, "ROTATE_ORDER_ZYX", "ROTATE_ORDER_ZYX"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoRotateOrder", values);
    }
    return type;
}

/* class ObjectNode */

static GObjectClass *object_node_parent_class = NULL;

struct _MotoObjectNodePriv
{
    MotoGeomViewNode **view_ptr;
    MotoMaterialNode **material_ptr;
    MotoCameraNode **camera_ptr;

    gfloat *tx_ptr, *ty_ptr, *tz_ptr;
    gfloat *rx_ptr, *ry_ptr, *rz_ptr;
    gfloat *sx_ptr, *sy_ptr, *sz_ptr;

    MotoTransformOrder      transform_order;
    MotoRotateOrder         rotate_order;
    MotoTransformOrder      *transform_order_ptr;
    MotoRotateOrder         *rotate_order_ptr;

    gboolean translate_calculated,
             rotate_calculated,
             scale_calculated,
             transform_calculated,
             inverse_calculated;

    gboolean *keep_transform_ptr;

    gboolean *visible_ptr;

    MotoObjectNode *parent;
    GSList *children;

    MotoBound *local_bound;
    gboolean local_bound_calculated;
    MotoBound *global_bound;
    gboolean global_bound_calculated;

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

    g_object_unref(self->priv->local_bound);
    g_object_unref(self->priv->global_bound);
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
    MotoNode *node = (MotoNode *)self;
    self->priv = g_slice_new(MotoObjectNodePriv);

    self->priv->transform_order     = MOTO_TRANSFORM_ORDER_TRS;
    self->priv->rotate_order        = MOTO_ROTATE_ORDER_XYZ;

    self->priv->local_bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    self->priv->global_bound = moto_bound_new(0, 0, 0, 0, 0, 0);

    /*
    moto_object_node_set_translate(self, 0, 0, 0);
    moto_object_node_set_rotate(self, 0, 0, 0);
    moto_object_node_set_scale(self, 1, 1, 1);
    */

    self->priv->parent = NULL;
    self->priv->children = NULL;

    /* optimizations */

    self->priv->inverse_calculated      = FALSE;
    self->priv->local_bound_calculated  = FALSE;
    self->priv->global_bound_calculated = FALSE;

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "parent", "Parent",  MOTO_TYPE_OBJECT_NODE, MOTO_PARAM_MODE_IN, NULL, pspec,        "Transform", "Transform",
            "transform", "Transform",  MOTO_TYPE_OBJECT_NODE, MOTO_PARAM_MODE_OUT, self, pspec,        "Transform", "Transform",
            "tx", "Transform X", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Transform", "Transform/Translate",
            "ty", "Transform Y", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Transform", "Transform/Translate",
            "tz", "Transform Z", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Transform", "Transform/Translate",
            "rx", "Rotate X",    G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Transform", "Transform/Rotate",
            "ry", "Rotate Y",    G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Transform", "Transform/Rotate",
            "rz", "Rotate Z",    G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Transform", "Transform/Rotate",
            "sx", "Scale X",     G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Transform", "Transform/Scale",
            "sy", "Scale Y",     G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Transform", "Transform/Scale",
            "sz", "Scale Z",     G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Transform", "Transform/Scale",
            "to", "Transform Order", MOTO_TYPE_TRANSFORM_ORDER, MOTO_PARAM_MODE_INOUT, MOTO_TRANSFORM_ORDER_TRS, pspec,   "Transform", "Transform/Misc",
            "ro", "Rotate Order", MOTO_TYPE_ROTATE_ORDER, MOTO_PARAM_MODE_INOUT, MOTO_ROTATE_ORDER_XYZ, pspec,   "Transform", "Transform/Misc",
            "kt", "Keep Transform", G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec,         "Transform", "Transform/Misc",
            "visible", "Visible",   G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec,         "View", "View",
            "view", "View",   MOTO_TYPE_GEOMETRY_VIEW_NODE, MOTO_PARAM_MODE_INOUT, NULL, pspec, "View", "View",
            "cam",  "Camera",   MOTO_TYPE_CAMERA_NODE, MOTO_PARAM_MODE_INOUT, NULL, pspec,      "View", "View",
            "material", "Material",   MOTO_TYPE_MATERIAL_NODE, MOTO_PARAM_MODE_INOUT, NULL, pspec, "Shading", "Shading/Material",
            NULL);

    /* pointers */

    self->priv->tx_ptr = moto_node_param_value_pointer(node, "tx", gfloat);
    self->priv->ty_ptr = moto_node_param_value_pointer(node, "ty", gfloat);
    self->priv->tz_ptr = moto_node_param_value_pointer(node, "tz", gfloat);
    self->priv->rx_ptr = moto_node_param_value_pointer(node, "rx", gfloat);
    self->priv->ry_ptr = moto_node_param_value_pointer(node, "ry", gfloat);
    self->priv->rz_ptr = moto_node_param_value_pointer(node, "rz", gfloat);
    self->priv->sx_ptr = moto_node_param_value_pointer(node, "sx", gfloat);
    self->priv->sy_ptr = moto_node_param_value_pointer(node, "sy", gfloat);
    self->priv->sz_ptr = moto_node_param_value_pointer(node, "sz", gfloat);

    self->priv->transform_order_ptr = moto_node_param_value_pointer(node, "to", MotoTransformOrder);
    self->priv->rotate_order_ptr = moto_node_param_value_pointer(node, "ro", MotoRotateOrder);

    self->priv->keep_transform_ptr = moto_node_param_value_pointer(node, "kt", gboolean);

    self->priv->visible_ptr = moto_node_param_value_pointer(node, "visible", gboolean);

    self->priv->camera_ptr = moto_node_param_value_pointer(node, "cam", MotoCameraNode*);
    self->priv->view_ptr = moto_node_param_value_pointer(node, "view", MotoGeomViewNode*);
    self->priv->material_ptr = moto_node_param_value_pointer(node, "material", MotoMaterialNode*);;

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

MotoObjectNode *moto_object_node_new(const gchar *name)
{
    MotoObjectNode *self = \
        (MotoObjectNode *)g_object_new(MOTO_TYPE_OBJECT_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

/* translate */

void moto_object_node_set_translate(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    MotoNode *node = (MotoNode *)self;

    g_value_set_float(moto_node_get_param_value(node, "tx"), x);
    g_value_set_float(moto_node_get_param_value(node, "ty"), y);
    g_value_set_float(moto_node_get_param_value(node, "tz"), z);

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    MotoNode *node = (MotoNode *)self;

    g_value_set_float(moto_node_get_param_value(node, "tx"), xyz[0]);
    g_value_set_float(moto_node_get_param_value(node, "ty"), xyz[1]);
    g_value_set_float(moto_node_get_param_value(node, "tz"), xyz[2]);

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/* rotate */

void moto_object_node_set_rotate(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    MotoNode *node = (MotoNode *)self;

    g_value_set_float(moto_node_get_param_value(node, "rx"), x);
    g_value_set_float(moto_node_get_param_value(node, "ry"), y);
    g_value_set_float(moto_node_get_param_value(node, "rz"), z);

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    MotoNode *node = (MotoNode *)self;

    g_value_set_float(moto_node_get_param_value(node, "rx"), xyz[0]);
    g_value_set_float(moto_node_get_param_value(node, "ry"), xyz[1]);
    g_value_set_float(moto_node_get_param_value(node, "rz"), xyz[2]);

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/* scale */

void moto_object_node_set_scale(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    MotoNode *node = (MotoNode *)self;

    g_value_set_float(moto_node_get_param_value(node, "sx"), x);
    g_value_set_float(moto_node_get_param_value(node, "sy"), y);
    g_value_set_float(moto_node_get_param_value(node, "sz"), z);

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    MotoNode *node = (MotoNode *)self;

    g_value_set_float(moto_node_get_param_value(node, "sx"), xyz[0]);
    g_value_set_float(moto_node_get_param_value(node, "sy"), xyz[1]);
    g_value_set_float(moto_node_get_param_value(node, "sz"), xyz[2]);

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
    if( ! detbuf)
    {
        // Exception?
    }

    self->priv->inverse_calculated = TRUE;
}

gfloat *moto_object_node_get_matrix(MotoObjectNode *self, gboolean global)
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

static void calc_global_bound(MotoObjectNode *self)
{
    MotoParam *p = moto_node_get_param((MotoNode *)self, "view");
    MotoParam *s = moto_param_get_source(p);
    if( ! s)
        return;
    MotoGeomViewNode *gvn = (MotoGeomViewNode *)moto_param_get_node(s);
    MotoGeometryNode *gn = moto_geom_view_node_get_geometry(gvn);
    if( ! gn)
        return;

    // MotoBound *b = moto_geometry_node_get_bound(gn);

    /* TODO */

    self->priv->global_bound_calculated = TRUE;
}

static void calc_local_bound(MotoObjectNode *self)
{
    MotoParam *p = moto_node_get_param((MotoNode *)self, "view");
    MotoParam *s = moto_param_get_source(p);
    if( ! s)
        return;
    MotoGeomViewNode *gvn = (MotoGeomViewNode *)moto_param_get_node(s);
    MotoGeometryNode *gn = moto_geom_view_node_get_geometry(gvn);
    if( ! gn)
        return;

    moto_bound_copy(self->priv->local_bound, moto_geometry_node_get_bound(gn));

    self->priv->local_bound_calculated = TRUE;
}

MotoBound *moto_object_node_get_bound(MotoObjectNode *self, gboolean global)
{
    if(global)
    {
        if( ! self->priv->global_bound_calculated)
            calc_global_bound(self);
        return self->priv->global_bound;
    }

    if( ! self->priv->local_bound_calculated)
        calc_local_bound(self);
    return self->priv->local_bound;
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
    return *(self->priv->rotate_order_ptr);
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

    g_value_set_enum(moto_node_get_param_value((MotoNode *)self, "ro"), order);
}

gboolean moto_object_node_get_keep_transform(MotoObjectNode *self)
{
    return *(self->priv->keep_transform_ptr);
}

void moto_object_node_set_keep_transform(MotoObjectNode *self, gboolean kt)
{
    if(kt != (*(self->priv->keep_transform_ptr)))
    {
        self->priv->transform_calculated = FALSE;
        g_value_set_enum(moto_node_get_param_value((MotoNode *)self, "kt"), kt);
    }
}

static void moto_object_node_calc_translate(MotoObjectNode *self)
{
    if(self->priv->translate_calculated)
        return;

    matrix44_translate(self->priv->translate_matrix,
            (*(self->priv->tx_ptr)), (*(self->priv->ty_ptr)), (*(self->priv->tz_ptr)));

    self->priv->translate_calculated = TRUE;
}

static void moto_object_node_calc_rotate(MotoObjectNode *self)
{
    if(self->priv->rotate_calculated)
        return;

    gfloat tmp[16], tmp2[16];

    switch(self->priv->rotate_order)
    {
     case MOTO_ROTATE_ORDER_XYZ:
        matrix44_rotate_x(tmp, (*(self->priv->rx_ptr)));
        matrix44_rotate_y(self->priv->rotate_matrix, (*(self->priv->ry_ptr)));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_z(tmp, (*(self->priv->rz_ptr)));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_XZY:
        matrix44_rotate_x(tmp, (*(self->priv->rx_ptr)));
        matrix44_rotate_z(self->priv->rotate_matrix, (*(self->priv->rz_ptr)));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_y(tmp, (*(self->priv->ry_ptr)));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_YXZ:
        matrix44_rotate_y(tmp, (*(self->priv->ry_ptr)));
        matrix44_rotate_x(self->priv->rotate_matrix, (*(self->priv->rx_ptr)));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_z(tmp, (*(self->priv->rz_ptr)));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_YZX:
        matrix44_rotate_y(tmp, (*(self->priv->ry_ptr)));
        matrix44_rotate_z(self->priv->rotate_matrix, (*(self->priv->rz_ptr)));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_x(tmp, (*(self->priv->rx_ptr)));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_ZXY:
        matrix44_rotate_z(tmp, (*(self->priv->rz_ptr)));
        matrix44_rotate_x(self->priv->rotate_matrix, (*(self->priv->rx_ptr)));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_y(tmp, (*(self->priv->ry_ptr)));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_ZYX:
        matrix44_rotate_z(tmp, (*(self->priv->rz_ptr)));
        matrix44_rotate_y(self->priv->rotate_matrix, (*(self->priv->ry_ptr)));
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_x(tmp, (*(self->priv->rx_ptr)));
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
    }

    self->priv->rotate_calculated = TRUE;
}

static void moto_object_node_calc_scale(MotoObjectNode *self)
{
    if(self->priv->scale_calculated)
        return;

    gfloat sx = (*(self->priv->sx_ptr));
    gfloat sy = (*(self->priv->sy_ptr));
    gfloat sz = (*(self->priv->sz_ptr));

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
    switch(self->priv->transform_order)
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
    glRotatef((*(self->priv->rx_ptr))*DEG_PER_RAD, 1, 0, 0);\
    glRotatef((*(self->priv->ry_ptr))*DEG_PER_RAD, 0, 1, 0);\
    glRotatef((*(self->priv->rz_ptr))*DEG_PER_RAD, 0, 0, 1);\
 break;\
 case MOTO_ROTATE_ORDER_XZY:\
    glRotatef((*(self->priv->rx_ptr))*DEG_PER_RAD, 1, 0, 0);\
    glRotatef((*(self->priv->rz_ptr))*DEG_PER_RAD, 0, 0, 1);\
    glRotatef((*(self->priv->ry_ptr))*DEG_PER_RAD, 0, 1, 0);\
 break;\
 case MOTO_ROTATE_ORDER_YXZ:\
    glRotatef((*(self->priv->ry_ptr))*DEG_PER_RAD, 0, 1, 0);\
    glRotatef((*(self->priv->rx_ptr))*DEG_PER_RAD, 1, 0, 0);\
    glRotatef((*(self->priv->rz_ptr))*DEG_PER_RAD, 0, 0, 1);\
 break;\
 case MOTO_ROTATE_ORDER_YZX:\
    glRotatef((*(self->priv->ry_ptr))*DEG_PER_RAD, 0, 1, 0);\
    glRotatef((*(self->priv->rz_ptr))*DEG_PER_RAD, 0, 0, 1);\
    glRotatef((*(self->priv->rx_ptr))*DEG_PER_RAD, 1, 0, 0);\
 break;\
 case MOTO_ROTATE_ORDER_ZXY:\
    glRotatef((*(self->priv->rz_ptr))*DEG_PER_RAD, 0, 0, 1);\
    glRotatef((*(self->priv->rx_ptr))*DEG_PER_RAD, 1, 0, 0);\
    glRotatef((*(self->priv->ry_ptr))*DEG_PER_RAD, 0, 1, 0);\
 break;\
 case MOTO_ROTATE_ORDER_ZYX:\
    glRotatef((*(self->priv->rz_ptr))*DEG_PER_RAD, 0, 0, 1);\
    glRotatef((*(self->priv->ry_ptr))*DEG_PER_RAD, 0, 1, 0);\
    glRotatef((*(self->priv->rx_ptr))*DEG_PER_RAD, 1, 0, 0);\
    break;\
}

static void apply_transform(MotoObjectNode *self)
{
    glMultMatrixf(moto_object_node_get_matrix(self, FALSE));
}

static void apply_global_transform(MotoObjectNode *self)
{
    glMultMatrixf(moto_object_node_get_matrix(self, TRUE));
}

void moto_object_node_draw_full(MotoObjectNode *self,
        gboolean recursive, gboolean use_global)
{
    if( ! *(self->priv->visible_ptr))
        return;

    glPushMatrix();

    if(use_global)
        apply_global_transform(self);
    else
        apply_transform(self);

    if(*(self->priv->material_ptr))
    {
        moto_material_node_use(*(self->priv->material_ptr));
    }

    if(*(self->priv->view_ptr))
    {
        moto_geom_view_node_draw(*(self->priv->view_ptr));
    }

    if(recursive)
    {
        GSList *child = self->priv->children;
        for(; child; child = g_slist_next(child))
        {
            moto_object_node_draw((MotoObjectNode *)child->data);
        }
    }

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
        {(*(self->priv->tx_ptr)), (*(self->priv->ty_ptr)), (*(self->priv->tz_ptr))};
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

void moto_object_node_tumble_h(MotoObjectNode *self, gfloat da)
{
    da = da*RAD_PER_DEG;

    gfloat ax[] = {1, 0, 0};
    gfloat ay[] = {0, 1, 0};
    gfloat az[] = {0, 0, 1};
    gfloat u[3], v[3], n[3], t[3];
    gfloat to_u[3], to_v[3], to_n[3];

    gfloat to_eye[3], eye[3];
    gfloat loc_pos[] = {0, 0, 0};
    gfloat tumble_axis[3];
    gfloat target[3];
    vector3_copy(target, self->priv->target);

    gfloat *matrix = moto_object_node_get_matrix(self, TRUE);

    vector3_transform(u, matrix, ax);
    vector3_transform(v, matrix, ay);
    vector3_transform(n, matrix, az);

    vector3_copy(tumble_axis, u);

    point3_transform(eye, matrix, loc_pos);

    vector3_dif(to_eye, eye, target);
    vector3_sum(to_u, to_eye, u);
    vector3_sum(to_v, to_eye, v);
    vector3_sum(to_n, to_eye, n);

    gfloat rm[16];
    matrix44_rotate_from_axis(rm, da, tumble_axis[0], tumble_axis[1], tumble_axis[2]);

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
    vector3_dif(u, to_u2, to_eye2);
    vector3_dif(v, to_v2, to_eye2);
    vector3_dif(n, to_n2, to_eye2);

    vector3_normalize(u, t[0]);
    vector3_normalize(v, t[0]);
    vector3_normalize(n, t[0]);

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

    gfloat identity[16], mult[16];
    matrix44_identity(identity);
    matrix44_mult(mult, igm, gm);
    // g_assert(matrix44_equal_dif(identity, mult, MICRO));

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

void moto_object_node_tumble_v(MotoObjectNode *self, gfloat da)
{
    da = da*RAD_PER_DEG;

    gfloat ax[] = {1, 0, 0};
    gfloat ay[] = {0, 1, 0};
    gfloat az[] = {0, 0, 1};
    gfloat u[3], v[3], n[3], t[3];
    gfloat to_u[3], to_v[3], to_n[3];

    gfloat to_eye[3], eye[3];
    gfloat loc_pos[] = {0, 0, 0};
    gfloat tumble_axis[] = {0, 1, 0};
    gfloat target[3];
    vector3_copy(target, self->priv->target);

    gfloat *matrix = moto_object_node_get_matrix(self, TRUE);

    vector3_transform(u, matrix, ax);
    vector3_transform(v, matrix, ay);
    vector3_transform(n, matrix, az);

    point3_transform(eye, matrix, loc_pos);

    vector3_dif(to_eye, eye, target);
    vector3_sum(to_u, to_eye, u);
    vector3_sum(to_v, to_eye, v);
    vector3_sum(to_n, to_eye, n);

    gfloat rm[16];
    matrix44_rotate_from_axis(rm, da, tumble_axis[0], tumble_axis[1], tumble_axis[2]);

    /* eye rotation */
    gfloat to_eye2[3];
    point3_transform(to_eye2, rm, to_eye);

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
    vector3_dif(u, to_u2, to_eye2);
    vector3_dif(v, to_v2, to_eye2);
    vector3_dif(n, to_n2, to_eye2);

    vector3_normalize(u, t[0]);
    vector3_normalize(v, t[0]);
    vector3_normalize(n, t[0]);

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
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(*(self->priv->camera_ptr))
    {
        moto_camera_node_apply(*(self->priv->camera_ptr), width, height);
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
    glLoadMatrixf(moto_object_node_get_inverse_matrix(self, TRUE));
}

gboolean moto_object_node_get_visible(MotoObjectNode *self)
{
    return *(self->priv->visible_ptr);
}

void moto_object_node_set_visible(MotoObjectNode *self, gboolean visible)
{
    g_value_set_boolean(moto_node_get_param_value((MotoNode *)self, "visible"), visible);
}

gboolean moto_object_node_button_press(MotoObjectNode *self,
    gint x, gint y, gint width, gint height, MotoRay *ray,
    MotoTransformInfo *tinfo)
{
    if(*(self->priv->view_ptr))
        if(moto_geom_view_node_process_button_press(*(self->priv->view_ptr),
                    x, y, width, height, ray, tinfo))
            return TRUE;

    return FALSE;
}

gboolean moto_object_node_button_release(MotoObjectNode *self,
    gint x, gint y, gint width, gint height)
{
    if(*(self->priv->view_ptr))
        if(moto_geom_view_node_process_button_release(*(self->priv->view_ptr), x, y, width, height))
            return TRUE;

    GSList *child = self->priv->children;
    for(; child; child = g_slist_next(child))
    {
        if(moto_object_node_button_release((MotoObjectNode *)child->data,
                x, y, width, height))
            return TRUE;
    }

    return FALSE;
}

gboolean moto_object_node_motion(MotoObjectNode *self,
    gint x, gint y, gint width, gint height)
{
    if(*(self->priv->view_ptr))
        if(moto_geom_view_node_process_motion(*(self->priv->view_ptr), x, y, width, height))
            return TRUE;

    GSList *child = self->priv->children;
    for(; child; child = g_slist_next(child))
    {
        if(moto_object_node_motion((MotoObjectNode *)child->data,
                x, y, width, height))
            return TRUE;
    }

    return FALSE;
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


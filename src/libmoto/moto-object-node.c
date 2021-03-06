#include "math.h"

#include "GL/glew.h"
#include "GL/glu.h"

#include "moto-types.h"
#include "moto-scene-node.h"
#include "moto-messager.h"
#include "moto-object-node.h"
#include "moto-material-node.h"
#include "moto-shape-node.h"
#include "moto-mesh.h"

#include "libmotoutil/xform.h"
#include "libmotoutil/numdef.h"
#include "libmotoutil/print-array.h"

/* forwards */

static void moto_object_node_calc_transform(MotoObjectNode *self);
static void moto_object_node_calc_inverse_transform(MotoObjectNode *self);

void moto_object_node_set_translate(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);
void moto_object_node_set_rotate(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);
void moto_object_node_set_scale(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);

static void moto_object_node_update(MotoNode *self);

// static void moto_object_node_convert_camera_transform(MotoObjectNode *self);

/* enums */

GType moto_transform_order_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_TRANSFORM_ORDER_TRS, "TRANSFORM_ORDER_TRS", "TRS"},
            {MOTO_TRANSFORM_ORDER_TSR, "TRANSFORM_ORDER_TSR", "TSR"},
            {MOTO_TRANSFORM_ORDER_RTS, "TRANSFORM_ORDER_RTS", "RTS"},
            {MOTO_TRANSFORM_ORDER_RST, "TRANSFORM_ORDER_RST", "RST"},
            {MOTO_TRANSFORM_ORDER_STR, "TRANSFORM_ORDER_STR", "STR"},
            {MOTO_TRANSFORM_ORDER_SRT, "TRANSFORM_ORDER_SRT", "SRT"},
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
        static GEnumValue values[] = {
            {MOTO_ROTATE_ORDER_XYZ, "ROTATE_ORDER_XYZ", "XYZ"},
            {MOTO_ROTATE_ORDER_XZY, "ROTATE_ORDER_XZY", "XZY"},
            {MOTO_ROTATE_ORDER_YXZ, "ROTATE_ORDER_YXZ", "YXZ"},
            {MOTO_ROTATE_ORDER_YZX, "ROTATE_ORDER_YZX", "YZX"},
            {MOTO_ROTATE_ORDER_ZXY, "ROTATE_ORDER_ZXY", "ZXY"},
            {MOTO_ROTATE_ORDER_ZYX, "ROTATE_ORDER_ZYX", "ZYX"},
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
    gboolean disposed;

    MotoTransformOrder      transform_order;
    MotoRotateOrder         rotate_order;

    gboolean translate_calculated,
             rotate_calculated,
             scale_calculated,
             transform_calculated,
             inverse_calculated;

    MotoObjectNode *parent;
    GSList *children;

    MotoShapeSelection* selection;

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

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_object_unref(self->priv->local_bound);
    g_object_unref(self->priv->global_bound);

    G_OBJECT_CLASS(object_node_parent_class)->dispose(G_OBJECT(self));
}

static void
moto_object_node_finalize(GObject *obj)
{
    MotoObjectNode *self = (MotoObjectNode *)obj;
    g_slice_free(MotoObjectNodePriv, self->priv);

    object_node_parent_class->finalize(obj);
}

static void
moto_object_node_init(MotoObjectNode *self)
{
    MotoNode *node = (MotoNode *)self;
    self->priv = g_slice_new(MotoObjectNodePriv);
    self->priv->disposed = FALSE;

    self->priv->transform_order = MOTO_TRANSFORM_ORDER_TRS;
    self->priv->rotate_order = MOTO_ROTATE_ORDER_XYZ;

    self->priv->selection = NULL;

    self->priv->local_bound  = moto_bound_new(0, 0, 0, 0, 0, 0);
    self->priv->global_bound = moto_bound_new(0, 0, 0, 0, 0, 0);

    self->priv->parent = NULL;
    self->priv->children = NULL;

    self->priv->inverse_calculated      = FALSE;
    self->priv->local_bound_calculated  = FALSE;
    self->priv->global_bound_calculated = FALSE;

    gfloat translate[3] = {0, 0, 0};
    gfloat rotate[3]    = {0, 0, 0};
    gfloat scale[3]     = {1, 1, 1};

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "t", "Translate", MOTO_TYPE_FLOAT3, MOTO_PARAM_MODE_INOUT, translate, pspec, "Transform",
            "r", "Rotate",    MOTO_TYPE_FLOAT3, MOTO_PARAM_MODE_INOUT, rotate, pspec, "Transform",
            "s", "Scale",     MOTO_TYPE_FLOAT3, MOTO_PARAM_MODE_INOUT, scale, pspec, "Transform",
            "fov", "Field of View", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 60.0, NULL, "Projection",
            "near", "Near Plane", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.3, NULL, "Projection",
            "far", "Far Plane", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 150.0, NULL, "Projection",
            "to", "Transform Order", MOTO_TYPE_TRANSFORM_ORDER, MOTO_PARAM_MODE_INOUT, MOTO_TRANSFORM_ORDER_SRT, pspec, "Transform/Misc",
            "ro", "Rotate Order", MOTO_TYPE_ROTATE_ORDER, MOTO_PARAM_MODE_INOUT, MOTO_ROTATE_ORDER_XYZ, pspec, "Transform/Misc",
            "kt", "Keep Transform", MOTO_TYPE_BOOL, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Transform/Misc",
            "visible", "Visible",   MOTO_TYPE_BOOL, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "View",
            "use_local_mode", "Use Local Mode",   MOTO_TYPE_BOOL, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "View",
            "mode", "Selection Mode",   MOTO_TYPE_SELECTION_MODE, MOTO_PARAM_MODE_INOUT, MOTO_SELECTION_MODE_OBJECT, pspec, "View",
            "shape", "Shape",   MOTO_TYPE_SHAPE_NODE, MOTO_PARAM_MODE_INOUT, NULL, NULL, "View",
            "material", "Material", MOTO_TYPE_MATERIAL_NODE, MOTO_PARAM_MODE_INOUT, NULL, pspec, "Shading/Material",
            NULL);

    /* camera */

    self->priv->target[0] = 0;
    self->priv->target[1] = 0;
    self->priv->target[2] = 0;
}

static void
moto_object_node_class_init(MotoObjectNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = MOTO_NODE_CLASS(klass);

    object_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    nclass->update = moto_object_node_update;

    goclass->dispose = moto_object_node_dispose;
    goclass->finalize = moto_object_node_finalize;
}

G_DEFINE_TYPE(MotoObjectNode, moto_object_node, MOTO_TYPE_NODE);

/* Methods of class ObjectNode */

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

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *t = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "t"));
    vector3_set(t, x, y, z);

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_translate_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    MotoNode *node = (MotoNode *)self;

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *t = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "t"));
    vector3_set(t, xyz[0], xyz[1], xyz[2]);

    self->priv->translate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/* rotate */

void moto_object_node_set_rotate(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    MotoNode *node = (MotoNode *)self;

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *r = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "r"));
    vector3_set(r, x, y, z);

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_rotate_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    MotoNode *node = (MotoNode *)self;

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *r = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "r"));
    vector3_set(r, xyz[0], xyz[1], xyz[2]);

    self->priv->rotate_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

/* scale */

void moto_object_node_set_scale(MotoObjectNode *self,
        gfloat x, gfloat y, gfloat z)
{
    MotoNode *node = (MotoNode *)self;

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *s = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "s"));
    vector3_set(s, x, y, z);

    self->priv->scale_calculated = FALSE;
    self->priv->transform_calculated = FALSE;
}

void moto_object_node_set_scale_array(MotoObjectNode *self,
        gfloat xyz[3])
{
    MotoNode *node = (MotoNode *)self;

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *s = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "s"));
    vector3_set(s, xyz[0], xyz[1], xyz[2]);

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
    MotoObjectNodePriv* priv = self->priv;

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

static void update_local_bound(MotoObjectNode *self);

static void update_global_bound(MotoObjectNode *self)
{
    update_local_bound(self);
    // TODO:
    moto_bound_copy(self->priv->global_bound, self->priv->local_bound);
    // self->priv->global_bound_calculated = TRUE;
}

static void update_local_bound(MotoObjectNode *self)
{
    MotoShapeNode* shape_node = \
        moto_object_node_get_shape(self);
    if(!shape_node)
    {
        moto_bound_set(self->priv->local_bound, 0, 0, 0, 0, 0, 0);
        return;
    }

    MotoShape* shape = moto_shape_node_get_shape(shape_node);
    if(!shape || !MOTO_IS_MESH(shape))
    {
        moto_bound_set(self->priv->local_bound, 0, 0, 0, 0, 0, 0);
        return;
    }

    MotoMesh* mesh = (MotoMesh*)shape;
    moto_mesh_calc_bound(mesh, self->priv->local_bound);

    self->priv->local_bound_calculated = TRUE;
}

MotoBound *moto_object_node_get_bound(MotoObjectNode *self, gboolean global)
{
    if(global)
    {
        update_global_bound(self);
        return self->priv->global_bound;
    }

    update_local_bound(self);
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
    MotoRotateOrder r;
    moto_node_get_param_enum((MotoNode*)self, "ro", (gint*)&r);
    return r;
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

    moto_node_set_param_enum((MotoNode *)self, "ro", order);
}

gboolean moto_object_node_get_keep_transform(MotoObjectNode *self)
{
    gboolean r = moto_node_get_param_boolean((MotoNode*)self, "kt", &r);
    return r;
}

void moto_object_node_set_keep_transform(MotoObjectNode *self, gboolean kt)
{
    if(kt != moto_object_node_get_keep_transform(self))
    {
        self->priv->transform_calculated = FALSE;
        moto_node_set_param_boolean((MotoNode *)self, "kt", kt);
    }
}

static void moto_object_node_calc_translate(MotoObjectNode *self)
{
    if(self->priv->translate_calculated)
        return;

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *t = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "t"));

    matrix44_translate(self->priv->translate_matrix, t[0], t[1], t[2]);

    self->priv->translate_calculated = TRUE;
}

static void moto_object_node_calc_rotate(MotoObjectNode *self)
{
    if(self->priv->rotate_calculated)
        return;

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *r = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "r"));

    gfloat tmp[16], tmp2[16];

    gfloat rx = r[0]*RAD_PER_DEG;
    gfloat ry = r[1]*RAD_PER_DEG;
    gfloat rz = r[2]*RAD_PER_DEG;

    switch(self->priv->rotate_order)
    {
     case MOTO_ROTATE_ORDER_XYZ:
        matrix44_rotate_x(tmp, rx);
        matrix44_rotate_y(self->priv->rotate_matrix, ry);
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_z(tmp, rz);
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_XZY:
        matrix44_rotate_x(tmp, rx);
        matrix44_rotate_z(self->priv->rotate_matrix, rz);
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_y(tmp, ry);
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_YXZ:
        matrix44_rotate_y(tmp, ry);
        matrix44_rotate_x(self->priv->rotate_matrix, rx);
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_z(tmp, rz);
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_YZX:
        matrix44_rotate_y(tmp, ry);
        matrix44_rotate_z(self->priv->rotate_matrix, rz);
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_x(tmp, rx);
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_ZXY:
        matrix44_rotate_z(tmp, rz);
        matrix44_rotate_x(self->priv->rotate_matrix, rx);
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_y(tmp, ry);
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
     case MOTO_ROTATE_ORDER_ZYX:
        matrix44_rotate_z(tmp, rz);
        matrix44_rotate_y(self->priv->rotate_matrix, ry);
        matrix44_mult(tmp2, tmp, self->priv->rotate_matrix);
        matrix44_rotate_x(tmp, rx);
        matrix44_mult(self->priv->rotate_matrix, tmp2, tmp);
     break;
    }

    self->priv->rotate_calculated = TRUE;
}

static void moto_object_node_calc_scale(MotoObjectNode *self)
{
    if(self->priv->scale_calculated)
        return;

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *s = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "s"));

    gfloat sx = s[0];
    gfloat sy = s[1];
    gfloat sz = s[2];

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

static void apply_transform(MotoObjectNode *self)
{
    glMultMatrixf(moto_object_node_get_matrix(self, FALSE));
}

static void apply_global_transform(MotoObjectNode *self)
{
    glMultMatrixf(moto_object_node_get_matrix(self, TRUE));
}

MotoDrawMode moto_object_node_get_draw_mode(MotoObjectNode* self)
{
    MotoSceneNode* scene = moto_node_get_scene_node((MotoNode*)self);
    if(!scene)
        return MOTO_DRAW_MODE_SMOOTH;

    return moto_scene_node_get_draw_mode(scene);
}

void moto_object_node_draw(MotoObjectNode *self)
{
    gboolean visible;
    moto_node_get_param_boolean((MotoNode *)self, "visible", &visible);
    if(!visible)
        return;

    MotoMaterialNode *mat;
    moto_node_get_param_object((MotoNode *)self, "material", (GObject **)&mat);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    apply_global_transform(self);

    if(mat)
        moto_material_node_use(mat);

    MotoShapeNode* shape = moto_object_node_get_shape(self);
    if(shape)
    {
        moto_shape_node_draw(shape,
            moto_object_node_get_draw_mode(self),
            moto_object_node_get_selection(self),
            moto_object_node_get_selection_mode(self));
    }

    glPopMatrix();

    GList* child = moto_node_get_children((MotoNode*)self);
    for(; child; child = g_list_next(child))
    {
        if(MOTO_IS_OBJECT_NODE(child->data))
            moto_object_node_draw((MotoObjectNode *)child->data);
    }
}

MotoShapeNode* moto_object_node_get_shape(MotoObjectNode* self)
{
    GObject* node = NULL;
    moto_node_get_param_object((MotoNode*)self, "shape", &node);
    return (MotoShapeNode*)node;
}

MotoShapeSelection* moto_object_node_get_selection(MotoObjectNode* self)
{
    return self->priv->selection;
}

MotoSelectionMode moto_object_node_get_selection_mode(MotoObjectNode* self)
{
    MotoSelectionMode sm = MOTO_SELECTION_MODE_OBJECT;
    moto_node_get_param_enum((MotoNode*)self, "mode", (gint*)&sm);
    return sm;
}

void moto_object_node_set_selection_mode(MotoObjectNode* self, MotoSelectionMode mode)
{
    moto_node_set_param_enum((MotoNode*)self, "mode", (gint)mode);

    MotoShapeNode* shape_node = moto_object_node_get_shape(self);
    if(shape_node)
        moto_shape_node_reset(shape_node);
}

void moto_object_node_select_more(MotoObjectNode* self)
{
    MotoShapeNode* shape = moto_object_node_get_shape(self);
    if(shape)
    {
        moto_shape_node_select_more(shape,
            moto_object_node_get_selection(self),
            moto_object_node_get_selection_mode(self));
    }
}

void moto_object_node_select_less(MotoObjectNode* self)
{
    MotoShapeNode* shape = moto_object_node_get_shape(self);
    if(shape)
    {
        moto_shape_node_select_less(shape,
            moto_object_node_get_selection(self),
            moto_object_node_get_selection_mode(self));
    }
}

void moto_object_node_select_inverse(MotoObjectNode* self)
{
    MotoShapeNode* shape = moto_object_node_get_shape(self);
    if(shape)
    {
        moto_shape_node_select_inverse(shape,
            moto_object_node_get_selection(self),
            moto_object_node_get_selection_mode(self));
    }
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

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *t = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "t"));

    gfloat pos[3] = {t[0], t[1], t[2]};
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

    euler[0] *= DEG_PER_RAD;
    euler[1] *= DEG_PER_RAD;
    euler[2] *= DEG_PER_RAD;
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

    euler[0] *= DEG_PER_RAD;
    euler[1] *= DEG_PER_RAD;
    euler[2] *= DEG_PER_RAD;
    moto_object_node_set_rotate_array(self, euler);
}

void moto_object_node_roll(MotoObjectNode *self, gfloat da)
{
    da *= RAD_PER_DEG;

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

    gfloat c = cos(da);
    gfloat s = sin(da);

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

    euler[0] *= DEG_PER_RAD;
    euler[1] *= DEG_PER_RAD;
    euler[2] *= DEG_PER_RAD;
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

    gfloat fov = 60;
    gfloat near = 0.1;
    gfloat far = 100;
    moto_node_get_param_float((MotoNode *)self, "fov", &fov);
    moto_node_get_param_float((MotoNode *)self, "near", &near);
    moto_node_get_param_float((MotoNode *)self, "far", &far);

    gdouble aspect = width/((double)height);
    gluPerspective(fov, aspect, near, far);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(moto_object_node_get_inverse_matrix(self, TRUE));
}

gboolean moto_object_node_get_visible(MotoObjectNode *self)
{
    gboolean r;
    moto_node_get_param_boolean((MotoNode *)self, "visible", &r);
    return r;
}

void moto_object_node_set_visible(MotoObjectNode *self, gboolean visible)
{
    g_value_set_boolean(moto_node_get_param_value((MotoNode *)self, "visible"), visible);
}

static void moto_object_node_update(MotoNode *self)
{
    MotoObjectNode *obj = (MotoObjectNode *)self;
    MotoObjectNodePriv *priv = obj->priv;

    priv->translate_calculated    = FALSE;
    priv->rotate_calculated       = FALSE;
    priv->scale_calculated        = FALSE;
    priv->transform_calculated    = FALSE;
    priv->inverse_calculated      = FALSE;
    priv->local_bound_calculated  = FALSE;
    priv->global_bound_calculated = FALSE;

    MotoShapeNode* shape_node = moto_object_node_get_shape(obj);
    if(shape_node)
    {
        MotoShape* shape = moto_shape_node_get_shape(shape_node);
        if(shape && MOTO_IS_MESH(shape))
        {
            MotoMesh* mesh = (MotoMesh*)shape;
            if(!priv->selection)
            {
                priv->selection = moto_mesh_create_selection(mesh); // TODO: Replace with moto_shape_create_selection
            }
            else
            {
                if(!moto_mesh_is_selection_valid(mesh, priv->selection))
                {
                    MotoShapeSelection *old = priv->selection;
                    priv->selection = moto_mesh_adapt_selection(mesh, old);
                    moto_shape_selection_free(old);
                }
            }
        }
        else
        {
            if(priv->selection)
                moto_shape_selection_free(priv->selection);
            priv->selection = NULL;
        }

        moto_shape_node_reset(shape_node);
    }
    else
    {
        if(priv->selection)
            moto_shape_selection_free(priv->selection);
        priv->selection = NULL;
    }
}

gboolean moto_object_node_button_press(MotoObjectNode *self,
    gint x, gint y, gint width, gint height, MotoRay *ray,
    MotoTransformInfo *tinfo)
{
    MotoShapeNode* shape_node = \
        moto_object_node_get_shape(self);

    if(shape_node)
    {
        return moto_shape_node_select(shape_node,
            moto_object_node_get_selection(self),
            moto_object_node_get_selection_mode(self),
            x, y, width, height, ray, tinfo);
    }

    return FALSE;
}

gboolean moto_object_node_button_release(MotoObjectNode *self,
    gint x, gint y, gint width, gint height)
{
    return FALSE;
}

gboolean moto_object_node_motion(MotoObjectNode *self,
    gint x, gint y, gint width, gint height)
{
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


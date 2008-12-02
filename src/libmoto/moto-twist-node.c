#include <math.h>


#include "moto-types.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-mesh.h" // FIXME: Temporary!
#include "moto-twist-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static void moto_twist_node_update(MotoNode *self);

/* class MotoTwistNode */

typedef struct _MotoTwistNodePriv MotoTwistNodePriv;
#define MOTO_TWIST_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_TWIST_NODE, MotoTwistNodePriv)

static GObjectClass *normal_move_node_parent_class = NULL;

struct _MotoTwistNodePriv
{
    MotoPointCloud *pc;
    MotoPointCloud *prev_pc;
    gfloat prev_angle;
};

static void
moto_twist_node_dispose(GObject *obj)
{
    normal_move_node_parent_class->dispose(obj);
}

static void
moto_twist_node_finalize(GObject *obj)
{
    MotoTwistNodePriv *priv = MOTO_TWIST_NODE_GET_PRIVATE(obj);

    if(priv->pc);
        g_object_unref(priv->pc);

    normal_move_node_parent_class->finalize(obj);
}

static void
moto_twist_node_init(MotoTwistNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoTwistNodePriv *priv = MOTO_TWIST_NODE_GET_PRIVATE(self);

    priv->pc = NULL;
    priv->prev_pc = NULL;
    priv->prev_angle = 0.0f;

    /* params */

    gfloat orig[3] = {0, 0, 0};
    gfloat dir[3] = {1, 1, 0};

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "angle",  "Angle",      G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Arguments", "Arguments",
            "orig",   "Origin",             MOTO_TYPE_FLOAT_3, MOTO_PARAM_MODE_INOUT, orig, pspec, "Arguments", "Arguments",
            "dir",    "Direction",          MOTO_TYPE_FLOAT_3, MOTO_PARAM_MODE_INOUT, dir, pspec, "Arguments", "Arguments",
            "in_pc",  "Input Point Cloud",  MOTO_TYPE_POINT_CLOUD, MOTO_PARAM_MODE_IN, NULL, pspec, "Geometry", "Geometry",
            "out_pc", "Output Point Cloud", MOTO_TYPE_POINT_CLOUD, MOTO_PARAM_MODE_OUT, NULL, pspec, "Geometry", "Geometry",
            NULL);
}

static void
moto_twist_node_class_init(MotoTwistNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoTwistNodePriv));

    normal_move_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    goclass->dispose    = moto_twist_node_dispose;
    goclass->finalize   = moto_twist_node_finalize;

    nclass->update = moto_twist_node_update;
}

G_DEFINE_TYPE(MotoTwistNode, moto_twist_node, MOTO_TYPE_NODE);

/* methods of class MotoTwistNode */

MotoTwistNode *moto_twist_node_new(const gchar *name)
{
    MotoTwistNode *self = (MotoTwistNode *)g_object_new(MOTO_TYPE_TWIST_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void moto_twist_node_update(MotoNode *self)
{
    MotoTwistNodePriv *priv = MOTO_TWIST_NODE_GET_PRIVATE(self);

    // FIXME
    GValue *vorig = moto_node_get_param_value(self, "orig");
    GValue *vdir  = moto_node_get_param_value(self, "dir");
    gfloat *orig = (gfloat *)g_value_peek_pointer(vorig);
    gfloat *dir  = (gfloat *)g_value_peek_pointer(vdir);

    MotoPointCloud *in_pc = (MotoPointCloud *)moto_node_get_param_object(self, "in_pc");
    if( ! in_pc)
    {
        if(priv->pc)
        {
            g_object_unref(priv->pc);
        }
        priv->pc = NULL;
        return;
    }
    else
    {
        if(in_pc != priv->prev_pc)
        {
            if(priv->pc)
            {
                g_object_unref(priv->pc);
            }
            priv->pc = MOTO_POINT_CLOUD(moto_copyable_copy(MOTO_COPYABLE(in_pc)));
            moto_mesh_prepare(MOTO_MESH(priv->pc));
        }
        MotoPointCloud *pc = priv->pc;
        priv->prev_pc = in_pc;

        gfloat angle = moto_node_get_param_float(self, "angle");
        //if(angle >= MICRO && fabs(angle - priv->prev_angle) <= MICRO)
        //    return;
        priv->prev_angle = angle;

        if(moto_point_cloud_can_provide_plain_data(pc))
        {
            gfloat *points_i  = NULL;
            gfloat *normals_i = NULL;
            gsize size_i      = 0;
            gfloat *points_o  = NULL;
            gfloat *normals_o = NULL;
            gsize size_o      = 0;

            moto_point_cloud_get_plain_data(in_pc, & points_i, & normals_i, & size_i);
            moto_point_cloud_get_plain_data(pc,    & points_o, & normals_o, & size_o);

            gint i;
            gfloat *pi, *po, *ni, *no;
            if(fabs(angle) >= MICRO)
            {
                gfloat axis[3];
                vector3_copy(axis, dir);
                gfloat lenbuf;
                vector3_normalize(axis, lenbuf);

                gfloat m[16];

                gfloat a, s, c;
                gfloat to_p[3], tmp[3];
                #pragma omp parallel for if(size_i > 100) private(i, pi, po, a, s, c, tmp, to_p) \
                    shared(size_i, angle, points_i, points_o, orig, axis)
                for(i = 0; i < size_i; i++)
                {
                    gint offset = i*3;
                    pi = points_i + offset;
                    po = points_o + offset;

                    vector3_dif(to_p, pi, orig);

                    a = angle * RAD_PER_DEG * vector3_dot(to_p, axis);
                    s = sin(a);
                    c = cos(a);

                    tmp[0] = pi[0] - orig[0];
                    tmp[1] = pi[1] - orig[1];
                    tmp[2] = pi[2] - orig[2];

                    matrix44_rotate_from_axis(m, a, axis[0], axis[1], axis[2]);

                    point3_transform(po, m, tmp);

                    po[0] += orig[0];
                    po[1] += orig[1];
                    po[2] += orig[2];
                }
                if(g_type_is_a(G_TYPE_FROM_INSTANCE(pc), MOTO_TYPE_MESH))
                {
                    moto_mesh_calc_normals(MOTO_MESH(pc));
                }
            }
            else
            {
                for(i = 0; i < size_i; i++)
                {
                    gint offset = i*3;

                    pi = points_i + offset;
                    po = points_o + offset;
                    vector3_copy(po, pi);

                    ni = normals_i + offset;
                    no = normals_o + offset;
                    vector3_copy(no, ni);
                }
            }
        }
    }

    MotoParam *param = moto_node_get_param((MotoNode *)self, "out_pc");
    moto_param_set_object(param, (GObject *)priv->pc);
    moto_param_update_dests(param);
}

#include <math.h>


#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-mesh.h" // FIXME: Temporary!
#include "moto-twist-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static void moto_twist_node_update(MotoNode *self);

/* class NormalMoveNode */

typedef struct _MotoTwistNodePriv MotoTwistNodePriv;
#define MOTO_TWIST_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_TWIST_NODE, MotoTwistNodePriv)

static GObjectClass *normal_move_node_parent_class = NULL;

struct _MotoTwistNodePriv
{
    MotoPointCloud *pc;
    MotoPointCloud *prev_pc;
    gfloat prev_scale;
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
    priv->prev_scale = 0.0f;

    /* params */

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "in_pc",  "Input Point Cloud",  MOTO_TYPE_POINT_CLOUD, MOTO_PARAM_MODE_IN, NULL, pspec, "Geometry", "Geometry",
            "out_pc", "Output Point Cloud", MOTO_TYPE_POINT_CLOUD, MOTO_PARAM_MODE_OUT, NULL, pspec, "Geometry", "Geometry",
            "scale",  "Scaling Value",        G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.1f, pspec, "Geometry", "Geometry",
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

/* methods of class NormalMoveNode */

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

        gfloat scale = moto_node_get_param_float(self, "scale");
        if(fabs(scale-priv->prev_scale) <= MICRO)
            return;
        priv->prev_scale = scale;

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

            gfloat axis[3] = {0, 1, 0};
            gfloat m[16];

            gint i;
            gfloat *pi, *po;
            gfloat a, s, c;
            #pragma omp parallel for if(size_i > 100) private(i, pi, po, a, s, c) \
                shared(size_i, scale, points_i, points_o)
            for(i = 0; i < size_i; i++)
            {
                pi = points_i + i*3;
                po = points_o + i*3;

                a = scale*RAD_PER_DEG*pi[1];
                s = sin(a);
                c = cos(a);

                matrix44_identity(m);
                matrix44_rotate_from_axis(m, a, axis[0], axis[1], axis[2]);

                point3_transform(po, m, pi);
            }
            if(g_type_is_a(G_TYPE_FROM_INSTANCE(pc), MOTO_TYPE_MESH))
            {
                moto_mesh_calc_normals(MOTO_MESH(pc));
            }
        }
    }

    MotoParam *param = moto_node_get_param((MotoNode *)self, "out_pc");
    g_value_set_object(moto_param_get_value(param), priv->pc);
    moto_param_update_dests(param);
}

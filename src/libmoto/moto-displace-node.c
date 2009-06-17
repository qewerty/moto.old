#include <math.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-mesh.h" // FIXME: Temporary!
#include "moto-displace-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static void moto_displace_node_update(MotoNode *self);

/* class MotoDisplaceNode */

typedef struct _MotoDisplaceNodePriv MotoDisplaceNodePriv;
#define MOTO_DISPLACE_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_DISPLACE_NODE, MotoDisplaceNodePriv)

static GObjectClass *normal_move_node_parent_class = NULL;

struct _MotoDisplaceNodePriv
{
    MotoPointCloud *pc;
    MotoPointCloud *prev_pc;
    gfloat prev_scale;
};

static void
moto_displace_node_dispose(GObject *obj)
{
    normal_move_node_parent_class->dispose(obj);
}

static void
moto_displace_node_finalize(GObject *obj)
{
    MotoDisplaceNodePriv *priv = MOTO_DISPLACE_NODE_GET_PRIVATE(obj);

    if(priv->pc);
        g_object_unref(priv->pc);

    normal_move_node_parent_class->finalize(obj);
}

static void
moto_displace_node_init(MotoDisplaceNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoDisplaceNodePriv *priv = MOTO_DISPLACE_NODE_GET_PRIVATE(self);

    priv->pc = NULL;
    priv->prev_pc = NULL;
    priv->prev_scale = 0.0f;

    /* params */

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "in_pc",  "Input Point Cloud",  MOTO_TYPE_POINT_CLOUD, MOTO_PARAM_MODE_IN, NULL, pspec, "Geometry",
            "out_pc", "Output Point Cloud", MOTO_TYPE_POINT_CLOUD, MOTO_PARAM_MODE_OUT, NULL, pspec, "Geometry",
            "scale",  "Scaling Value",        G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.1f, pspec, "Geometry",
            NULL);
}

static void
moto_displace_node_class_init(MotoDisplaceNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoDisplaceNodePriv));

    normal_move_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    goclass->dispose    = moto_displace_node_dispose;
    goclass->finalize   = moto_displace_node_finalize;

    nclass->update = moto_displace_node_update;
}

G_DEFINE_TYPE(MotoDisplaceNode, moto_displace_node, MOTO_TYPE_NODE);

/* Methods of class MotoDisplaceNode */

MotoDisplaceNode *moto_displace_node_new(const gchar *name)
{
    MotoDisplaceNode *self = (MotoDisplaceNode *)g_object_new(MOTO_TYPE_DISPLACE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void moto_displace_node_update(MotoNode *self)
{
    MotoDisplaceNodePriv *priv = MOTO_DISPLACE_NODE_GET_PRIVATE(self);

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

            gint i;
            gfloat *pi, *ni, *po;
            #pragma omp parallel for if(size_i > 400) private(i, pi, ni, po) \
                shared(size_i, scale, points_i, normals_i, points_o)
            for(i = 0; i < size_i; i++)
            {
                pi = points_i + i*3;
                ni = normals_i + i*3;
                po = points_o + i*3;

                po[0] = pi[0] + ni[0]*scale;
                po[1] = pi[1] + ni[1]*scale;
                po[2] = pi[2] + ni[2]*scale;
            }
        }
    }

    MotoParam *param = moto_node_get_param((MotoNode *)self, "out_pc");
    g_value_set_object(moto_param_get_value(param), priv->pc);
    moto_param_update_dests(param);
}

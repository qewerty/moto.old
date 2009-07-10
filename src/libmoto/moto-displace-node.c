#include <math.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-displace-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static MotoGeom *moto_displace_node_perform(MotoNode *self, MotoGeom *in);

/* class MotoDisplaceNode */

static GObjectClass *bend_node_parent_class = NULL;

static void
moto_displace_node_init(MotoDisplaceNode *self)
{
    MotoNode *node = (MotoNode *)self;

    /* params */
    MotoParamSpec *scale_spec = moto_param_spec_float_new(0.1, -1000000, 1000000, 0.01, 0.1);
    moto_node_add_params(node,
            "scale",  "Scaling Value", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.1f, scale_spec, "Geometry",
            NULL);
    g_object_unref(scale_spec);
}

static void
moto_displace_node_class_init(MotoDisplaceNodeClass *klass)
{
    bend_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoGeomOpNodeClass *gopclass = (MotoGeomOpNodeClass *)klass;
    gopclass->perform = moto_displace_node_perform;
}

G_DEFINE_TYPE(MotoDisplaceNode, moto_displace_node, MOTO_TYPE_GEOM_OP_NODE);

/* Methods of class MotoDisplaceNode */

MotoDisplaceNode *moto_displace_node_new(const gchar *name)
{
    MotoDisplaceNode *self = (MotoDisplaceNode *)g_object_new(MOTO_TYPE_DISPLACE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static MotoGeom *moto_displace_node_perform(MotoNode *self, MotoGeom *in)
{
    MotoNode *node = (MotoNode*)self;

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(in), MOTO_TYPE_POINT_CLOUD))
        return in;

    MotoPointCloud *in_pc = (MotoPointCloud*)in;
    MotoPointCloud *geom  = MOTO_POINT_CLOUD(moto_copyable_copy(MOTO_COPYABLE(in_pc)));
    MotoGeom *out = (MotoGeom*)geom;

    gfloat scale;
    moto_node_get_param_float(node, "scale", &scale);

    if(moto_point_cloud_can_provide_plain_data(in_pc))
    {
        gfloat *points_i  = NULL;
        gfloat *normals_i = NULL;
        gsize size_i      = 0;
        gfloat *points_o  = NULL;
        gfloat *normals_o = NULL;
        gsize size_o      = 0;

        moto_point_cloud_get_plain_data(in_pc, & points_i, & normals_i, & size_i);
        moto_point_cloud_get_plain_data(geom,  & points_o, & normals_o, & size_o);

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

    moto_geom_prepare(out);
    return out;
}

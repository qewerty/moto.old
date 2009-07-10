#include <math.h>
#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-twist-node.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static MotoGeom *moto_twist_node_perform(MotoGeomOpNode *self, MotoGeom *in);

/* class MotoTwistNode */

#define MOTO_TWIST_NODE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_TWIST_NODE, MotoTwistNodePriv)

static GObjectClass *twist_node_parent_class = NULL;

static void
moto_twist_node_init(MotoTwistNode *self)
{
    MotoNode *node = (MotoNode *)self;

    /* params */

    gfloat orig[3] = {0, 0, 0};
    gfloat dir[3]  = {1, 1, 0};

    MotoParamSpec *angle_spec = moto_param_spec_float_new(0, -1000000, 1000000, 0.1, 1);
    moto_node_add_params(node,
            "angle",  "Angle",     G_TYPE_FLOAT,          MOTO_PARAM_MODE_INOUT, 0.0f, angle_spec, "Arguments",
            "orig",   "Origin",    MOTO_TYPE_FLOAT_3,     MOTO_PARAM_MODE_INOUT, orig, NULL,       "Arguments",
            "dir",    "Direction", MOTO_TYPE_FLOAT_3,     MOTO_PARAM_MODE_INOUT, dir,  NULL,       "Arguments",
            NULL);
    g_object_unref(angle_spec);
}

static void
moto_twist_node_class_init(MotoTwistNodeClass *klass)
{
    twist_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoGeomOpNodeClass *gopclass = (MotoGeomOpNodeClass *)klass;
    gopclass->perform = moto_twist_node_perform;
}

G_DEFINE_TYPE(MotoTwistNode, moto_twist_node, MOTO_TYPE_GEOM_OP_NODE);

/* Methods of class MotoTwistNode */

MotoTwistNode *moto_twist_node_new(const gchar *name)
{
    MotoTwistNode *self = (MotoTwistNode *)g_object_new(MOTO_TYPE_TWIST_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static MotoGeom *moto_twist_node_perform(MotoGeomOpNode *self, MotoGeom *in)
{
    MotoNode *node = (MotoNode*)self;

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(in), MOTO_TYPE_POINT_CLOUD))
        return in;

    // FIXME
    GValue *vorig = moto_node_get_param_value(node, "orig");
    GValue *vdir  = moto_node_get_param_value(node, "dir");
    gfloat *orig = (gfloat *)g_value_peek_pointer(vorig);
    gfloat *dir  = (gfloat *)g_value_peek_pointer(vdir);

    MotoPointCloud *in_pc = (MotoPointCloud*)in;
    MotoPointCloud *geom  = MOTO_POINT_CLOUD(moto_copyable_copy(MOTO_COPYABLE(in_pc)));
    MotoGeom *out         = (MotoGeom*)geom;

    gfloat angle;
    moto_node_get_param_float(node, "angle", &angle);

    if(moto_point_cloud_can_provide_plain_data(in_pc))
    {
        gfloat *points_i  = NULL;
        gfloat *normals_i = NULL;
        gsize size_i      = 0;
        gfloat *points_o  = NULL;
        gfloat *normals_o = NULL;
        gsize size_o      = 0;

        moto_point_cloud_get_plain_data(in_pc,   & points_i, & normals_i, & size_i);
        moto_point_cloud_get_plain_data(geom,    & points_o, & normals_o, & size_o);

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

    moto_geom_prepare(out);
    return out;
}

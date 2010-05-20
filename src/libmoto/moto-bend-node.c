#include <math.h>
#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-bend-node.h"
#include "libmotoutil/xform.h"
#include "libmotoutil/numdef.h"

/* forwards */

static MotoShape *moto_bend_node_perform(MotoOpNode *self, MotoShape *in, gboolean *the_same);

/* class MotoBendNode */

static GObjectClass *bend_node_parent_class = NULL;

static void
moto_bend_node_init(MotoBendNode *self)
{
    MotoNode *node = (MotoNode *)self;

    /* params */

    gfloat orig[3] = {0, 0, 0};
    gfloat dir[3]  = {1, 0, 0};
    MotoParamSpec *angle_spec = moto_param_spec_floatnew(0, -1000000, 1000000, 0.1, 1);
    moto_node_add_params(node,
            "angle",  "Angle",              G_TYPE_FLOAT,          MOTO_PARAM_MODE_INOUT, 0.0f, angle_spec, "Arguments",
            "orig",   "Origin",             MOTO_TYPE_FLOAT3,     MOTO_PARAM_MODE_INOUT, orig, NULL,       "Arguments",
            "dir",    "Direction",          MOTO_TYPE_FLOAT3,     MOTO_PARAM_MODE_INOUT, dir,  NULL,       "Arguments",
            NULL);
    g_object_unref(angle_spec);
}

static void
moto_bend_node_class_init(MotoBendNodeClass *klass)
{
    bend_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoOpNodeClass *gopclass = (MotoOpNodeClass *)klass;
    gopclass->perform = moto_bend_node_perform;
}

G_DEFINE_TYPE(MotoBendNode, moto_bend_node, MOTO_TYPE_OP_NODE);

/* methods of class MotoBendNode */

MotoBendNode *moto_bend_node_new(const gchar *name)
{
    MotoBendNode *self = (MotoBendNode *)g_object_new(MOTO_TYPE_BEND_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static MotoShape *moto_bend_node_perform(MotoOpNode *self, MotoShape *in, gboolean *the_same)
{
    *the_same = TRUE;

    MotoNode *node = (MotoNode*)self;

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(in), MOTO_TYPE_POINTCLOUD))
        return in;

    MotoPointCloud *in_pc = (MotoPointCloud*)in;
    MotoPointCloud *geom = g_object_get_data((GObject*)self, "_prev_geom");
    if(!geom || !moto_shape_is_struct_the_same(geom, in))
    {
        *the_same = FALSE;
        geom = MOTO_POINTCLOUD(moto_copyable_copy(MOTO_COPYABLE(in_pc)));
        g_object_set_data((GObject*)self, "_prev_geom", geom);
    }
    MotoShape *out = (MotoShape*)geom;

    // FIXME
    GValue *vorig = moto_node_get_param_value(node, "orig");
    GValue *vdir  = moto_node_get_param_value(node, "dir");
    gfloat *orig = (gfloat *)g_value_peek_pointer(vorig);
    gfloat *dir  = (gfloat *)g_value_peek_pointer(vdir);

    gfloat angle;
    moto_node_get_param_float(node, "angle", &angle);

    if(moto_pointcloud_can_provide_plain_data(in_pc))
    {
        gfloat *points_i  = NULL;
        gfloat *normals_i = NULL;
        gsize size_i      = 0;
        gfloat *points_o  = NULL;
        gfloat *normals_o = NULL;
        gsize size_o      = 0;

        moto_pointcloud_get_plain_data(in_pc, & points_i, & normals_i, & size_i);
        moto_pointcloud_get_plain_data(geom,  & points_o, & normals_o, & size_o);

        gint i;
        gfloat *pi, *po, *ni, *no;
        if(fabs(angle) >= MICRO)
        {
            gfloat axis[3];
            vector3_copy(axis, dir);
            gfloat lenbuf;
            vector3_normalize(axis, lenbuf);

            gfloat m[16];

            // 2*PI/A = 2*PI*R
            // R*PI = PI/A
            // R = 1/A
            gfloat R = (fabs(angle) >= MICRO) ? 1 / (angle*RAD_PER_DEG): MACRO;

            gfloat a, s, c;
            gfloat to_p[3], tmp[3];
            for(i = 0; i < size_i; i++)
            {
                gint offset = i*3;
                pi = points_i + offset;
                po = points_o + offset;

                vector3_dif(to_p, pi, orig);

                gfloat vec[3] = {0, 1, 0};

                gfloat move = vector3_dot(to_p, vec);

                gfloat P[3] = {pi[0], pi[1], pi[2]};
                point3_move(P, vec, -move);

                a = move/R;
                s = sin(a);
                c = cos(a);

                gfloat z[3] = {0, 0, 1};
                gfloat O[3] = {orig[0], orig[1], orig[2]};
                point3_move(O, z, R);

                tmp[0] = P[0] - O[0];
                tmp[1] = P[1] - O[1];
                tmp[2] = P[2] - O[2];

                matrix44_rotate_from_axis(m, a, axis[0], axis[1], axis[2]);

                point3_transform(po, m, tmp);

                po[0] += O[0];
                po[1] += O[1];
                po[2] += O[2];
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

    moto_shape_prepare(out);
    return out;
}

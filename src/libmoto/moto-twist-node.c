#include <string.h>
#include <math.h>

#ifdef __SSE__
#include <xmmintrin.h>
#endif

#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-twist-node.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

#ifdef __SSE__
#include "libmotoutil/sse_mathfun.h"
#endif

#define MOTO_ALIGNED16 __attribute__((aligned(16)))

/* forwards */

static MotoGeom *moto_twist_node_perform(MotoGeomOpNode *self, MotoGeom *in, gboolean *the_same);

/* class MotoTwistNode */

#define MOTO_TWIST_NODE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_TWIST_NODE, MotoTwistNodePriv)

static GObjectClass *twist_node_parent_class = NULL;

typedef struct _MotoTwistNodePriv
{
    MotoGeom *in;
} MotoTwistNodePriv;

static void
moto_twist_node_init(MotoTwistNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoTwistNodePriv *priv = MOTO_TWIST_NODE_GET_PRIVATE(self);

    priv->in = NULL;

    /* params */

    gfloat orig[3] = {0, 0, 0};
    gfloat dir[3]  = {0, 1, 0};

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
    g_type_class_add_private(klass, sizeof(MotoTwistNodePriv));

    twist_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoGeomOpNodeClass *gopclass = (MotoGeomOpNodeClass *)klass;
    gopclass->perform   = moto_twist_node_perform;
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

static MotoGeom *moto_twist_node_perform(MotoGeomOpNode *self, MotoGeom *in, gboolean *the_same)
{
    *the_same = TRUE;

    MotoNode *node = (MotoNode*)self;
    MotoTwistNodePriv *priv = MOTO_TWIST_NODE_GET_PRIVATE(self);

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(in), MOTO_TYPE_POINT_CLOUD))
        return in;

    MotoPointCloud *in_pc = (MotoPointCloud*)in;
    MotoPointCloud *geom = g_object_get_data((GObject*)self, "_prev_geom");
    if(!geom || !moto_geom_is_struct_the_same(geom, in))
    {
        *the_same = FALSE;
        geom = MOTO_POINT_CLOUD(moto_copyable_copy(MOTO_COPYABLE(in_pc)));
        g_object_set_data((GObject*)self, "_prev_geom", geom);
    }
    MotoGeom *out = (MotoGeom*)geom;

    float tmp;

    // FIXME
    GValue *vorig = moto_node_get_param_value(node, "orig");
    GValue *vdir  = moto_node_get_param_value(node, "dir");

    gfloat orig[4] MOTO_ALIGNED16;
    memcpy(orig, (gfloat *)g_value_peek_pointer(vorig), 16);
    orig[3] = 0;

    gfloat dir[4]  MOTO_ALIGNED16;
    memcpy(dir,  (gfloat *)g_value_peek_pointer(vdir), 16);
    dir[3] = 0;
    vector3_normalize(dir, tmp);

    gfloat angle[4] MOTO_ALIGNED16;
    moto_node_get_param_float(node, "angle", angle);
    angle[0] *= RAD_PER_DEG;
    angle[1]  = angle[0];
    angle[2]  = angle[0];
    angle[3]  = angle[0];

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
        if(fabs(angle[0]) >= MICRO)
        {
            pi = points_i;
            po = points_o;

#ifdef __SSE__
            // Computing four points at once.
            gsize size_sse = size_i/4;

            float tmp0[16] MOTO_ALIGNED16;
            float tmp1[16] MOTO_ALIGNED16;
            float dots[16] MOTO_ALIGNED16;
            float a[4] MOTO_ALIGNED16;
            float s[4] MOTO_ALIGNED16;
            float c[4] MOTO_ALIGNED16;

            gfloat m0[16] MOTO_ALIGNED16;
            gfloat m1[16] MOTO_ALIGNED16;
            gfloat m2[16] MOTO_ALIGNED16;
            gfloat m3[16] MOTO_ALIGNED16;

            _mm_prefetch(points_i, _MM_HINT_T0);
            for(i = 0; i < size_sse; ++i)
            {

                asm(
                    "movaps (%3), %%xmm6\n\t"
                    "movaps (%4), %%xmm7\n\t"
                    "movaps   (%0), %%xmm0\n\t"
                    "movaps 16(%0), %%xmm1\n\t"
                    "movaps 32(%0), %%xmm2\n\t"
                    "movaps 48(%0), %%xmm3\n\t"
                    "subps %%xmm6, %%xmm0\n\t"
                    "subps %%xmm6, %%xmm1\n\t"
                    "subps %%xmm6, %%xmm2\n\t"
                    "subps %%xmm6, %%xmm3\n\t"
                    "movaps %%xmm0,   (%1)\n\t" // Store vector to point.
                    "movaps %%xmm1, 16(%1)\n\t"
                    "movaps %%xmm2, 32(%1)\n\t"
                    "movaps %%xmm3, 48(%1)\n\t"
                    "mulps %%xmm7, %%xmm0\n\t" // Products for dots.
                    "mulps %%xmm7, %%xmm1\n\t"
                    "mulps %%xmm7, %%xmm2\n\t"
                    "mulps %%xmm7, %%xmm3\n\t"
                    "movaps %%xmm0,   (%2)\n\t"
                    "movaps %%xmm1, 16(%2)\n\t"
                    "movaps %%xmm2, 32(%2)\n\t"
                    "movaps %%xmm3, 48(%2)\n\t"
                    :
                    : "r" (pi), "r" (tmp0), "r" (tmp1), "r" (orig), "r" (dir)
                    : "xmm0", "xmm1", "xmm2", "xmm3", "xmm6", "xmm7");

                // vector3_dif(tmp, pi, orig);
                dots[0] = tmp1[0]  + tmp1[1]  + tmp1[2];  // 3
                dots[1] = tmp1[4]  + tmp1[5]  + tmp1[6];  // 7
                dots[2] = tmp1[8]  + tmp1[9]  + tmp1[10]; // 11
                dots[3] = tmp1[12] + tmp1[13] + tmp1[14]; // 15

                // a = angle * dot;
                asm("movaps (%0), %%xmm0\n\t"
                    "mulps  (%1), %%xmm0\n\t"
                    "movaps %%xmm0, (%2)\n\t"
                    :
                    : "r" (angle), "r" (dots), "r" (a)
                    : "xmm0");


                __m128 aa = _mm_load_ps(a);
                __m128 ss, cc;
                sincos_ps(aa, &ss, &cc);
                _mm_store_ps(s, ss);
                _mm_store_ps(c, cc);

                matrix44_rotate_from_axis_sincos(m0, s[0], c[0], dir[0], dir[1], dir[2]);
                matrix44_rotate_from_axis_sincos(m1, s[1], c[1], dir[0], dir[1], dir[2]);
                matrix44_rotate_from_axis_sincos(m2, s[2], c[2], dir[0], dir[1], dir[2]);
                matrix44_rotate_from_axis_sincos(m3, s[3], c[3], dir[0], dir[1], dir[2]);
                point3_transform(po,    m0, tmp0);
                point3_transform(po+4,  m1, tmp0+4);
                point3_transform(po+8,  m2, tmp0+8);
                point3_transform(po+12, m3, tmp0+12);

                asm("movaps   (%0), %%xmm0\n\t"
                    "movaps 16(%0), %%xmm1\n\t"
                    "movaps 32(%0), %%xmm2\n\t"
                    "movaps 48(%0), %%xmm3\n\t"
                    "movaps (%1), %%xmm4\n\t"
                    "addps  %%xmm4, %%xmm0\n\t"
                    "addps  %%xmm4, %%xmm1\n\t"
                    "addps  %%xmm4, %%xmm2\n\t"
                    "addps  %%xmm4, %%xmm3\n\t"
                    "movaps %%xmm0,   (%0)\n\t"
                    "movaps %%xmm1, 16(%0)\n\t"
                    "movaps %%xmm2, 32(%0)\n\t"
                    "movaps %%xmm3, 48(%0)\n\t"
                    :
                    : "r" (po), "r" (orig)
                    : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");

                pi += 16;
                po += 16;
            }

            // Finish work.
            float to_p[3];
            for(i = 0; i < (size_i - size_sse*4); ++i)
            {
                vector3_dif(to_p, pi, orig);

                float a = angle[0] * vector3_dot(to_p, dir);
                float s = sin(a);
                float c = cos(a);

                matrix44_rotate_from_axis_sincos(m0, s, c, dir[0], dir[1], dir[2]);
                point3_transform(po, m0, to_p);

                po[0] += orig[0];
                po[1] += orig[1];
                po[2] += orig[2];

                pi += 4;
                po += 4;
            }
#else
            float m[16];
            float to_p[3];
            for(i = 0; i < size_i; ++i)
            {
                vector3_dif(to_p, pi, orig);

                float a = angle[0] * vector3_dot(to_p, dir);
                float s = sin(a);
                float c = cos(a);

                matrix44_rotate_from_axis_sincos(m, s, c, dir[0], dir[1], dir[2]);
                point3_transform(po, m, to_p);

                po[0] += orig[0];
                po[1] += orig[1];
                po[2] += orig[2];

                pi += 4;
                po += 4;
            }
#endif
        }
        else
        {
            memcpy(points_o, points_i, size_i*3*sizeof(float));
            memcpy(normals_o, normals_i, size_i*3*sizeof(float));
        }
    }

    moto_geom_prepare(out);
    return out;
}

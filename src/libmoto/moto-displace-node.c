#include <math.h>

#ifdef __SSE__
#include <xmmintrin.h>
#endif

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

static MotoGeom *moto_displace_node_perform(MotoNode *self, MotoGeom *in, gboolean *the_same);

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

static MotoGeom *moto_displace_node_perform(MotoNode *self, MotoGeom *in, gboolean *the_same)
{
    *the_same = TRUE;

    MotoNode *node = (MotoNode*)self;

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
#ifndef __SSE__
        for(i = 0; i < size_i; ++i)
        {
            gint ii = i*3;
            pi = points_i + ii;
            ni = normals_i + ii;
            po = points_o + ii;
            po[0] = pi[0] + ni[0]*scale;
            po[1] = pi[1] + ni[1]*scale;
            po[2] = pi[2] + ni[2]*scale;
        }
#else
        float scale_sse[4] __attribute__((aligned(16))) = {scale, scale, scale, scale};
        gsize size_sse = size_i*4/16;

        pi = points_i;
        ni = normals_i;
        po = points_o;

        asm("movaps (%0), %%xmm0\n\t" : : "r" (scale_sse) : "xmm0");

        _mm_prefetch(points_i, _MM_HINT_T0);
        _mm_prefetch(normals_i, _MM_HINT_T0);
        for(i = 0; i < size_sse; ++i)
        {
            asm("movaps   (%0), %%xmm1\n\t"
                "movaps 16(%0), %%xmm2\n\t"
                "movaps 32(%0), %%xmm3\n\t"
                "movaps 48(%0), %%xmm4\n\t"
                "mulps  %%xmm0, %%xmm1\n\t" // xmm0 loaded before cycle.
                "mulps  %%xmm0, %%xmm2\n\t"
                "mulps  %%xmm0, %%xmm3\n\t"
                "mulps  %%xmm0, %%xmm4\n\t"
                "addps    (%1), %%xmm1\n\t"
                "addps  16(%1), %%xmm2\n\t"
                "addps  32(%1), %%xmm3\n\t"
                "addps  48(%1), %%xmm4\n\t"
                "movaps %%xmm1, (%2)\n\t"
                "movaps %%xmm2, 16(%2)\n\t"
                "movaps %%xmm3, 32(%2)\n\t"
                "movaps %%xmm4, 48(%2)\n\t"
                : /* Nothing. */
                : "r" (ni), "r" (pi), "r" (po)
                : "xmm1", "xmm2", "xmm3", "xmm4"
                );

            pi += 16;
            ni += 16;
            po += 16;
        }

        for(i = 0; i < (size_i*4 - size_sse*16); ++i)
        {
            po[i] = pi[i]+ ni[i]*scale;
        }
#endif
    }

    moto_geom_prepare(out);
    return out;
}

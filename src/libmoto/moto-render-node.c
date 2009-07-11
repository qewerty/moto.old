#include <math.h>
#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-render-node.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static gboolean moto_render_node_render_default(MotoRenderNode *self);

/* class MotoRenderNode */

static GObjectClass *render_node_parent_class = NULL;

static void
moto_render_node_init(MotoRenderNode *self)
{
    MotoNode *node = (MotoNode *)self;

    /* params */

    /*
    moto_node_add_params(node,
            "angle",  "Angle",     G_TYPE_FLOAT,      MOTO_PARAM_MODE_INOUT, 0.0f, angle_spec, "Arguments",
            "orig",   "Origin",    MOTO_TYPE_FLOAT_3, MOTO_PARAM_MODE_INOUT, orig, NULL,       "Arguments",
            "dir",    "Direction", MOTO_TYPE_FLOAT_3, MOTO_PARAM_MODE_INOUT, dir,  NULL,       "Arguments",
            NULL);
    */
}

static void
moto_render_node_class_init(MotoRenderNodeClass *klass)
{
    render_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    klass->render = moto_render_node_render_default;
}

G_DEFINE_ABSTRACT_TYPE(MotoRenderNode, moto_render_node, MOTO_TYPE_NODE);

/* methods of class MotoRenderNode */

gboolean moto_render_node_render(MotoRenderNode *self)
{
    MotoRenderNodeClass *klass = MOTO_RENDER_NODE_GET_CLASS(self);

    if(klass->render)
        return klass->render(self);

    return FALSE;
}

static gboolean moto_render_node_render_default(MotoRenderNode *self)
{}


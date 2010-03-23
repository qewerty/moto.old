#include <math.h>
#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-render-node.h"
#include "libmotoutil/xform.h"
#include "libmotoutil/numdef.h"

/* forwards */


static gboolean moto_render_node_render_default(MotoRenderNode *self);

/* class MotoRenderNode */

static GObjectClass *render_node_parent_class = NULL;

typedef struct _MotoRenderNodePriv MotoRenderNodePriv;

#define MOTO_RENDER_NODE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_RENDER_NODE, MotoRenderNodePriv)

struct _MotoRenderNodePriv
{
    gboolean rendered;
    GTimeVal last_render;
};

static void
moto_render_node_init(MotoRenderNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoRenderNodePriv *priv = MOTO_RENDER_NODE_GET_PRIVATE(self);

    priv->rendered = FALSE;
    g_get_current_time( & priv->last_render);

    /* params */

    moto_node_add_params(node,
        "variation", "Variation", MOTO_TYPE_STRING, MOTO_PARAM_MODE_INOUT, "", NULL, "Arguments",
        "skip_untouched",  "Skip Untouched", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Optimization",
        NULL);
}

/*
static MotoNodeActionResult *render(MotoRenderNode *node)
{
    moto_render_node_render(node);
}
*/

static void
moto_render_node_class_init(MotoRenderNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoRenderNodePriv));

    render_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    klass->render = moto_render_node_render_default;

    // TODO: moto_node_class_add_action(moto_node_action_new("render", "Render", (MotoNodeActionFunc)render));
}

G_DEFINE_ABSTRACT_TYPE(MotoRenderNode, moto_render_node, MOTO_TYPE_NODE);

/* methods of class MotoRenderNode */

gboolean moto_render_node_render(MotoRenderNode *self)
{
    MotoRenderNodeClass *klass = MOTO_RENDER_NODE_GET_CLASS(self);
    MotoRenderNodePriv  *priv  = MOTO_RENDER_NODE_GET_PRIVATE(self);

    gboolean result = FALSE;
    if(klass->render)
        result = klass->render(self);

    priv->rendered = TRUE;
    g_get_current_time( & priv->last_render);
    return result;
}

static gboolean moto_render_node_render_default(MotoRenderNode *self)
{
    return FALSE;
}

gboolean moto_render_node_check_time(MotoRenderNode *self, const GTimeVal *tv)
{
    gboolean skip_not_modified;
    moto_node_get_param_boolean((MotoNode*)self, "skip_not_modified", &skip_not_modified);
    if(!skip_not_modified)
        return TRUE;

    MotoRenderNodePriv *priv = MOTO_RENDER_NODE_GET_PRIVATE(self);
    if(!priv->rendered)
        return FALSE;
    return (tv->tv_sec > priv->last_render.tv_sec);
}


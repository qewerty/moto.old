#include <math.h>
#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-rman-node.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static gboolean moto_rman_node_render(MotoRenderNode *self);

/* class MotoRManNode */

static GObjectClass *rman_node_parent_class = NULL;

static void
moto_rman_node_init(MotoRManNode *self)
{
    MotoNode *node = (MotoNode *)self;

    /* params */

    moto_node_add_params(node,
            "variation", "Variation", G_TYPE_STRING, MOTO_PARAM_MODE_INOUT, "", NULL, "Arguments",
            "command",   "Command",   G_TYPE_STRING, MOTO_PARAM_MODE_INOUT, "", NULL, "Arguments",
            NULL);
}

static void
moto_rman_node_class_init(MotoRManNodeClass *klass)
{
    rman_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoRenderNodeClass *rclass = (MotoRenderNodeClass*)klass;
    rclass->render = moto_rman_node_render;
}

G_DEFINE_TYPE(MotoRManNode, moto_rman_node, MOTO_TYPE_RENDER_NODE);

/* methods of class MotoRManNode */

MotoRManNode *moto_rman_node_new(const gchar *name)
{
    MotoRManNode *self = (MotoRManNode *)g_object_new(MOTO_TYPE_RMAN_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static gboolean moto_rman_node_render(MotoRenderNode *self)
{
    return FALSE;
}

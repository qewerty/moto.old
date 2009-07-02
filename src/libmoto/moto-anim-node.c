#include "libmotoutil/moto-gl.h"

#include "moto-anim-node.h"

/* class AnimNode */

typedef struct _MotoAnimNodePriv MotoAnimNodePriv;

#define MOTO_ANIM_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_ANIM_NODE, MotoAnimNodePriv)

static GObjectClass *anim_node_parent_class = NULL;

struct _MotoAnimNodePriv
{
    gboolean prepared;
};

/*
static void
moto_anim_node_finalize(GObject *obj)
{
    anim_node_parent_class->finalize(obj);
}
*/

static void
moto_anim_node_init(MotoAnimNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoAnimNodePriv *priv = MOTO_ANIM_NODE_GET_PRIVATE(self);

    moto_node_add_params(node,
            "time", "Time", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, NULL, "Time",
            NULL);

    priv->prepared = FALSE;
}

static void
moto_anim_node_class_init(MotoAnimNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    anim_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    // goclass->finalize = moto_anim_node_finalize;

    g_type_class_add_private(klass, sizeof(MotoAnimNodePriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoAnimNode, moto_anim_node, MOTO_TYPE_NODE);

gint moto_anim_node_insert_animator(MotoAnimNode *self, gint index, gfloat width)
{}

gint moto_anim_node_append_animator(MotoAnimNode *self, gfloat width)
{
    return moto_anim_node_insert_animator(self, -1, width);
}

gint moto_anim_node_prepend_animator(MotoAnimNode *self, gfloat width)
{
    return moto_anim_node_insert_animator(self, 0, width);
}

void moto_anim_node_delete_animator(MotoAnimNode *self, guint index)
{}

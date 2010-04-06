#include <math.h>
#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-geom.h"
#include "moto-op-node.h"

/* forwards */

static void moto_op_node_update(MotoNode *self);

/* class MotoOpNode */

typedef struct _MotoOpNodePriv MotoOpNodePriv;

#define MOTO_OP_NODE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_OP_NODE, MotoOpNodePriv)

static GObjectClass *op_node_parent_class = NULL;

struct _MotoOpNodePriv
{
    MotoMeshSelection *selection;
};

static void
moto_op_node_dispose(GObject *obj)
{
    MotoOpNodePriv *priv = MOTO_OP_NODE_GET_PRIVATE(obj);

    if(priv->selection)
        moto_mesh_selection_free(priv->selection);

    op_node_parent_class->dispose(obj);
}

static void
moto_op_node_finalize(GObject *obj)
{
    op_node_parent_class->finalize(obj);
}

static void
moto_op_node_init(MotoOpNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoOpNodePriv *priv = MOTO_OP_NODE_GET_PRIVATE(self);

    priv->selection = NULL;

    moto_node_add_params(node,
            "in", "Input Shape", MOTO_TYPE_GEOM, MOTO_PARAM_MODE_IN, NULL, NULL, "Shape",
            NULL);
}

static void
moto_op_node_class_init(MotoOpNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoOpNodePriv));

    op_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    goclass->dispose    = moto_op_node_dispose;
    goclass->finalize   = moto_op_node_finalize;

    klass->perform   = NULL;

    nclass->update = moto_op_node_update;

    // TODO: moto_node_class_add_action(moto_node_action_new("reselect", "Reselect"));
}

G_DEFINE_ABSTRACT_TYPE(MotoOpNode, moto_op_node, MOTO_TYPE_SHAPE_NODE);

/* Methods of class MotoOpNode */

MotoOpNode *moto_op_node_new(const gchar *name)
{
    MotoOpNode *self = (MotoOpNode *)g_object_new(MOTO_TYPE_OP_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

void moto_op_node_set_selection(MotoOpNode *self,
    MotoMeshSelection *selection)
{
    MotoOpNodePriv *priv = MOTO_OP_NODE_GET_PRIVATE(self);

    if(priv->selection)
        moto_mesh_selection_free(priv->selection);

    priv->selection = moto_mesh_selection_copy(selection);
}

MotoMeshSelection *moto_op_node_get_selection(MotoOpNode *self)
{
    return MOTO_OP_NODE_GET_PRIVATE(self)->selection;
}

static void moto_op_node_update(MotoNode *self)
{
    MotoOpNodePriv *priv = MOTO_OP_NODE_GET_PRIVATE(self);

    MotoGeom *in;
    MotoGeom *old_geom;
    moto_node_get_param_object(self, "in", (GObject**)&in);
    moto_node_get_param_object((MotoNode *)self, "out", (GObject**)&old_geom);
    if( ! in)
    {
        if(old_geom)
            g_object_unref(old_geom);
        moto_node_set_param_object((MotoNode *)self, "out", NULL);
        return;
    }
    if( ! priv->selection)
    {
        if(old_geom && old_geom != in)
        {
            g_object_unref(old_geom);
            moto_node_set_param_object(self, "out", (GObject*)in);
        }
        return;
    }

    MotoGeom *geom = in;

    gboolean the_same = FALSE;
    gboolean active;
    moto_node_get_param_boolean(self, "active", &active);
    if(active)
    {
        geom = moto_op_node_perform((MotoOpNode*)self, in, &the_same);
        if(!the_same && old_geom && (old_geom != in))
            g_object_unref(old_geom);
    }

    moto_node_set_param_object(self, "out", (GObject *)geom);
}

MotoGeom *moto_op_node_perform(MotoOpNode *self, MotoGeom *in, gboolean *the_same)
{
    MotoOpNodeClass *klass = MOTO_OP_NODE_GET_CLASS(self);

    if(klass->perform)
        return klass->perform(self, in, the_same);

    return NULL;
}

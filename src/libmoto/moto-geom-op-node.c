#include <math.h>
#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-geom.h"
#include "moto-geom-op-node.h"

/* forwards */

static void moto_geom_op_node_update(MotoNode *self);

/* class MotoGeomOpNode */

typedef struct _MotoGeomOpNodePriv MotoGeomOpNodePriv;

#define MOTO_GEOM_OP_NODE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_GEOM_OP_NODE, MotoGeomOpNodePriv)

static GObjectClass *geom_op_node_parent_class = NULL;

struct _MotoGeomOpNodePriv
{
    MotoMeshSelection *selection;
};

static void
moto_geom_op_node_dispose(GObject *obj)
{
    MotoGeomOpNodePriv *priv = MOTO_GEOM_OP_NODE_GET_PRIVATE(obj);

    if(priv->selection)
        moto_mesh_selection_free(priv->selection);

    geom_op_node_parent_class->dispose(obj);
}

static void
moto_geom_op_node_finalize(GObject *obj)
{
    geom_op_node_parent_class->finalize(obj);
}

static void
moto_geom_op_node_init(MotoGeomOpNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoGeomOpNodePriv *priv = MOTO_GEOM_OP_NODE_GET_PRIVATE(self);

    priv->selection = NULL;

    moto_node_add_params(node,
            "active", "Active",          MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "in",     "Input Geometry",  MOTO_TYPE_GEOM,    MOTO_PARAM_MODE_IN,    NULL, NULL, "Geometry",
            "out",    "Output Geometry", MOTO_TYPE_GEOM,    MOTO_PARAM_MODE_OUT,   NULL, NULL, "Geometry",
            NULL);
}

static void
moto_geom_op_node_class_init(MotoGeomOpNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoGeomOpNodePriv));

    geom_op_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    goclass->dispose    = moto_geom_op_node_dispose;
    goclass->finalize   = moto_geom_op_node_finalize;

    klass->perform   = NULL;

    nclass->update = moto_geom_op_node_update;

    // TODO: moto_node_class_add_action(moto_node_action_new("reselect", "Reselect"));
}

G_DEFINE_ABSTRACT_TYPE(MotoGeomOpNode, moto_geom_op_node, MOTO_TYPE_NODE);

/* Methods of class MotoGeomOpNode */

MotoGeomOpNode *moto_geom_op_node_new(const gchar *name)
{
    MotoGeomOpNode *self = (MotoGeomOpNode *)g_object_new(MOTO_TYPE_GEOM_OP_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

void moto_geom_op_node_set_selection(MotoGeomOpNode *self,
    MotoMeshSelection *selection)
{
    MotoGeomOpNodePriv *priv = MOTO_GEOM_OP_NODE_GET_PRIVATE(self);

    if(priv->selection)
        moto_mesh_selection_free(priv->selection);

    priv->selection = moto_mesh_selection_copy(selection);
}

MotoMeshSelection *moto_geom_op_node_get_selection(MotoGeomOpNode *self)
{
    return MOTO_GEOM_OP_NODE_GET_PRIVATE(self)->selection;
}

static void moto_geom_op_node_update(MotoNode *self)
{
    MotoGeomOpNodePriv *priv = MOTO_GEOM_OP_NODE_GET_PRIVATE(self);

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
        geom = moto_geom_op_node_perform((MotoGeomOpNode*)self, in, &the_same);
        if(!the_same && old_geom && (old_geom != in))
            g_object_unref(old_geom);
    }

    moto_node_set_param_object(self, "out", (GObject *)geom);
}

MotoGeom *moto_geom_op_node_perform(MotoGeomOpNode *self, MotoGeom *in, gboolean *the_same)
{
    MotoGeomOpNodeClass *klass = MOTO_GEOM_OP_NODE_GET_CLASS(self);

    if(klass->perform)
        return klass->perform(self, in, the_same);

    return NULL;
}

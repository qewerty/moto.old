#include <math.h>

#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-mesh.h"
#include "moto-remove-faces-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static void moto_remove_faces_node_update(MotoNode *self);

/* class MotoRemoveFacesNode */

typedef struct _MotoRemoveFacesNodePriv MotoRemoveFacesNodePriv;
#define MOTO_REMOVE_FACES_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_REMOVE_FACES_NODE, MotoRemoveFacesNodePriv)

static GObjectClass *normal_move_node_parent_class = NULL;

struct _MotoRemoveFacesNodePriv
{
    MotoMeshSelection *selection;
};

static void
moto_remove_faces_node_dispose(GObject *obj)
{
    MotoRemoveFacesNodePriv *priv = MOTO_REMOVE_FACES_NODE_GET_PRIVATE(obj);

    if(priv->selection)
        moto_mesh_selection_free(priv->selection);

    normal_move_node_parent_class->dispose(obj);
}

static void
moto_remove_faces_node_finalize(GObject *obj)
{
    normal_move_node_parent_class->finalize(obj);
}

static void
moto_remove_faces_node_init(MotoRemoveFacesNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoRemoveFacesNodePriv *priv = MOTO_REMOVE_FACES_NODE_GET_PRIVATE(self);

    priv->selection = NULL;

    /* params */

    moto_node_add_params(node,
            "in_mesh",  "Input Mesh",  MOTO_TYPE_MESH, MOTO_PARAM_MODE_IN,    NULL, NULL, "Geometry",
            "out_mesh", "Output Mesh", MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT,   NULL, NULL, "Geometry",
            NULL);
}

static void
moto_remove_faces_node_class_init(MotoRemoveFacesNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoRemoveFacesNodePriv));

    normal_move_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    goclass->dispose    = moto_remove_faces_node_dispose;
    goclass->finalize   = moto_remove_faces_node_finalize;

    nclass->update = moto_remove_faces_node_update;
}

G_DEFINE_TYPE(MotoRemoveFacesNode, moto_remove_faces_node, MOTO_TYPE_NODE);

/* Methods of class MotoRemoveFacesNode */

MotoRemoveFacesNode *moto_remove_faces_node_new(const gchar *name)
{
    MotoRemoveFacesNode *self = (MotoRemoveFacesNode *)g_object_new(MOTO_TYPE_REMOVE_FACES_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

void moto_remove_faces_node_set_mesh_selection(MotoRemoveFacesNode *self,
    MotoMeshSelection *selection)
{
    MotoRemoveFacesNodePriv *priv = MOTO_REMOVE_FACES_NODE_GET_PRIVATE(self);
    priv->selection = moto_mesh_selection_copy(selection);
}

static void moto_remove_faces_node_update(MotoNode *self)
{
    MotoRemoveFacesNodePriv *priv = MOTO_REMOVE_FACES_NODE_GET_PRIVATE(self);

    MotoMesh *old_mesh;
    moto_node_get_param_object((MotoNode *)self, "out_mesh", (GObject**)&old_mesh);
    if(old_mesh)
        g_object_unref(old_mesh);

    MotoMesh *in_mesh;
    moto_node_get_param_object(self, "in_mesh", (GObject**)&in_mesh);
    if( ! in_mesh)
    {
        moto_node_set_param_object(self, "out_mesh", NULL);
        MotoParam *param = moto_node_get_param((MotoNode *)self, "out_mesh");
        moto_param_update_dests(param);
        return;
    }

    if( ! priv->selection)
    {
        MotoMesh *mesh = moto_mesh_new_copy(in_mesh);
        moto_node_set_param_object(self, "out_mesh", (GObject*)mesh);
        MotoParam *param = moto_node_get_param((MotoNode *)self, "out_mesh");
        moto_param_update_dests(param);
        return;
    }

    MotoMesh *mesh = moto_mesh_remove_faces(in_mesh, priv->selection);

    MotoParam *param = moto_node_get_param((MotoNode *)self, "out_mesh");
    moto_param_set_object(param, (GObject *)mesh);
    moto_param_update_dests(param);
}


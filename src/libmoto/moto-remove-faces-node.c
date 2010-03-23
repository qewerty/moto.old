#include <math.h>

#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-mesh.h"
#include "moto-remove-faces-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/xform.h"
#include "libmotoutil/numdef.h"

/* forwards */

static MotoMesh *moto_remove_faces_node_perform(MotoGeomOpNode *self, MotoGeom *in, gboolean *the_same);

/* class MotoRemoveFacesNode */

static GObjectClass *remove_faces_node_parent_class = NULL;

static void
moto_remove_faces_node_init(MotoRemoveFacesNode *self)
{}

static void
moto_remove_faces_node_class_init(MotoRemoveFacesNodeClass *klass)
{
    remove_faces_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoGeomOpNodeClass *moclass = (MotoGeomOpNodeClass *)klass;

    moclass->perform = moto_remove_faces_node_perform;
}

G_DEFINE_TYPE(MotoRemoveFacesNode, moto_remove_faces_node, MOTO_TYPE_GEOM_OP_NODE);

/* Methods of class MotoRemoveFacesNode */

MotoRemoveFacesNode *moto_remove_faces_node_new(const gchar *name)
{
    MotoRemoveFacesNode *self = (MotoRemoveFacesNode *)g_object_new(MOTO_TYPE_REMOVE_FACES_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static MotoMesh *moto_remove_faces_node_perform(MotoGeomOpNode *self, MotoGeom *in, gboolean *the_same)
{
    *the_same = FALSE;

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(in), MOTO_TYPE_MESH))
        return in;

    MotoMesh *in_mesh = (MotoMesh*)in;

    return moto_mesh_remove_faces(in_mesh, moto_geom_op_node_get_selection(self));
}

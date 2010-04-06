#include <math.h>

#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-mesh.h"
#include "moto-remove-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/xform.h"
#include "libmotoutil/numdef.h"

/* forwards */

static MotoMesh *moto_remove_node_perform(MotoOpNode *self, MotoGeom *in, gboolean *the_same);

/* class MotoRemoveNode */

static GObjectClass *remove_node_parent_class = NULL;

static void
moto_remove_node_init(MotoRemoveNode *self)
{}

static void
moto_remove_node_class_init(MotoRemoveNodeClass *klass)
{
    remove_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoOpNodeClass *moclass = (MotoOpNodeClass *)klass;

    moclass->perform = moto_remove_node_perform;
}

G_DEFINE_TYPE(MotoRemoveNode, moto_remove_node, MOTO_TYPE_OP_NODE);

/* Methods of class MotoRemoveNode */

MotoRemoveNode *moto_remove_node_new(const gchar *name)
{
    MotoRemoveNode *self = (MotoRemoveNode *)g_object_new(MOTO_TYPE_REMOVE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static MotoMesh *moto_remove_node_perform(MotoOpNode *self, MotoGeom *in, gboolean *the_same)
{
    *the_same = FALSE;

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(in), MOTO_TYPE_MESH))
        return in;

    MotoMesh *in_mesh = (MotoMesh*)in;

    return moto_mesh_remove(in_mesh, moto_op_node_get_selection(self));
}

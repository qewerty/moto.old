#include <math.h>

#include "moto-types.h"
#include "moto-enums.h"
#include "moto-param-spec.h"
#include "moto-mesh.h"
#include "moto-object-node.h"
#include "moto-extrude-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static MotoMesh *moto_extrude_node_perform(MotoGeomOpNode *self, MotoGeom *in, gboolean *the_same);

/* class MotoExtrudeNode */

static GObjectClass *extrude_node_parent_class = NULL;

static void
moto_extrude_node_init(MotoExtrudeNode *self)
{
    MotoNode *node = (MotoNode *)self;

    /* params */
    MotoParamSpec *sections_spec = moto_param_spec_int_new(1, 1, 100000, 1, 2);
    moto_node_add_params(node,
            "mode",     "Mode",        MOTO_TYPE_EXTRUDE_MODE, MOTO_PARAM_MODE_INOUT, MOTO_EXTRUDE_MODE_FACES, NULL, "Arguments",
            "sections", "Sections",    MOTO_TYPE_INT,          MOTO_PARAM_MODE_INOUT, 1, sections_spec, "Arguments",
            "length",   "Length",      MOTO_TYPE_FLOAT,        MOTO_PARAM_MODE_INOUT, 0.5f, NULL,       "Arguments",
            "path",     "Path",        MOTO_TYPE_OBJECT_NODE,  MOTO_PARAM_MODE_INOUT, NULL, NULL,       "Path",
            "global",   "Global",      MOTO_TYPE_BOOLEAN,      MOTO_PARAM_MODE_INOUT, TRUE, NULL,       "Path",
            NULL);
    g_object_unref(sections_spec);
}

static void
moto_extrude_node_class_init(MotoExtrudeNodeClass *klass)
{
    extrude_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoGeomOpNodeClass *moclass = (MotoGeomOpNodeClass *)klass;
    moclass->perform = moto_extrude_node_perform;
}

G_DEFINE_TYPE(MotoExtrudeNode, moto_extrude_node, MOTO_TYPE_GEOM_OP_NODE);

/* Methods of class MotoExtrudeNode */

MotoExtrudeNode *moto_extrude_node_new(const gchar *name)
{
    MotoExtrudeNode *self = (MotoExtrudeNode *)g_object_new(MOTO_TYPE_EXTRUDE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static MotoMesh *moto_extrude_node_perform(MotoGeomOpNode *self, MotoGeom *in, gboolean *the_same)
{
    *the_same = FALSE;

    MotoNode *node = (MotoNode *)self;

    if( ! g_type_is_a(G_TYPE_FROM_INSTANCE(in), MOTO_TYPE_MESH))
    {
        return in;
    }

    MotoMesh *in_mesh = (MotoMesh*)in;

    gint sections;
    moto_node_get_param_int(node, "sections", &sections);
    if(sections < 1)
        return in_mesh;

    gfloat length;
    moto_node_get_param_float(node, "length", &length);

    MotoExtrudeMode mode;
    moto_node_get_param_enum(node, "mode", (gint*)&mode);

    MotoMeshSelection *selection = moto_geom_op_node_get_selection(self);

    MotoMesh *mesh = NULL;
    switch(mode)
    {
        case MOTO_EXTRUDE_MODE_VERTS:
            mesh = moto_mesh_extrude_verts(in_mesh, selection, sections, length);
        break;
        case MOTO_EXTRUDE_MODE_EDGES:
            ;// mesh = moto_mesh_extrude_edges(in_mesh, selection, sections, length);
        break;
        case MOTO_EXTRUDE_MODE_FACES:
            mesh = moto_mesh_extrude_faces(in_mesh, selection, sections, length);
        break;
        case MOTO_EXTRUDE_MODE_REGION:
            ; // mesh = moto_mesh_extrude_region(in_mesh, selection, sections, length);
        break;
    }

    return mesh;
}


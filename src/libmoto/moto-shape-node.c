#include <GL/gl.h>

#include "moto-shape-node.h"
#include "moto-shape.h"

void moto_shape_node_draw_DEFUALT(MotoShapeNode* self, MotoShapeSelection* selection,
    MotoDrawMode draw_mode, MotoSelectionMode selection_mode);

/* class ShapeNode */

typedef struct _MotoShapeNodePriv MotoShapeNodePriv;

#define MOTO_SHAPE_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_SHAPE_NODE, MotoShapeNodePriv)

static GObjectClass *shape_node_parent_class = NULL;

struct _MotoShapeNodePriv
{
    gboolean prepared;
};

/*
static void
moto_shape_node_finalize(GObject *obj)
{
    shape_node_parent_class->finalize(obj);
}
*/

static void
moto_shape_node_init(MotoShapeNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoShapeNodePriv *priv = MOTO_SHAPE_NODE_GET_PRIVATE(self);

    moto_node_add_params(node,
            "active", "Active", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "visible", "Visible",   MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "lock", "Lock", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "out", "Output Shape", MOTO_TYPE_GEOM, MOTO_PARAM_MODE_OUT,   NULL, NULL, "Shape",
            NULL);

    priv->prepared = FALSE;
}

static void
moto_shape_node_class_init(MotoShapeNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    shape_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    // goclass->finalize = moto_shape_node_finalize;

    klass->get_bound = NULL;
    klass->draw = moto_shape_node_draw_DEFUALT;

    g_type_class_add_private(klass, sizeof(MotoShapeNodePriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoShapeNode, moto_shape_node, MOTO_TYPE_NODE);

/* Methods of class ShapeNode */

MotoBound *moto_shape_node_get_bound(MotoShapeNode *self)
{
    MotoShapeNodeClass *klass = MOTO_SHAPE_NODE_GET_CLASS(self);

    if(klass->get_bound)
        return klass->get_bound(self);

    return NULL;
}

void moto_shape_node_draw(MotoShapeNode* self, MotoShapeSelection* selection,
    MotoDrawMode draw_mode, MotoSelectionMode selection_mode)
{
    MOTO_SHAPE_NODE_GET_CLASS(self)->draw(self, selection, draw_mode, selection_mode);
}

static void moto_shape_node_draw_BBOX_WIREFRAME_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_WIREFRAME_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_WIREFRAME_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_WIREFRAME_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_SOLID_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_SOLID_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_SOLID_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_SOLID_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_TEX_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_TEX_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_TEX_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_TEX_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_TEX_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_TEX_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_TEX_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_TEX_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_TEX_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_TEX_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_TEX_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_TEX_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SHADED_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SHADED_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SHADED_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SHADED_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

void moto_shape_node_draw_DEFUALT(MotoShapeNode* self, MotoShapeSelection* selection,
    MotoDrawMode draw_mode, MotoSelectionMode selection_mode)
{
    switch(draw_mode)
    {
        case MOTO_DRAW_MODE_BBOX_WIREFRAME:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_BBOX_WIREFRAME_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_BBOX_WIREFRAME_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_BBOX_WIREFRAME_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_BBOX_WIREFRAME_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_BBOX_SOLID:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_BBOX_SOLID_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_BBOX_SOLID_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_BBOX_SOLID_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_BBOX_SOLID_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_WIREFRAME:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_WIREFRAME_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_WIREFRAME_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_WIREFRAME_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_WIREFRAME_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_WIREFRAME_TEX:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_WIREFRAME_TEX_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_WIREFRAME_TEX_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_WIREFRAME_TEX_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_WIREFRAME_TEX_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SOLID:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SOLID_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SOLID_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SOLID_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SOLID_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SOLID_TEX:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SOLID_TEX_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SOLID_TEX_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SOLID_TEX_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SOLID_TEX_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SMOOTH:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SMOOTH_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SMOOTH_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SMOOTH_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SMOOTH_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SMOOTH_TEX:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SMOOTH_TEX_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SMOOTH_TEX_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SMOOTH_TEX_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SMOOTH_TEX_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SHADED:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SHADED_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SHADED_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SHADED_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SHADED_FACE(self, selection);
                break;
            }
        }
        break;
        default:
        break;
    }
}

#include <math.h>

#include "moto-light-node.h"
#include "libmotoutil/moto-gl.h"

/* forwards */

static void moto_light_node_draw(MotoShapeViewNode *self);
static void moto_light_node_prepare_for_draw(MotoShapeViewNode *self);

/* MotoLightModel */

GType moto_light_model_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_LIGHT_MODEL_POINT, "LIGHT_MODEL_POINT", "Point"},
            {MOTO_LIGHT_MODEL_DISTANT, "LIGHT_MODEL_DISTANT", "Distant"},
            {MOTO_LIGHT_MODEL_SPOT, "LIGHT_MODEL_SPOT", "Spot"},
            {MOTO_LIGHT_MODEL_AMBIENT, "LIGHT_MODEL_AMBIENT", "Ambient"},
            {MOTO_LIGHT_MODEL_ENVIRONMENT, "LIGHT_MODEL_ENVIRONMENT", "Environment"},
            {MOTO_LIGHT_MODEL_CUSTOM, "LIGHT_MODEL_CUSTOM", "Custom"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoLightModel", values);
    }
    return type;
}

/* class LightNode */

static GObjectClass *light_node_parent_class = NULL;

#define MOTO_LIGHT_NODE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_LIGHT_NODE, MotoLightNodePriv)

typedef struct
{
    gboolean prepared;
    GLuint dlist;

    GLUquadric *quadric;
} MotoLightNodePriv;

static void
moto_light_node_dispose(GObject *obj)
{
    MotoLightNode *self = (MotoLightNode *)obj;
    MotoLightNodePriv* priv = MOTO_LIGHT_NODE_GET_PRIVATE(self);

    glDeleteLists(priv->dlist, 1);
    gluDeleteQuadric(priv->quadric);

    G_OBJECT_CLASS(light_node_parent_class)->dispose(obj);
}

static void
moto_light_node_finalize(GObject *obj)
{
    light_node_parent_class->finalize(obj);
}

static void
moto_light_node_init(MotoLightNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoLightNodePriv* priv = MOTO_LIGHT_NODE_GET_PRIVATE(self);

    /* params */

    gboolean diffspec[] = {TRUE, TRUE};
    gfloat color[] = {1, 1, 1};

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "model", "Model", MOTO_TYPE_LIGHT_MODEL, MOTO_PARAM_MODE_INOUT, MOTO_LIGHT_MODEL_POINT, NULL, "Light",
            "diffspec", "Diffuse/Specular", MOTO_TYPE_BOOLEAN_2, MOTO_PARAM_MODE_INOUT, diffspec, NULL, "Light",
            "category", "Category", MOTO_TYPE_STRING, MOTO_PARAM_MODE_INOUT, "", NULL, "Light",
            "intensity", "Intensity", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0, NULL, "Light",
            "color", "Color", MOTO_TYPE_FLOAT_3, MOTO_PARAM_MODE_INOUT, color, NULL, "Light",
            "decay", "Decay", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 2.0, NULL, "Light",
            "spot_blur", "Spot Blur", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 2.0, NULL, "Spot",
            "cone_angle", "Cone Angle", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 30.0, NULL, "Spot",
            "cone_angle_delta", "Cone Angle Delta", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 5.0, NULL, "Spot",
            "view", "View", MOTO_TYPE_GEOM_VIEW_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Shape",
            NULL);

    priv->prepared = FALSE;
    priv->dlist = 0;

    priv->quadric = NULL;
}

static void
moto_light_node_class_init(MotoLightNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoLightNodePriv));
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoShapeViewNodeClass *gvnclass = \
        MOTO_SHAPE_VIEW_NODE_CLASS(klass);

    light_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_light_node_dispose;
    goclass->finalize   = moto_light_node_finalize;

    gvnclass->draw              = moto_light_node_draw;
    gvnclass->prepare_for_draw  = moto_light_node_prepare_for_draw;
}

G_DEFINE_TYPE(MotoLightNode, moto_light_node, MOTO_TYPE_GEOM_VIEW_NODE);

/* Methods of class LightNode */

MotoLightNode *moto_light_node_new(const gchar *name)
{
    MotoLightNode *self = (MotoLightNode *)g_object_new(MOTO_TYPE_LIGHT_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void draw_light(MotoLightNode *self)
{
    glColor3f(1, 1, 1);
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();
}

static void moto_light_node_draw(MotoShapeViewNode *self)
{
    MotoLightNode *view = (MotoLightNode *)self;
    MotoLightNodePriv* priv = MOTO_LIGHT_NODE_GET_PRIVATE(self);

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
    // glEnable(GL_LINE_SMOOTH);

    if(!priv->prepared)
        moto_light_node_prepare_for_draw(self);
    else
        glCallList(priv->dlist);

    glPopAttrib();
}

static void moto_light_node_prepare_for_draw(MotoShapeViewNode *self)
{
    MotoLightNodePriv* priv = MOTO_LIGHT_NODE_GET_PRIVATE(self);
    MotoLightNode *view = (MotoLightNode *)self;

    if(!glIsList(priv->dlist))
        priv->dlist = glGenLists(1);

    if(!priv->quadric)
        priv->quadric = gluNewQuadric();

    glNewList(priv->dlist, GL_COMPILE_AND_EXECUTE);

    draw_light(view);

    glEndList();

    priv->prepared = TRUE;
}


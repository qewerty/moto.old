#include <math.h>

#include "moto-light-node.h"
#include "libmotoutil/moto-gl.h"

/* forwards */

static void moto_light_node_draw(MotoGeomViewNode *self);
static void moto_light_node_prepare_for_draw(MotoGeomViewNode *self);

/* MotoLightKind */

GType moto_light_kind_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_LIGHT_KIND_POINT, "LIGHT_KIND_POINT", "Point"},
            {MOTO_LIGHT_KIND_DISTANT, "LIGHT_KIND_DISTANT", "Distant"},
            {MOTO_LIGHT_KIND_SPOT, "LIGHT_KIND_SPOT", "Spot"},
            {MOTO_LIGHT_KIND_CUSTOM, "LIGHT_KIND_CUSTOM", "Custom"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoLightKind", values);
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
            "kind", "Kind", MOTO_TYPE_LIGHT_KIND, MOTO_PARAM_MODE_INOUT, MOTO_LIGHT_KIND_POINT, NULL, "Light",
            "diffspec", "Diffuse/Specular", MOTO_TYPE_BOOLEAN_2, MOTO_PARAM_MODE_INOUT, diffspec, NULL, "Light",
            "intensity", "Intensity", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0, NULL, "Light",
            "color", "Color", MOTO_TYPE_FLOAT_3, MOTO_PARAM_MODE_INOUT, color, NULL, "Light",
            "decay", "Decay", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 2.0, NULL, "Light",
            "spot_blur", "Spot Blur", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 2.0, NULL, "Spot",
            "cone_angle", "Cone Angle", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 30.0, NULL, "Spot",
            "cone_angle_delta", "Cone Angle Delta", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 5.0, NULL, "Spot",
            "view", "View", MOTO_TYPE_GEOM_VIEW_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Geometry",
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
    MotoGeomViewNodeClass *gvnclass = \
        MOTO_GEOM_VIEW_NODE_CLASS(klass);

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

static void moto_light_node_draw(MotoGeomViewNode *self)
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

static void moto_light_node_prepare_for_draw(MotoGeomViewNode *self)
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


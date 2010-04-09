#include <math.h>

#include "moto-axes-view-node.h"
#include "libmotoutil/moto-gl.h"

/* forwards */

static void moto_axes_node_draw(MotoShapeNode *self);
static void moto_axes_node_prepare_for_draw(MotoShapeNode *self);

/* class AxesNode */

static GObjectClass *axes_node_parent_class = NULL;

struct _MotoAxesNodePriv
{
    gboolean prepared;
    GLuint dlist;

    GLUquadric *quadric;
};

static void
moto_axes_node_dispose(GObject *obj)
{
    MotoAxesNode *self = (MotoAxesNode *)obj;

    glDeleteLists(self->priv->dlist, 1);
    gluDeleteQuadric(self->priv->quadric);

    G_OBJECT_CLASS(axes_node_parent_class)->dispose(obj);
}

static void
moto_axes_node_finalize(GObject *obj)
{
    MotoAxesNode *self = (MotoAxesNode *)obj;
    g_slice_free(MotoAxesNodePriv, self->priv);

    axes_node_parent_class->finalize(obj);
}

static void
moto_axes_node_init(MotoAxesNode *self)
{
    MotoNode *node = (MotoNode *)self;

    self->priv = g_slice_new(MotoAxesNodePriv);

    /* params */

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "view", "View", MOTO_TYPE_SHAPE_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Shape",
            NULL);

    self->priv->prepared = FALSE;
    self->priv->dlist = 0;

    self->priv->quadric = NULL;
}

static void
moto_axes_node_class_init(MotoAxesNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoShapeNodeClass *gvnclass = \
        MOTO_SHAPE_NODE_CLASS(klass);

    axes_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_axes_node_dispose;
    goclass->finalize   = moto_axes_node_finalize;

    gvnclass->draw              = moto_axes_node_draw;
    // gvnclass->prepare_for_draw  = moto_axes_node_prepare_for_draw;
}

G_DEFINE_TYPE(MotoAxesNode, moto_axes_node, MOTO_TYPE_SHAPE_NODE);

/* Methods of class AxesNode */

MotoAxesNode *moto_axes_node_new(const gchar *name)
{
    MotoAxesNode *self = (MotoAxesNode *)g_object_new(MOTO_TYPE_AXES_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void draw_axes(MotoAxesNode *self)
{
    GLint slices = 8;
    GLint len = 5;
    GLUquadric *q = self->priv->quadric;

    glLineWidth(1);

    glColor4f(1, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(len, 0, 0);
    glEnd();

    /*
    glPushMatrix();
    glTranslatef(1, 0, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(q, 0.02, 0, 0.06, slices, 1);
    glPopMatrix();
    */

    glColor4f(0, 1, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, len, 0);
    glEnd();

    /*
    glPushMatrix();
    glTranslatef(0, 1, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(q, 0.02, 0, 0.06, slices, 1);
    glPopMatrix();
    */

    glColor4f(0, 0, 1, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, len);
    glEnd();

    /*
    glPushMatrix();
    glTranslatef(0, 0, 1);
    // glRotatef(90, 0, 1, 0);
    gluCylinder(q, 0.02, 0, 0.06, slices, 1);
    glPopMatrix();
    */

}

static void moto_axes_node_draw(MotoShapeNode *self)
{
    MotoAxesNode *view = (MotoAxesNode *)self;

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
    // glEnable(GL_LINE_SMOOTH);

    if( ! view->priv->prepared)
        moto_axes_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    glPopAttrib();
}

static void moto_axes_node_prepare_for_draw(MotoShapeNode *self)
{
    MotoAxesNode *view = (MotoAxesNode *)self;

    if( ! glIsList(view->priv->dlist))
        view->priv->dlist = glGenLists(1);

    if( ! view->priv->quadric)
        view->priv->quadric = gluNewQuadric();

    glNewList(view->priv->dlist, GL_COMPILE_AND_EXECUTE);

    draw_axes(view);

    glEndList();

    view->priv->prepared = TRUE;
}

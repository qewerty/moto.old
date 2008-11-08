#include <math.h>

#include "moto-axes-view-node.h"
#include "libmotoutil/moto-gl.h"

/* forwards */

static void moto_axes_view_node_draw(MotoGeomViewNode *self);
static void moto_axes_view_node_prepare_for_draw(MotoGeomViewNode *self);

/* class AxesViewNode */

static GObjectClass *axes_view_node_parent_class = NULL;

struct _MotoAxesViewNodePriv
{
    gboolean prepared;
    GLuint dlist;

    GLUquadric *quadric;
};

static void
moto_axes_view_node_dispose(GObject *obj)
{
    MotoAxesViewNode *self = (MotoAxesViewNode *)obj;

    glDeleteLists(self->priv->dlist, 1);
    gluDeleteQuadric(self->priv->quadric);

    G_OBJECT_CLASS(axes_view_node_parent_class)->dispose(obj);
}

static void
moto_axes_view_node_finalize(GObject *obj)
{
    MotoAxesViewNode *self = (MotoAxesViewNode *)obj;
    g_slice_free(MotoAxesViewNodePriv, self->priv);

    axes_view_node_parent_class->finalize(obj);
}

static void
moto_axes_view_node_init(MotoAxesViewNode *self)
{
    MotoNode *node = (MotoNode *)self;

    self->priv = g_slice_new(MotoAxesViewNodePriv);

    /* params */

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "view", "View", MOTO_TYPE_GEOMETRY_VIEW_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Geometry", "Geometry",
            NULL);

    self->priv->prepared = FALSE;
    self->priv->dlist = 0;

    self->priv->quadric = NULL;
}

static void
moto_axes_view_node_class_init(MotoAxesViewNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoGeomViewNodeClass *gvnclass = \
        MOTO_GEOM_VIEW_NODE_CLASS(klass);

    axes_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_axes_view_node_dispose;
    goclass->finalize   = moto_axes_view_node_finalize;

    gvnclass->draw              = moto_axes_view_node_draw;
    gvnclass->prepare_for_draw  = moto_axes_view_node_prepare_for_draw;
}

G_DEFINE_TYPE(MotoAxesViewNode, moto_axes_view_node, MOTO_TYPE_GEOMETRY_VIEW_NODE);

/* methods of class AxesViewNode */

MotoAxesViewNode *moto_axes_view_node_new(const gchar *name)
{
    MotoAxesViewNode *self = (MotoAxesViewNode *)g_object_new(MOTO_TYPE_AXES_VIEW_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void draw_axes(MotoAxesViewNode *self)
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

static void moto_axes_view_node_draw(MotoGeomViewNode *self)
{
    MotoAxesViewNode *view = (MotoAxesViewNode *)self;

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
    // glEnable(GL_LINE_SMOOTH);

    if( ! view->priv->prepared)
        moto_axes_view_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    glPopAttrib();
}

static void moto_axes_view_node_prepare_for_draw(MotoGeomViewNode *self)
{
    MotoAxesViewNode *view = (MotoAxesViewNode *)self;

    if( ! glIsList(view->priv->dlist))
        view->priv->dlist = glGenLists(1);

    if( ! view->priv->quadric)
        view->priv->quadric = gluNewQuadric();

    glNewList(view->priv->dlist, GL_COMPILE_AND_EXECUTE);

    draw_axes(view);

    glEndList();

    view->priv->prepared = TRUE;
}

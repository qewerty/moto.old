#include "math.h"

#include "GL/gl.h"
#include "GL/glu.h"

#include "moto-axes-view-node.h"

/* forwards */

static void moto_axes_view_node_draw(MotoGeometryViewNode *self);
static void moto_axes_view_node_prepare_for_draw(MotoGeometryViewNode *self);

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
    self->priv = g_slice_new(MotoAxesViewNodePriv);

    self->priv->prepared = FALSE;
    self->priv->dlist = 0;

    self->priv->quadric = NULL;
}

static void
moto_axes_view_node_class_init(MotoAxesViewNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoGeometryViewNodeClass *gvnclass = \
        MOTO_GEOMETRY_VIEW_NODE_CLASS(klass);

    axes_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_axes_view_node_dispose;
    goclass->finalize   = moto_axes_view_node_finalize;

    gvnclass->draw              = moto_axes_view_node_draw;
    gvnclass->prepare_for_draw  = moto_axes_view_node_prepare_for_draw;
}

G_DEFINE_TYPE(MotoAxesViewNode, moto_axes_view_node, MOTO_TYPE_GEOMETRY_VIEW_NODE);

/* methods of class AxesViewNode */

static gpointer get_view(MotoParam *param)
{
    return moto_param_get_node(param);
}

MotoAxesViewNode *moto_axes_view_node_new(const gchar *name)
{
    MotoAxesViewNode *self = (MotoAxesViewNode *)g_object_new(MOTO_TYPE_AXES_VIEW_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    /* params */

    /*
    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("view", "Geometry View", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_geometry_view_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, get_view, NULL);
    */

    return self;
}

static void draw_axes(MotoAxesViewNode *self)
{
    GLint slices = 8;
    GLUquadric *q = self->priv->quadric;

    glLineWidth(1);

    glColor4f(1, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();

    glPushMatrix();
    glTranslatef(1, 0, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(q, 0.02, 0, 0.06, slices, 1);
    glPopMatrix();

    glColor4f(0, 1, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glEnd();

    glPushMatrix();
    glTranslatef(0, 1, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(q, 0.02, 0, 0.06, slices, 1);
    glPopMatrix();

    glColor4f(0, 0, 1, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();

    glPushMatrix();
    glTranslatef(0, 0, 1);
    // glRotatef(90, 0, 1, 0);
    gluCylinder(q, 0.02, 0, 0.06, slices, 1);
    glPopMatrix();

}

static void moto_axes_view_node_draw(MotoGeometryViewNode *self)
{
    MotoAxesViewNode *view = (MotoAxesViewNode *)self;

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LINE_SMOOTH);

    if( ! view->priv->prepared)
        moto_axes_view_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    glPopAttrib();
}

static void moto_axes_view_node_prepare_for_draw(MotoGeometryViewNode *self)
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

/* class AxesViewNodeFactory */

GType moto_axes_view_node_factory_get_node_type(MotoNodeFactory *self);

MotoNode *
moto_axes_view_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name);

static GObjectClass *axes_view_node_factory_parent_class = NULL;

static void
moto_axes_view_node_factory_dispose(GObject *obj)
{
    G_OBJECT_CLASS(axes_view_node_factory_parent_class)->dispose(obj);
}

static void
moto_axes_view_node_factory_finalize(GObject *obj)
{
    axes_view_node_factory_parent_class->finalize(obj);
}

static void
moto_axes_view_node_factory_init(MotoAxesViewNodeFactory *self)
{}

static void
moto_axes_view_node_factory_class_init(MotoAxesViewNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeFactoryClass *nfclass = (MotoNodeFactoryClass *)klass;

    axes_view_node_factory_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_axes_view_node_factory_dispose;
    goclass->finalize   = moto_axes_view_node_factory_finalize;

    nfclass->get_node_type  = moto_axes_view_node_factory_get_node_type;
    nfclass->create_node    = moto_axes_view_node_factory_create_node;
}

G_DEFINE_TYPE(MotoAxesViewNodeFactory, moto_axes_view_node_factory, MOTO_TYPE_NODE_FACTORY);

/* methods of class AxesViewNodeFactory */

static MotoNodeFactory *axes_view_node_factory = NULL;

MotoNodeFactory *moto_axes_view_node_factory_new()
{
    if( ! axes_view_node_factory)
        axes_view_node_factory = \
            (MotoNodeFactory *)g_object_new(MOTO_TYPE_AXES_VIEW_NODE_FACTORY, NULL);

    return axes_view_node_factory;
}

GType moto_axes_view_node_factory_get_node_type(MotoNodeFactory *self)
{
    return MOTO_TYPE_AXES_VIEW_NODE;
}

MotoNode *moto_axes_view_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name)
{
    return (MotoNode *)moto_axes_view_node_new(name);
}

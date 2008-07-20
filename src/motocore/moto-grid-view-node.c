#include "math.h"

#include "GL/gl.h"
#include "GL/glu.h"

#include "numdef.h"
#include "moto-grid-view-node.h"

/* forwards */

static void moto_grid_view_node_draw(MotoGeometryViewNode *self);
static void moto_grid_view_node_prepare_for_draw(MotoGeometryViewNode *self);

/* class GridViewNode */

static GObjectClass *grid_view_node_parent_class = NULL;

struct _MotoGridViewNodePriv
{
    gboolean disposed;

    gboolean prepared;
    GLuint dlist;
};

static void
moto_grid_view_node_dispose(GObject *obj)
{
    MotoGridViewNode *self = (MotoGridViewNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    grid_view_node_parent_class->dispose(obj);
}

static void
moto_grid_view_node_finalize(GObject *obj)
{
    MotoGridViewNode *self = (MotoGridViewNode *)obj;

    g_slice_free(MotoGridViewNodePriv, self->priv);

    grid_view_node_parent_class->finalize(obj);
}

static void
moto_grid_view_node_init(MotoGridViewNode *self)
{
    MotoNode *node = (MotoNode *)self;

    self->priv = g_slice_new(MotoGridViewNodePriv);
    self->priv->disposed = FALSE;

    /* params */

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "view", "View", MOTO_TYPE_GEOMETRY_VIEW_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Geometry", "Geometry",
            NULL);

    self->priv->prepared = FALSE;
    self->priv->dlist = 0;
}

static void
moto_grid_view_node_class_init(MotoGridViewNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoGeometryViewNodeClass *gvnclass = \
        MOTO_GEOMETRY_VIEW_NODE_CLASS(klass);

    grid_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_grid_view_node_dispose;
    goclass->finalize   = moto_grid_view_node_finalize;

    gvnclass->draw              = moto_grid_view_node_draw;
    gvnclass->prepare_for_draw  = moto_grid_view_node_prepare_for_draw;
}

G_DEFINE_TYPE(MotoGridViewNode, moto_grid_view_node, MOTO_TYPE_GEOMETRY_VIEW_NODE);

/* methods of class GridViewNode */

MotoGridViewNode *moto_grid_view_node_new(const gchar *name)
{
    MotoGridViewNode *self = (MotoGridViewNode *)g_object_new(MOTO_TYPE_GRID_VIEW_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void draw_grid()
{
    gfloat s;

    glLineWidth(0.5);
    glColor4f(0.6, 0.6, 0.6, 0.2);

    glBegin(GL_LINES);
    for(s = -10; s < 10; s += 0.1)
    {
        if(fmod(fabs(s), 1) < 0.05 || (1 - fmod(fabs(s), 1)) < 0.05)
            continue;

        glVertex3f(s, 0, -10);
        glVertex3f(s, 0, 10);
    }
    for(s = -10; s < 10; s += 0.1)
    {
        if(fmod(fabs(s), 1) < 0.05 || (1 - fmod(fabs(s), 1)) < 0.05)
            continue;

        glVertex3f(-10, 0, s);
        glVertex3f(10, 0, s);
    }
    glEnd();

    glLineWidth(2);
    glColor4f(0.6, 0.6, 0.6, 0.4);

    glBegin(GL_LINES);
    for(s = -10; s < 10.5; s += 1)
    {
        glVertex3f(s, 0, -10);
        glVertex3f(s, 0, 10);
    }
    for(s = -10; s < 10.5; s += 1)
    {
        glVertex3f(-10, 0, s);
        glVertex3f(10, 0, s);
    }
    glEnd();
}

static void moto_grid_view_node_draw(MotoGeometryViewNode *self)
{
    MotoGridViewNode *view = (MotoGridViewNode *)self;

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LINE_SMOOTH);

    if( ! view->priv->prepared)
        moto_grid_view_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    glPopAttrib();
}

static void moto_grid_view_node_prepare_for_draw(MotoGeometryViewNode *self)
{
    MotoGridViewNode *view = (MotoGridViewNode *)self;

    if( ! glIsList(view->priv->dlist))
        view->priv->dlist = glGenLists(1);

    glNewList(view->priv->dlist, GL_COMPILE_AND_EXECUTE);

    draw_grid();

    glEndList();

    view->priv->prepared = TRUE;
}

/* class GridViewNodeFactory */

GType moto_grid_view_node_factory_get_node_type(MotoNodeFactory *self);

MotoNode *
moto_grid_view_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name);

static GObjectClass *grid_view_node_factory_parent_class = NULL;

static void
moto_grid_view_node_factory_dispose(GObject *obj)
{
    G_OBJECT_CLASS(grid_view_node_factory_parent_class)->dispose(obj);
}

static void
moto_grid_view_node_factory_finalize(GObject *obj)
{
    grid_view_node_factory_parent_class->finalize(obj);
}

static void
moto_grid_view_node_factory_init(MotoGridViewNodeFactory *self)
{}

static void
moto_grid_view_node_factory_class_init(MotoGridViewNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeFactoryClass *nfclass = (MotoNodeFactoryClass *)klass;

    grid_view_node_factory_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_grid_view_node_factory_dispose;
    goclass->finalize   = moto_grid_view_node_factory_finalize;

    nfclass->get_node_type  = moto_grid_view_node_factory_get_node_type;
    nfclass->create_node    = moto_grid_view_node_factory_create_node;
}

G_DEFINE_TYPE(MotoGridViewNodeFactory, moto_grid_view_node_factory, MOTO_TYPE_NODE_FACTORY);

/* methods of class GridViewNodeFactory */

static MotoNodeFactory *grid_view_node_factory = NULL;

MotoNodeFactory *moto_grid_view_node_factory_new()
{
    if( ! grid_view_node_factory)
        grid_view_node_factory = \
            (MotoNodeFactory *)g_object_new(MOTO_TYPE_GRID_VIEW_NODE_FACTORY, NULL);

    return grid_view_node_factory;
}

GType moto_grid_view_node_factory_get_node_type(MotoNodeFactory *self)
{
    return MOTO_TYPE_GRID_VIEW_NODE;
}

MotoNode *moto_grid_view_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name)
{
    return (MotoNode *)moto_grid_view_node_new(name);
}

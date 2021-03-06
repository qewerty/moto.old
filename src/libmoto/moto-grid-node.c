#include "math.h"

#include "GL/glew.h"
#include "GL/glu.h"

#include "libmotoutil/numdef.h"
#include "moto-grid-node.h"

/* forwards */

static void moto_grid_node_draw(MotoShapeNode *self);
static void moto_grid_node_prepare_for_draw(MotoGridNode *self);

/* class GridNode */

static GObjectClass *grid_node_parent_class = NULL;

struct _MotoGridNodePriv
{
    gboolean disposed;

    gboolean prepared;
    GLuint dlist;
};

static void
moto_grid_node_dispose(GObject *obj)
{
    MotoGridNode *self = (MotoGridNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    grid_node_parent_class->dispose(obj);
}

static void
moto_grid_node_finalize(GObject *obj)
{
    MotoGridNode *self = (MotoGridNode *)obj;

    g_slice_free(MotoGridNodePriv, self->priv);

    grid_node_parent_class->finalize(obj);
}

static void
moto_grid_node_init(MotoGridNode *self)
{
    MotoNode *node = (MotoNode *)self;

    self->priv = g_slice_new(MotoGridNodePriv);
    self->priv->disposed = FALSE;

    /* params */

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "view", "View", MOTO_TYPE_SHAPE_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Shape",
            NULL);

    self->priv->prepared = FALSE;
    self->priv->dlist = 0;
}

static void
moto_grid_node_class_init(MotoGridNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoShapeNodeClass *gvnclass = \
        MOTO_SHAPE_NODE_CLASS(klass);

    grid_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_grid_node_dispose;
    goclass->finalize   = moto_grid_node_finalize;

    gvnclass->draw              = moto_grid_node_draw;
    // gvnclass->prepare_for_draw  = moto_grid_node_prepare_for_draw;
}

G_DEFINE_TYPE(MotoGridNode, moto_grid_node, MOTO_TYPE_SHAPE_NODE);

/* Methods of class GridNode */

MotoGridNode *moto_grid_node_new(const gchar *name)
{
    MotoGridNode *self = (MotoGridNode *)g_object_new(MOTO_TYPE_GRID_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void draw_grid()
{
    gfloat s;

    /*
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
    */

    glLineWidth(1);
    glColor4f(0.1, 0.1, 0.1, 1);

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

static void moto_grid_node_draw(MotoShapeNode *self)
{
    MotoGridNode *view = (MotoGridNode *)self;

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LINE_SMOOTH);

    if( ! view->priv->prepared)
        moto_grid_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    glPopAttrib();
}

static void moto_grid_node_prepare_for_draw(MotoGridNode *self)
{
    MotoGridNode *view = (MotoGridNode *)self;

    if( ! glIsList(view->priv->dlist))
        view->priv->dlist = glGenLists(1);

    glNewList(view->priv->dlist, GL_COMPILE_AND_EXECUTE);

    draw_grid();

    glEndList();

    view->priv->prepared = TRUE;
}


#include "GL/glew.h"
#include "GL/glu.h"

#include "libmotoutil/matrix.h"

#include "moto-ray-view-node.h"

/* forwards */

static void moto_ray_view_node_draw(MotoGeomViewNode *self);
static void moto_ray_view_node_prepare_for_draw(MotoGeomViewNode *self);
static MotoGeometryNode *moto_ray_view_node_get_geometry(MotoGeomViewNode *self);

/* class RayViewNode */

static GObjectClass *ray_view_node_parent_class = NULL;

struct _MotoRayViewNodePriv
{
    gboolean disposed;

    MotoRay ray;

    GLuint dlist;
};

static void
moto_ray_view_node_dispose(GObject *obj)
{
    MotoRayViewNode *self = (MotoRayViewNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    G_OBJECT_CLASS(ray_view_node_parent_class)->dispose(obj);
}

static void
moto_ray_view_node_finalize(GObject *obj)
{
    MotoRayViewNode *self = (MotoRayViewNode *)obj;

    g_slice_free(MotoRayViewNodePriv, self->priv);

    ray_view_node_parent_class->finalize(obj);
}

static void
moto_ray_view_node_init(MotoRayViewNode *self)
{
    MotoNode *node = (MotoNode *)self;

    self->priv = g_slice_new(MotoRayViewNodePriv);
    self->priv->disposed = FALSE;

    /* params */

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "view", "View", MOTO_TYPE_RAY_VIEW_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Geometry",
            NULL);

    self->priv->ray.pos[0] = 0;
    self->priv->ray.pos[1] = 0;
    self->priv->ray.pos[2] = 0;
    self->priv->ray.dir[0] = 1;
    self->priv->ray.dir[1] = 1;
    self->priv->ray.dir[2] = 1;

    self->priv->dlist = 0;
}

static void
moto_ray_view_node_class_init(MotoRayViewNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoGeomViewNodeClass *gvclass = \
        MOTO_GEOM_VIEW_NODE_CLASS(klass);

    ray_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_ray_view_node_dispose;
    goclass->finalize   = moto_ray_view_node_finalize;

    gvclass->draw               = moto_ray_view_node_draw;
    gvclass->prepare_for_draw   = moto_ray_view_node_prepare_for_draw;
    gvclass->get_geometry       = moto_ray_view_node_get_geometry;

    /*
    gvclass->states = g_slist_append(gvclass->states,
            moto_geom_view_state_new("object", "Object",
                moto_ray_view_node_draw_as_object));
    */
}

G_DEFINE_TYPE(MotoRayViewNode, moto_ray_view_node, MOTO_TYPE_GEOM_VIEW_NODE);

/* Methods of class RayViewNode */

static gpointer view_param_get(MotoParam *param)
{
    return moto_param_get_node(param);
}

MotoRayViewNode *moto_ray_view_node_new(const gchar *name)
{
    MotoRayViewNode *self = (MotoRayViewNode *)g_object_new(MOTO_TYPE_RAY_VIEW_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    MotoGeomViewNode *gv = (MotoGeomViewNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void moto_ray_view_node_draw(MotoGeomViewNode *self)
{
    MotoRayViewNode *view = (MotoRayViewNode *)self;

    if( ! moto_geom_view_node_get_prepared(self))
        moto_ray_view_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    /* draw */
}

static void moto_ray_view_node_prepare_for_draw(MotoGeomViewNode *self)
{
    MotoRayViewNode *rv = (MotoRayViewNode *)self;

    if( ! glIsList(rv->priv->dlist))
        rv->priv->dlist = glGenLists(1);

    glNewList(rv->priv->dlist, GL_COMPILE_AND_EXECUTE);

    gfloat point[3];
    vector3_sum(point, rv->priv->ray.pos, rv->priv->ray.dir);

    glLineWidth(4);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor4f(1, 1, 1, 1);
    glVertex3fv(rv->priv->ray.pos);
    glColor4f(0, 0, 1, 1);
    glVertex3fv(point);
    glEnd();
    glEnable(GL_LIGHTING);

    glEndList();

    moto_geom_view_node_set_prepared(self, TRUE);
}

static MotoGeometryNode *moto_ray_view_node_get_geometry(MotoGeomViewNode *self)
{
    return NULL;
}

MotoRay *moto_ray_view_node_get_ray(MotoRayViewNode *self)
{
    return & self->priv->ray;
}

void moto_ray_view_node_set_ray(MotoRayViewNode *self, MotoRay *ray)
{
    moto_ray_copy(& self->priv->ray, ray);
    moto_geom_view_node_set_prepared((MotoGeomViewNode *)self, FALSE);
}


#include "GL/gl.h"
#include "GL/glu.h"

#include "common/matrix.h"

#include "moto-ray-view-node.h"

/* forwards */

static void moto_ray_view_node_draw(MotoGeometryViewNode *self);
static void moto_ray_view_node_prepare_for_draw(MotoGeometryViewNode *self);
static MotoGeometryNode *moto_ray_view_node_get_geometry(MotoGeometryViewNode *self);

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
    self->priv = g_slice_new(MotoRayViewNodePriv);
    self->priv->disposed = FALSE;

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
    MotoGeometryViewNodeClass *gvclass = \
        MOTO_GEOMETRY_VIEW_NODE_CLASS(klass);

    ray_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_ray_view_node_dispose;
    goclass->finalize   = moto_ray_view_node_finalize;

    gvclass->draw               = moto_ray_view_node_draw;
    gvclass->prepare_for_draw   = moto_ray_view_node_prepare_for_draw;
    gvclass->get_geometry       = moto_ray_view_node_get_geometry;

    /*
    gvclass->states = g_slist_append(gvclass->states,
            moto_geometry_view_state_new("object", "Object",
                moto_ray_view_node_draw_as_object));
    */
}

G_DEFINE_TYPE(MotoRayViewNode, moto_ray_view_node, MOTO_TYPE_GEOMETRY_VIEW_NODE);

/* methods of class RayViewNode */

static gpointer view_param_get(MotoParam *param)
{
    return moto_param_get_node(param);
}

MotoRayViewNode *moto_ray_view_node_new(const gchar *name)
{
    MotoRayViewNode *self = (MotoRayViewNode *)g_object_new(MOTO_TYPE_RAY_VIEW_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    MotoGeometryViewNode *gv = (MotoGeometryViewNode *)self;

    moto_node_set_name(node, name);

    /* moto_geometry_view_node_set_state(gv, "object"); */

    /* params */

    /*
    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("view", "Geometry View", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_geometry_view_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, view_param_get, NULL);
    */

    return self;
}

static void moto_ray_view_node_draw(MotoGeometryViewNode *self)
{
    MotoRayViewNode *view = (MotoRayViewNode *)self;

    if( ! moto_geometry_view_node_get_prepared(self))
        moto_ray_view_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    /* draw */
}

static void moto_ray_view_node_prepare_for_draw(MotoGeometryViewNode *self)
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

    moto_geometry_view_node_set_prepared(self, TRUE);
}

static MotoGeometryNode *moto_ray_view_node_get_geometry(MotoGeometryViewNode *self)
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
    moto_geometry_view_node_set_prepared((MotoGeometryViewNode *)self, FALSE);
}

/* class RayViewNodeFactory */

GType moto_ray_view_node_factory_get_node_type(MotoNodeFactory *self);

MotoNode *
moto_ray_view_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name);

static GObjectClass *ray_view_node_factory_parent_class = NULL;

static void
moto_ray_view_node_factory_dispose(GObject *obj)
{
    G_OBJECT_CLASS(ray_view_node_factory_parent_class)->dispose(obj);
}

static void
moto_ray_view_node_factory_finalize(GObject *obj)
{
    ray_view_node_factory_parent_class->finalize(obj);
}

static void
moto_ray_view_node_factory_init(MotoRayViewNodeFactory *self)
{}

static void
moto_ray_view_node_factory_class_init(MotoRayViewNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeFactoryClass *nfclass = (MotoNodeFactoryClass *)klass;

    ray_view_node_factory_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_ray_view_node_factory_dispose;
    goclass->finalize   = moto_ray_view_node_factory_finalize;

    nfclass->get_node_type  = moto_ray_view_node_factory_get_node_type;
    nfclass->create_node    = moto_ray_view_node_factory_create_node;
}

G_DEFINE_TYPE(MotoRayViewNodeFactory, moto_ray_view_node_factory, MOTO_TYPE_NODE_FACTORY);

/* methods of class RayViewNodeFactory */

static MotoNodeFactory *ray_view_node_factory = NULL;

MotoNodeFactory *moto_ray_view_node_factory_new()
{
    if( ! ray_view_node_factory)
        ray_view_node_factory = \
            (MotoNodeFactory *)g_object_new(MOTO_TYPE_RAY_VIEW_NODE_FACTORY, NULL);

    return ray_view_node_factory;
}

GType moto_ray_view_node_factory_get_node_type(MotoNodeFactory *self)
{
    return MOTO_TYPE_RAY_VIEW_NODE;
}

MotoNode *moto_ray_view_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name)
{
    return (MotoNode *)moto_ray_view_node_new(name);
}


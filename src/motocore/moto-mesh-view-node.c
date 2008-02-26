#include "GL/gl.h"
#include "GL/glu.h"

#include "moto-mesh-view-node.h"
#include "moto-mesh.h"
#include "moto-mesh-param-data.h"
#include "moto-geometry-view-param-data.h"

/* forwards */

static void moto_mesh_view_node_draw(MotoGeometryViewNode *self);
static void moto_mesh_view_node_prepare_for_draw(MotoGeometryViewNode *self);

/* class MeshViewNode */

static GObjectClass *mesh_view_node_parent_class = NULL;

struct _MotoMeshViewNodePriv
{
    MotoMesh *mesh;
    MotoMesh **mesh_ptr;

    gboolean prepared;
    GLuint dlist;
};

static void
moto_mesh_view_node_dispose(GObject *obj)
{
    MotoMeshViewNode *self = (MotoMeshViewNode *)obj;

    g_slice_free(MotoMeshViewNodePriv, self->priv);

    G_OBJECT_CLASS(mesh_view_node_parent_class)->dispose(obj);
}

static void
moto_mesh_view_node_finalize(GObject *obj)
{
    mesh_view_node_parent_class->finalize(obj);
}

static void
moto_mesh_view_node_init(MotoMeshViewNode *self)
{
    self->priv = g_slice_new(MotoMeshViewNodePriv);

    self->priv->mesh = NULL;
    self->priv->mesh_ptr = & self->priv->mesh;

    self->priv->prepared = FALSE;
    self->priv->dlist = 0;
}

static void
moto_mesh_view_node_class_init(MotoMeshViewNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoGeometryViewNodeClass *gvnclass = \
        MOTO_GEOMETRY_VIEW_NODE_CLASS(klass);

    mesh_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_view_node_dispose;
    goclass->finalize   = moto_mesh_view_node_finalize;

    gvnclass->draw              = moto_mesh_view_node_draw;
    gvnclass->prepare_for_draw  = moto_mesh_view_node_prepare_for_draw;
}

G_DEFINE_TYPE(MotoMeshViewNode, moto_mesh_view_node, MOTO_TYPE_GEOMETRY_VIEW_NODE);

/* methods of class MeshViewNode */

static void update_mesh(MotoParam *param)
{
    MotoMeshViewNode *obj = (MotoMeshViewNode *)moto_param_get_node(param);
    obj->priv->prepared = FALSE;
}

static void point_mesh(MotoParam *param, gpointer p)
{
    MotoMeshViewNode *obj = (MotoMeshViewNode *)moto_param_get_node(param);

    obj->priv->mesh_ptr = (MotoMesh **)p;
}

static gpointer get_view(MotoParam *param)
{
    return moto_param_get_node(param);
}

MotoMeshViewNode *moto_mesh_view_node_new(const gchar *name)
{
    MotoMeshViewNode *self = (MotoMeshViewNode *)g_object_new(MOTO_TYPE_MESH_VIEW_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    MotoParamBlock *pb;
    MotoParamData *pdata;

    /* params */

    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("mesh", "Polygonal Mesh", MOTO_PARAM_MODE_IN, pb,
            pdata = moto_mesh_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, point_mesh, update_mesh, NULL, NULL);

    moto_param_new("view", "Geometry View", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_geometry_view_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, get_view, NULL);

    return self;
}

static void draw_mesh(MotoMesh *mesh)
{
    int i, j;
    for(i = 0; i < mesh->faces_num; i++)
    {
        /* TODO: Temporary solution! =) */

        MotoMeshFace *face = & mesh->faces[i];

        glNormal3fv(face->normal);
        glColor3f(1, 0, 0);

        glBegin(GL_POLYGON);

        for(j = 0; j < face->verts_num; j++)
        {
            MotoMeshVertex *vert = & mesh->verts[face->indecies[j]];

            /* TODO: attrs */

            // glNormal3fv(vert->normal);
            glVertex3fv(vert->xyz);
        }

        glEnd();
    }
}

static void moto_mesh_view_node_draw(MotoGeometryViewNode *self)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)self;

    if( ! *(view->priv->mesh_ptr))
        return;

    if( ! view->priv->prepared)
        moto_mesh_view_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    /* draw */
}

static void moto_mesh_view_node_prepare_for_draw(MotoGeometryViewNode *self)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)self;
    MotoMesh *mesh = *(view->priv->mesh_ptr);

    if(! mesh)
        return;

    if( ! glIsList(view->priv->dlist))
        view->priv->dlist = glGenLists(1);

    glNewList(view->priv->dlist, GL_COMPILE_AND_EXECUTE);

    draw_mesh(mesh);

    glEndList();

    view->priv->prepared = TRUE;
}

/* class MeshViewNodeFactory */

GType moto_mesh_view_node_factory_get_node_type(MotoNodeFactory *self);

MotoNode *
moto_mesh_view_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name);

static GObjectClass *mesh_view_node_factory_parent_class = NULL;

static void
moto_mesh_view_node_factory_dispose(GObject *obj)
{
    G_OBJECT_CLASS(mesh_view_node_factory_parent_class)->dispose(obj);
}

static void
moto_mesh_view_node_factory_finalize(GObject *obj)
{
    mesh_view_node_factory_parent_class->finalize(obj);
}

static void
moto_mesh_view_node_factory_init(MotoMeshViewNodeFactory *self)
{}

static void
moto_mesh_view_node_factory_class_init(MotoMeshViewNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeFactoryClass *nfclass = (MotoNodeFactoryClass *)klass;

    mesh_view_node_factory_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_view_node_factory_dispose;
    goclass->finalize   = moto_mesh_view_node_factory_finalize;

    nfclass->get_node_type  = moto_mesh_view_node_factory_get_node_type;
    nfclass->create_node    = moto_mesh_view_node_factory_create_node;
}

G_DEFINE_TYPE(MotoMeshViewNodeFactory, moto_mesh_view_node_factory, MOTO_TYPE_NODE_FACTORY);

/* methods of class MeshViewNodeFactory */

static MotoNodeFactory *mesh_view_node_factory = NULL;

MotoNodeFactory *moto_mesh_view_node_factory_new()
{
    if( ! mesh_view_node_factory)
        mesh_view_node_factory = \
            (MotoNodeFactory *)g_object_new(MOTO_TYPE_MESH_VIEW_NODE_FACTORY, NULL);

    return mesh_view_node_factory;
}

GType moto_mesh_view_node_factory_get_node_type(MotoNodeFactory *self)
{
    return MOTO_TYPE_MESH_VIEW_NODE;
}

MotoNode *moto_mesh_view_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name)
{
    return (MotoNode *)moto_mesh_view_node_new(name);
}

#include "GL/gl.h"
#include "GL/glu.h"

#include "moto-mesh-view-node.h"
#include "moto-mesh.h"
#include "moto-mesh-param-data.h"
#include "moto-geometry-view-param-data.h"

/* forwards */

static void moto_mesh_view_node_draw(MotoGeometryViewNode *self);
static void moto_mesh_view_node_prepare_for_draw(MotoGeometryViewNode *self);

static void moto_mesh_view_node_draw_as_object(MotoGeometryViewState *self, MotoGeometryViewNode *geom);
static void moto_mesh_view_node_draw_as_verts(MotoGeometryViewState *self, MotoGeometryViewNode *geom);
static void moto_mesh_view_node_draw_as_edges(MotoGeometryViewState *self, MotoGeometryViewNode *geom);
static void moto_mesh_view_node_draw_as_faces(MotoGeometryViewState *self, MotoGeometryViewNode *geom);

/* class MeshViewNode */

static GObjectClass *mesh_view_node_parent_class = NULL;

struct _MotoMeshViewNodePriv
{
    MotoMesh *mesh;
    MotoMesh **mesh_ptr;
    MotoMeshSelection *selection;

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

    gvnclass->states = g_slist_append(gvnclass->states,
            moto_geometry_view_state_new("object", "Object",
                moto_mesh_view_node_draw_as_object));
    gvnclass->states = g_slist_append(gvnclass->states,
            moto_geometry_view_state_new("verts", "Vertex",
                moto_mesh_view_node_draw_as_verts));
    gvnclass->states = g_slist_append(gvnclass->states,
            moto_geometry_view_state_new("edges", "Edges",
                moto_mesh_view_node_draw_as_edges));
    gvnclass->states = g_slist_append(gvnclass->states,
            moto_geometry_view_state_new("faces", "Faces",
                moto_mesh_view_node_draw_as_faces));
}

G_DEFINE_TYPE(MotoMeshViewNode, moto_mesh_view_node, MOTO_TYPE_GEOMETRY_VIEW_NODE);

/* methods of class MeshViewNode */

static void update_mesh(MotoParam *param)
{
    MotoGeometryViewNode *obj = (MotoGeometryViewNode *)moto_param_get_node(param);
    moto_geometry_view_node_set_prepared(obj, FALSE);
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
    MotoGeometryViewNode *gvn = (MotoGeometryViewNode *)self;
    MotoGeometryViewNodeClass *gvnclass = (MotoGeometryViewNodeClass *)MOTO_MESH_VIEW_NODE_GET_CLASS(self);

    moto_node_set_name(node, name);

    MotoParamBlock *pb;
    MotoParamData *pdata;

    moto_geometry_view_node_set_state(gvn, "verts");

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

static void process_vertex(MotoMeshFace *face, MotoMeshVertex *vert)
{
    glVertex3fv(vert->xyz);
}

static void draw_mesh_as_object(MotoMesh *mesh)
{
    int i;
    for(i = 0; i < mesh->faces_num; i++)
    {
        /* TODO: Temporary solution! =) */

        MotoMeshFace *face = & mesh->faces[i];

        glNormal3fv(face->normal);

        glBegin(GL_POLYGON);

        moto_mesh_face_foreach_vertex(face, process_vertex, mesh);

        glEnd();
    }
}

void draw_vertex(MotoMesh *mesh, MotoMeshVertex *vert)
{
    glVertex3fv(vert->xyz);
}

static void draw_mesh_as_verts(MotoMesh *mesh)
{
    glColor4f(1, 1, 1, 0.25);
    int i;
    for(i = 0; i < mesh->faces_num; i++)
    {

        MotoMeshFace *face = & mesh->faces[i];

        glNormal3fv(face->normal);

        glBegin(GL_POLYGON);

        moto_mesh_face_foreach_vertex(face, process_vertex, mesh);

        glEnd();
    }

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor4f(1, 0, 0, 1);
    glPointSize(4);
    glBegin(GL_POINTS);
    moto_mesh_foreach_vertex(mesh, draw_vertex);
    glEnd();

    glPopAttrib();
}

static void draw_edge(MotoMesh *mesh, MotoMeshEdge *edge)
{
    glVertex3fv(mesh->verts[edge->a].xyz);
    glVertex3fv(mesh->verts[edge->b].xyz);
}

static void draw_mesh_as_edges(MotoMesh *mesh)
{
    glColor4f(1, 1, 1, 0.25);
    int i;
    for(i = 0; i < mesh->faces_num; i++)
    {

        MotoMeshFace *face = & mesh->faces[i];

        glNormal3fv(face->normal);

        glBegin(GL_POLYGON);

        moto_mesh_face_foreach_vertex(face, process_vertex, mesh);

        glEnd();
    }

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor4f(1, 0, 0, 1);
    glPointSize(4);
    glBegin(GL_LINES);
    moto_mesh_foreach_edge(mesh, draw_edge);
    glEnd();

    glPopAttrib();
}

static void draw_mesh_as_faces(MotoMesh *mesh)
{
    glColor4f(1, 1, 1, 0.25);
    int i;
    for(i = 0; i < mesh->faces_num; i++)
    {

        MotoMeshFace *face = & mesh->faces[i];

        glNormal3fv(face->normal);

        glBegin(GL_POLYGON);

        moto_mesh_face_foreach_vertex(face, process_vertex, mesh);

        glEnd();
    }
}

static void moto_mesh_view_node_draw(MotoGeometryViewNode *self)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)self;

    if( ! *(view->priv->mesh_ptr))
        return;

    if( ! moto_geometry_view_node_get_prepared(self))
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

    MotoGeometryViewState *state = moto_geometry_view_node_get_state((MotoGeometryViewNode *)self);
    if(state)
        moto_geometry_view_state_draw(state, (MotoGeometryViewNode *)self);
    else
        draw_mesh_as_object(mesh);

    glEndList();

    moto_geometry_view_node_set_prepared(self, TRUE);
}

/* states */

static void moto_mesh_view_node_draw_as_object(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = *(view->priv->mesh_ptr);

    if(! mesh)
        return;

    draw_mesh_as_object(mesh);
}

static void moto_mesh_view_node_draw_as_verts(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = *(view->priv->mesh_ptr);

    if(! mesh)
        return;

    draw_mesh_as_verts(mesh);
}

static void moto_mesh_view_node_draw_as_edges(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = *(view->priv->mesh_ptr);

    if(! mesh)
        return;

    draw_mesh_as_edges(mesh);
}

static void moto_mesh_view_node_draw_as_faces(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = *(view->priv->mesh_ptr);

    if(! mesh)
        return;

    draw_mesh_as_faces(mesh);
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


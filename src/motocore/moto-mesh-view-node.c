#include <GL/gl.h>
#include <GL/glu.h>

#include "common/numdef.h"
#include "common/matrix.h"

#include "moto-mesh-view-node.h"
#include "moto-mesh.h"
#include "moto-mesh-param-data.h"
#include "moto-geometry-view-param-data.h"

/* forwards */

static void moto_mesh_view_node_draw(MotoGeometryViewNode *self);
static void moto_mesh_view_node_prepare_for_draw(MotoGeometryViewNode *self);
static gboolean moto_mesh_view_node_select(MotoGeometryViewNode *self, gint x, gint y,
        gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
static MotoGeometryNode *moto_mesh_view_node_get_geometry(MotoGeometryViewNode *self);

static void moto_mesh_view_node_draw_as_object(MotoGeometryViewState *self, MotoGeometryViewNode *geom);
static void moto_mesh_view_node_draw_as_verts(MotoGeometryViewState *self, MotoGeometryViewNode *geom);
static gboolean
moto_mesh_view_node_select_as_verts(MotoGeometryViewState *self, MotoGeometryViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
static void moto_mesh_view_node_draw_as_edges(MotoGeometryViewState *self, MotoGeometryViewNode *geom);
static void moto_mesh_view_node_draw_as_faces(MotoGeometryViewState *self, MotoGeometryViewNode *geom);

/* class MeshViewNode */

static GObjectClass *mesh_view_node_parent_class = NULL;

struct _MotoMeshViewNodePriv
{
    gboolean disposed;

    MotoMesh *mesh;
    MotoMeshSelection *selection;

    GLuint dlist;
};

static void
moto_mesh_view_node_dispose(GObject *obj)
{
    MotoMeshViewNode *self = (MotoMeshViewNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    moto_mesh_selection_free(self->priv->selection);

    G_OBJECT_CLASS(mesh_view_node_parent_class)->dispose(obj);
}

static void
moto_mesh_view_node_finalize(GObject *obj)
{
    MotoMeshViewNode *self = (MotoMeshViewNode *)obj;

    g_slice_free(MotoMeshViewNodePriv, self->priv);

    mesh_view_node_parent_class->finalize(obj);
}

static void
moto_mesh_view_node_init(MotoMeshViewNode *self)
{
    self->priv = g_slice_new(MotoMeshViewNodePriv);
    self->priv->disposed = FALSE;

    self->priv->selection = NULL;

    self->priv->mesh = NULL;

    self->priv->dlist = 0;
}

static void
moto_mesh_view_node_class_init(MotoMeshViewNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoGeometryViewNodeClass *gvclass = \
        MOTO_GEOMETRY_VIEW_NODE_CLASS(klass);

    mesh_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_view_node_dispose;
    goclass->finalize   = moto_mesh_view_node_finalize;

    gvclass->draw               = moto_mesh_view_node_draw;
    gvclass->prepare_for_draw   = moto_mesh_view_node_prepare_for_draw;
    gvclass->select             = moto_mesh_view_node_select;
    gvclass->get_geometry       = moto_mesh_view_node_get_geometry;

    gvclass->states = g_slist_append(gvclass->states,
            moto_geometry_view_state_new("object", "Object",
                moto_mesh_view_node_draw_as_object, NULL));
    gvclass->states = g_slist_append(gvclass->states,
            moto_geometry_view_state_new("verts", "Vertex",
                moto_mesh_view_node_draw_as_verts, moto_mesh_view_node_select_as_verts));
    gvclass->states = g_slist_append(gvclass->states,
            moto_geometry_view_state_new("edges", "Edges",
                moto_mesh_view_node_draw_as_edges, NULL));
    gvclass->states = g_slist_append(gvclass->states,
            moto_geometry_view_state_new("faces", "Faces",
                moto_mesh_view_node_draw_as_faces, NULL));
}

G_DEFINE_TYPE(MotoMeshViewNode, moto_mesh_view_node, MOTO_TYPE_GEOMETRY_VIEW_NODE);

/* methods of class MeshViewNode */

static void mesh_param_update(MotoParam *param)
{
    MotoGeometryViewNode *gv = (MotoGeometryViewNode *)moto_param_get_node(param);
    MotoMeshViewNode *mv = (MotoMeshViewNode *)gv;

    // mv->priv->mesh = *(MotoMesh **)p;

    moto_geometry_view_node_set_prepared(gv, FALSE);

    if( ! mv->priv->mesh)
        return;

    if( ! mv->priv->selection)
    {
        mv->priv->selection = moto_mesh_selection_for_mesh(mv->priv->mesh);

        return;
    }

    if( ! moto_mesh_selection_is_valid(mv->priv->selection, mv->priv->mesh))
    {
        moto_mesh_selection_free(mv->priv->selection);
        mv->priv->selection = moto_mesh_selection_for_mesh(mv->priv->mesh);
    }
}

static void mesh_param_point(MotoParam *param, gpointer p)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)moto_param_get_node(param);

    mv->priv->mesh = *(MotoMesh **)p;

    if( ! mv->priv->mesh)
        return;

    if( ! mv->priv->selection)
    {
        mv->priv->selection = moto_mesh_selection_for_mesh(mv->priv->mesh);

        return;
    }

    if( ! moto_mesh_selection_is_valid(mv->priv->selection, mv->priv->mesh))
    {
        moto_mesh_selection_free(mv->priv->selection);
        mv->priv->selection = moto_mesh_selection_for_mesh(mv->priv->mesh);
    }
}

static gpointer view_param_get(MotoParam *param)
{
    return moto_param_get_node(param);
}

MotoMeshViewNode *moto_mesh_view_node_new(const gchar *name)
{
    MotoMeshViewNode *self = (MotoMeshViewNode *)g_object_new(MOTO_TYPE_MESH_VIEW_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    MotoGeometryViewNode *gv = (MotoGeometryViewNode *)self;

    moto_node_set_name(node, name);

    MotoParamBlock *pb;
    MotoParamData *pdata;

    moto_geometry_view_node_set_state(gv, "object");

    /* params */

    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("mesh", "Polygonal Mesh", MOTO_PARAM_MODE_IN, pb,
            pdata = moto_mesh_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, mesh_param_point, mesh_param_update, NULL, NULL);

    moto_param_new("view", "Geometry View", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_geometry_view_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, view_param_get, NULL);

    return self;
}

MotoMesh *moto_mesh_view_node_get_mesh(MotoMeshViewNode *self)
{
    return self->priv->mesh;
}

MotoMeshSelection *moto_mesh_view_node_get_selection(MotoMeshViewNode *self)
{
    return self->priv->selection;
}

static void process_vertex(MotoMeshFace *face, MotoMeshVert *vert)
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

void draw_vertex(MotoMesh *mesh, MotoMeshVert *vert)
{
    glVertex3fv(vert->xyz);
}

static void draw_mesh_as_verts(MotoMesh *mesh, MotoMeshSelection *selection)
{
    glColor4f(1, 1, 1, 0.25);
    guint i;
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

    MotoMeshVert *vert;
    for(i = 0; i < mesh->verts_num; i++)
    {
        vert = & mesh->verts[i];

        if(moto_mesh_selection_is_vertex_selected(selection, i))
            glColor4f(0, 1, 0, 1);
        else
            glColor4f(1, 0, 0, 1);

        glVertex3fv(vert->xyz);
    }

    glEnd();

    glPopAttrib();
}

static void draw_mesh_as_edges(MotoMesh *mesh, MotoMeshSelection *selection)
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

    MotoMeshEdge *edge;
    for(i = 0; i < mesh->edges_num; i++)
    {
        edge = & mesh->edges[i];

        if(moto_mesh_selection_is_edge_selected(selection, i))
            glColor4f(0, 1, 0, 1);
        else
            glColor4f(1, 0, 0, 1);

        glVertex3fv(mesh->verts[edge->a].xyz);
        glVertex3fv(mesh->verts[edge->b].xyz);
    }

    glEnd();

    glPopAttrib();
}

static void draw_mesh_as_faces(MotoMesh *mesh, MotoMeshSelection *selection)
{
    int i;
    for(i = 0; i < mesh->faces_num; i++)
    {

        MotoMeshFace *face = & mesh->faces[i];

        if(moto_mesh_selection_is_face_selected(selection, i))
            glColor4f(0.5, 1, 0.2, 0.25);
        else
            glColor4f(1, 1, 1, 0.25);

        glNormal3fv(face->normal);

        glBegin(GL_POLYGON);

        moto_mesh_face_foreach_vertex(face, process_vertex, mesh);

        glEnd();
    }
}

static void moto_mesh_view_node_draw(MotoGeometryViewNode *self)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)self;

    if( ! view->priv->mesh)
        return;

    if( ! moto_geometry_view_node_get_prepared(self))
        moto_mesh_view_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    /* draw */
}

static void moto_mesh_view_node_prepare_for_draw(MotoGeometryViewNode *self)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)self;
    MotoMesh *mesh = mv->priv->mesh;

    if(! mesh)
        return;

    if( ! glIsList(mv->priv->dlist))
        mv->priv->dlist = glGenLists(1);

    glNewList(mv->priv->dlist, GL_COMPILE_AND_EXECUTE);

    MotoGeometryViewState *state = moto_geometry_view_node_get_state((MotoGeometryViewNode *)self);
    if(state)
        moto_geometry_view_state_draw(state, (MotoGeometryViewNode *)self);
    else
        draw_mesh_as_object(mesh);

    glEndList();

    moto_geometry_view_node_set_prepared(self, TRUE);
}

static gboolean moto_mesh_view_node_select(MotoGeometryViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)self;
    MotoMesh *mesh = mv->priv->mesh;

    if(! mesh)
        return TRUE;

    if( ! glIsList(mv->priv->dlist))
        mv->priv->dlist = glGenLists(1);

    MotoGeometryViewState *state = moto_geometry_view_node_get_state((MotoGeometryViewNode *)self);
    if(state)
        moto_geometry_view_state_select(state, (MotoGeometryViewNode *)self,
                x, y, width, height, ray, tinfo);
}

static MotoGeometryNode *moto_mesh_view_node_get_geometry(MotoGeometryViewNode *self)
{
    MotoParam *p = moto_node_get_param((MotoNode *)self, "main", "mesh");
    MotoParam *s = moto_param_get_source(p);
    if( ! s)
        return NULL;
    return (MotoGeometryNode *)moto_param_get_node(s);
}

/* states */

static void moto_mesh_view_node_draw_as_object(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = mv->priv->mesh;

    if(! mesh)
        return;

    draw_mesh_as_object(mesh);
}

static void moto_mesh_view_node_draw_as_verts(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = mv->priv->mesh;

    if(! mesh)
        return;

    draw_mesh_as_verts(mesh, mv->priv->selection);
}

static gboolean
moto_mesh_view_node_select_as_verts(MotoGeometryViewState *self, MotoGeometryViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = moto_mesh_view_node_get_mesh(mv);
    if( ! mesh)
    {
        g_print("No mesh\n");
        return TRUE;
    }

    /* Array of intersected verts. */
    GArray *hits = g_array_new(FALSE, FALSE, sizeof(gint));

    gint index = -1;
    gfloat dist, dist_tmp;
    dist = MACRO;
    gfloat square_radius = 0.25*0.25;
    gfloat fovy = atan((1/tinfo->proj[5]))*2;

    gint i;
    for(i = 0; i < mesh->verts_num; i++)
    {
        gfloat *xyz = mesh->verts[i].xyz;

        /* perspective compensatioin for sphere radius */
        gfloat p2v[3]; /* Vector from ray origin to vertex. */
        vector3_dif(p2v, xyz, ray->pos);
        gfloat pc = 1 + vector3_length(p2v)*fovy/PI_HALF;

        if( ! moto_ray_intersect_sphere_2_dist(ray, & dist_tmp, xyz, square_radius*pc))
            continue;

        g_array_append_val(hits, i);

        if(dist_tmp < dist)
        {
            dist = dist_tmp;
            index = i;
        }
    }

    if(index > -1)
    {
        /* Detecting which of intersected verts is nearest to cursor. */
        GLdouble win_dist,
                 min_win_dist = MACRO;
        GLdouble win_x, win_y, win_z, xx, yy;
        gint i, ii;
        for(i = 0; i < hits->len; i++)
        {
            ii = g_array_index(hits, gint, i);
            gluProject(mesh->verts[ii].xyz[0], mesh->verts[ii].xyz[1], mesh->verts[ii].xyz[2],
                    tinfo->model, tinfo->proj, tinfo->view, & win_x, & win_y, & win_z);

            xx = (x - win_x);
            yy = (height - y - win_y);
            win_dist = sqrt(xx*xx + yy*yy);
            if(win_dist < min_win_dist)
            {
                min_win_dist = win_dist;
                index = ii;
            }
        }

        moto_mesh_selection_toggle_vertex_selection(moto_mesh_view_node_get_selection(mv), index);
        moto_geometry_view_node_set_prepared((MotoGeometryViewNode *)mv, FALSE);
        moto_geometry_view_node_draw((MotoGeometryViewNode *)mv);
    }

    return TRUE;
}

static void moto_mesh_view_node_draw_as_edges(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = mv->priv->mesh;

    if(! mesh)
        return;

    draw_mesh_as_edges(mesh, mv->priv->selection);
}

static void moto_mesh_view_node_draw_as_faces(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;
    MotoMesh *mesh = mv->priv->mesh;

    if(! mesh)
        return;

    draw_mesh_as_faces(mesh, mv->priv->selection);
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


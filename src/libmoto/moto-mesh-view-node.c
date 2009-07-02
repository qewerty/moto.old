#include <stdio.h>
#include <string.h>

#include "libmotoutil/moto-gl.h"
#include "libmotoutil/numdef.h"
#include "libmotoutil/matrix.h"

#include "moto-mesh-view-node.h"
#include "moto-enums.h"
#include "moto-mesh.h"
#include "moto-world.h"

typedef enum
{
    VBUF_VERTEX,
    VBUF_NORMAL,
    VBUF_ELEMENT,
    VBUF_LINE_ELEMENT,
    VBUF_NUMBER
} MotoVBufs;

/* forwards */

static void moto_mesh_view_node_draw(MotoGeomViewNode *self);
static void moto_mesh_view_node_prepare_for_draw(MotoGeomViewNode *self);
static gboolean moto_mesh_view_node_select(MotoGeomViewNode *self, gint x, gint y,
        gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
static MotoGeometryNode *moto_mesh_view_node_get_geometry(MotoGeomViewNode *self);

static void moto_mesh_view_node_draw_bound(MotoMeshViewNode *self, gboolean wireframe);

static void moto_mesh_view_node_draw_as_object(MotoGeomViewState *self, MotoGeomViewNode *geom);
static void moto_mesh_view_node_draw_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom);
static gboolean
moto_mesh_view_node_select_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
static void moto_mesh_view_node_grow_selection_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom);
static void moto_mesh_view_node_select_less_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom);
static void moto_mesh_view_node_select_less_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom);
static void moto_mesh_view_node_select_less_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom);

static void moto_mesh_view_node_draw_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom);
static gboolean
moto_mesh_view_node_select_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);

static void moto_mesh_view_node_grow_selection_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom);
static void moto_mesh_view_node_grow_selection_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom);

static void moto_mesh_view_node_invert_selection_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom);
static void moto_mesh_view_node_invert_selection_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom);
static void moto_mesh_view_node_invert_selection_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom);

static void moto_mesh_view_node_draw_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom);
static gboolean
moto_mesh_view_node_select_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);

static void moto_mesh_view_node_update(MotoNode *self);
static void moto_mesh_view_node_delete_buffers(MotoMeshViewNode *self);

/* class MeshViewNode */

typedef struct _MotoMeshViewNodePriv MotoMeshViewNodePriv;

struct _MotoMeshViewNodePriv
{
    gboolean disposed;

    MotoMeshSelection *selection;

    MotoBound *bound;

    GLuint dlist;
    GLuint vbufs[VBUF_NUMBER];
};

#define MOTO_MESH_VIEW_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_MESH_VIEW_NODE, MotoMeshViewNodePriv)
static GObjectClass *mesh_view_node_parent_class = NULL;

static void
moto_mesh_view_node_dispose(GObject *obj)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(obj);
    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    g_print("moto_mesh_view_node_dispose\n");
    glDeleteLists(priv->dlist, 1);
    moto_mesh_view_node_delete_buffers((MotoMeshViewNode *)obj);

    moto_mesh_selection_free(priv->selection);
    g_object_unref(priv->bound);

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
    MotoNode *node = (MotoNode *)self;

    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->selection = NULL;

    priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);

    priv->dlist = 0;
    bzero(priv->vbufs, VBUF_NUMBER*sizeof(GLuint));

    /* params */

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "mesh", "Mesh", MOTO_TYPE_MESH, MOTO_PARAM_MODE_IN, NULL, pspec, "Geometry",
            "view", "View", MOTO_TYPE_GEOM_VIEW_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Geometry",
            NULL);

    moto_geom_view_node_set_state((MotoGeomViewNode *)self, "verts");
}

static void
moto_mesh_view_node_class_init(MotoMeshViewNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoMeshViewNodePriv));

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = MOTO_NODE_CLASS(klass);
    MotoGeomViewNodeClass *gvclass = \
        MOTO_GEOM_VIEW_NODE_CLASS(klass);

    mesh_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_view_node_dispose;
    goclass->finalize   = moto_mesh_view_node_finalize;

    nclass->update = moto_mesh_view_node_update;

    gvclass->draw               = moto_mesh_view_node_draw;
    gvclass->prepare_for_draw   = moto_mesh_view_node_prepare_for_draw;
    gvclass->select             = moto_mesh_view_node_select;
    gvclass->get_geometry       = moto_mesh_view_node_get_geometry;

    gvclass->states = g_slist_append(gvclass->states,
            moto_geom_view_state_new("object", "Object",
                moto_mesh_view_node_draw_as_object, NULL, NULL, NULL, NULL));
    gvclass->states = g_slist_append(gvclass->states,
            moto_geom_view_state_new("verts", "Vertex",
                moto_mesh_view_node_draw_as_verts, moto_mesh_view_node_select_as_verts,
                moto_mesh_view_node_grow_selection_as_verts,
                moto_mesh_view_node_select_less_as_verts,
                moto_mesh_view_node_invert_selection_as_verts));
    gvclass->states = g_slist_append(gvclass->states,
            moto_geom_view_state_new("edges", "Edges",
                moto_mesh_view_node_draw_as_edges, moto_mesh_view_node_select_as_edges,
                moto_mesh_view_node_grow_selection_as_edges,
                moto_mesh_view_node_select_less_as_edges,
                moto_mesh_view_node_invert_selection_as_edges));
    gvclass->states = g_slist_append(gvclass->states,
            moto_geom_view_state_new("faces", "Faces",
                moto_mesh_view_node_draw_as_faces, moto_mesh_view_node_select_as_faces,
                moto_mesh_view_node_grow_selection_as_faces,
                moto_mesh_view_node_select_less_as_faces,
                moto_mesh_view_node_invert_selection_as_faces));
}

G_DEFINE_TYPE(MotoMeshViewNode, moto_mesh_view_node, MOTO_TYPE_GEOM_VIEW_NODE);

/* Methods of class MeshViewNode */

MotoMeshViewNode *moto_mesh_view_node_new(const gchar *name)
{
    MotoMeshViewNode *self = (MotoMeshViewNode *)g_object_new(MOTO_TYPE_MESH_VIEW_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    moto_node_set_name(node, name);
    return self;
}

MotoMesh *moto_mesh_view_node_get_mesh(MotoMeshViewNode *self)
{
    return (MotoMesh *)moto_node_get_param_object(MOTO_NODE(self), "mesh");
}

MotoMeshSelection *moto_mesh_view_node_get_selection(MotoMeshViewNode *self)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);
    return priv->selection;
}

inline static void draw_mesh_as_object(MotoMeshViewNode *mv, MotoMesh *mesh)
{
    MotoMeshViewNodePriv *priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(mv);

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    glPushAttrib(GL_ENABLE_BIT);

    glColor4f(1, 1, 1, 1);

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    gboolean force_arrays = FALSE;
    MotoWorld *world = moto_node_get_world((MotoNode *)mv);
    if(world)
        force_arrays = ! moto_world_get_use_vbo(world);
    if(moto_gl_is_vbo_supported() && ! force_arrays) // VBO used if supported
    {
        if( ! glIsBufferARB(priv->vbufs[VBUF_VERTEX]))
        {
            g_print("Initializing VBO for 'object' drawing mode ... ");
            glGenBuffersARB(VBUF_NUMBER, priv->vbufs);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_VERTEX]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->v_num * sizeof(MotoVector), mesh->v_coords,  GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_NORMAL]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->v_num * sizeof(MotoVector), mesh->v_normals, GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_ELEMENT]);
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    moto_mesh_get_index_size(mesh) * mesh->f_num * 3 * 2,
                    mesh->f_tess_verts, GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
            g_print("OK\n");
        }

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_VERTEX]);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_NORMAL]);
        glNormalPointer(GL_FLOAT, 0, 0);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_ELEMENT]);
        glDrawElements(GL_TRIANGLES, 3*mesh->f_tess_num, mesh->index_gl_type, 0);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    else // Vertex arrays
    {
        glVertexPointer(3, GL_FLOAT, 0, mesh->v_coords);
        glNormalPointer(GL_FLOAT, 0, mesh->v_normals);
        glDrawElements(GL_TRIANGLES, 3*mesh->f_tess_num, mesh->index_gl_type, mesh->f_tess_verts);
    }

    glPopAttrib();
    glPopClientAttrib();
}

inline static void draw_mesh_as_verts(MotoMeshViewNode *mv, MotoMesh *mesh, MotoMeshSelection *selection)
{
    MotoMeshViewNodePriv *priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(mv);

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

    if(moto_gl_is_glsl_supported())
        glUseProgramObjectARB(0);
    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDepthFunc(GL_LEQUAL);

    gboolean force_arrays = FALSE;
    MotoWorld *world = moto_node_get_world((MotoNode *)mv);
    if(world)
        force_arrays = ! moto_world_get_use_vbo(world);

    gboolean using_vbo = TRUE;
    if(moto_gl_is_vbo_supported() && ! force_arrays) // vbo
    {
        g_print("Initializng VBO for 'vertex' drawing mode ... ");
        if( ! glIsBufferARB(priv->vbufs[VBUF_VERTEX]))
        {
            GLenum error;

            glGenBuffersARB(VBUF_NUMBER, priv->vbufs);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_VERTEX]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->v_num * sizeof(MotoVector), mesh->v_coords,  GL_STATIC_DRAW_ARB);

            error = glGetError();
            switch(error)
            {
                case GL_NO_ERROR:
                    // Ok
                break;
                case GL_OUT_OF_MEMORY:
                // break;
                default:
                    // Unknown Error
                    moto_mesh_view_node_delete_buffers(mv);
                    using_vbo = FALSE;
                break;
            }

            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_ELEMENT]);
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    moto_mesh_get_index_size(mesh) * mesh->e_num * 2,
                    mesh->e_verts, GL_STATIC_DRAW_ARB);

            error = glGetError();
            switch(error)
            {
                case GL_NO_ERROR:
                    // Ok
                break;
                case GL_OUT_OF_MEMORY:
                // break;
                default:
                    // Unknown Error
                    moto_mesh_view_node_delete_buffers(mv);
                    using_vbo = FALSE;
                break;
            }

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        }

        if(using_vbo)
        {
            g_print("OK\n");

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_VERTEX]);
            glVertexPointer(3, GL_FLOAT, 0, 0);

            glColor4f(0.6, 0.6, 0.6, 1.0);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_ELEMENT]);
            glDrawElements(GL_LINES, 2*mesh->e_num, mesh->index_gl_type, 0);

            glColor4f(1, 0.1, 0.1, 1);
            glPointSize(3);
            glDrawArrays(GL_POINTS, 0, mesh->v_num);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        }
        else
        {
            g_print("Failed\n");
        }
    }
    else // vertex arrays
    {
        using_vbo = FALSE;
    }

    if( ! using_vbo)
    {
        glVertexPointer(3, GL_FLOAT, 0, mesh->v_coords);

        glColor4f(0.6, 0.6, 0.6, 1.0);
        glDrawElements(GL_LINES, 2*mesh->e_num, mesh->index_gl_type, mesh->e_verts);

        glColor4f(1, 0.1, 0.1, 1);
        glPointSize(3);
        glDrawArrays(GL_POINTS, 0, mesh->v_num);
    }

    glColor3f(0, 1, 0);
    glPointSize(4);

    glBegin(GL_POINTS);
    guint i;
    for(i = 0; i < mesh->v_num; i++)
    {
        if(moto_mesh_selection_is_vertex_selected(selection, i))
        {
            glVertex3fv((GLfloat *)(& mesh->v_coords[i]));
        }
    }
    glEnd();

    glPopAttrib();
    glPopClientAttrib();
}

inline static void draw_mesh_as_edges(MotoMeshViewNode *mv, MotoMesh *mesh, MotoMeshSelection *selection)
{
    MotoMeshViewNodePriv *priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(mv);

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    glPushAttrib(GL_ENABLE_BIT);

    if(moto_gl_is_glsl_supported())
        glUseProgramObjectARB(0);
    glDisable(GL_LIGHTING);
    glColor4f(1, 1, 1, 0.25);

    glEnableClientState(GL_VERTEX_ARRAY);

    gboolean force_arrays = FALSE;
    MotoWorld *world = moto_node_get_world((MotoNode *)mv);
    if(world)
        force_arrays = ! moto_world_get_use_vbo(world);
    if(moto_gl_is_vbo_supported() && ! force_arrays) // vbo
    {
        g_print("Initializng VBO for 'edge' drawing mode ... ");
        if( ! glIsBufferARB(priv->vbufs[VBUF_VERTEX]))
        {
            glGenBuffersARB(VBUF_NUMBER, priv->vbufs);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_VERTEX]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->v_num * sizeof(MotoVector), mesh->v_coords,  GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_LINE_ELEMENT]);
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    moto_mesh_get_index_size(mesh) * mesh->e_num * 2,
                    mesh->e_verts, GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        }
        g_print("OK\n");

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_VERTEX]);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glDisable(GL_DEPTH_TEST);
        glLineWidth(1.0);
        glColor4f(0.9, 0.9, 0.9, 1.0);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_LINE_ELEMENT]);
        glDrawElements(GL_LINES, 2*mesh->e_num, mesh->index_gl_type, 0);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    else // vertex arrays
    {
        glVertexPointer(3, GL_FLOAT, 0, mesh->v_coords);

        glDisable(GL_DEPTH_TEST);
        glLineWidth(1.0);
        glColor4f(0.9, 0.9, 0.9, 1.0);

        glDrawElements(GL_LINES, 2*mesh->e_num, mesh->index_gl_type, mesh->e_verts);
    }

    // Drawing selected edges.
    glBegin(GL_LINES);
    glColor3f(0, 1, 0);
    glLineWidth(2.0);
    guint i;
    for(i = 0; i < mesh->e_num; i++)
    {
        if(mesh->b32)
        {
            guint32 *e_verts = (guint32 *)mesh->e_verts;
            if(moto_mesh_selection_is_edge_selected(selection, i))
            {
                glVertex3fv((GLfloat *)(& mesh->v_coords[e_verts[i*2]]));
                glVertex3fv((GLfloat *)(& mesh->v_coords[e_verts[i*2 + 1]]));
            }
        }
        else
        {
            guint16 *e_verts = (guint16 *)mesh->e_verts;
            if(moto_mesh_selection_is_edge_selected(selection, i))
            {
                glVertex3fv((GLfloat *)(& mesh->v_coords[e_verts[i*2]]));
                glVertex3fv((GLfloat *)(& mesh->v_coords[e_verts[i*2 + 1]]));
            }
        }
    }
    glEnd();

    glPopAttrib();
    glPopClientAttrib();
}

inline static void draw_mesh_as_faces(MotoMeshViewNode *mv, MotoMesh *mesh, MotoMeshSelection *selection)
{
    MotoMeshViewNodePriv *priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(mv);

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glColor3f(1, 1, 1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    gboolean force_arrays = FALSE;
    MotoWorld *world = moto_node_get_world((MotoNode *)mv);
    if(world)
        force_arrays = ! moto_world_get_use_vbo(world);
    if(moto_gl_is_vbo_supported() && ! force_arrays) // vbo
    {
        g_print("Initializng VBO for 'face' drawing mode ... ");
        if( ! glIsBufferARB(priv->vbufs[VBUF_VERTEX]))
        {
            glGenBuffersARB(VBUF_NUMBER, priv->vbufs);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_VERTEX]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->v_num * sizeof(MotoVector), mesh->v_coords,  GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_NORMAL]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->v_num * sizeof(MotoVector), mesh->v_normals, GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_ELEMENT]);
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    moto_mesh_get_index_size(mesh) * mesh->f_num * 3 * 2,
                    mesh->f_tess_verts, GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_LINE_ELEMENT]);
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    moto_mesh_get_index_size(mesh) * mesh->e_num * 2,
                    mesh->e_verts, GL_STATIC_DRAW_ARB);

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        }
        g_print("OK\n");

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_VERTEX]);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_NORMAL]);
        glNormalPointer(GL_FLOAT, 0, 0);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_ELEMENT]);
        glDrawElements(GL_TRIANGLES, 3*mesh->f_tess_num, mesh->index_gl_type, 0);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

    }
    else
    {
        glVertexPointer(3, GL_FLOAT, 0, mesh->v_coords);
        glNormalPointer(GL_FLOAT, 0, mesh->v_normals);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0, 1.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, 3*mesh->f_tess_num, mesh->index_gl_type, mesh->f_tess_verts);
    }

    glPolygonOffset(1.5, 1.0);

    MotoMeshFace16 *f_data = (MotoMeshFace16 *)mesh->f_data;
    guint16 *f_verts = (guint16 *)mesh->f_verts;

    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glColor4f(0, 1, 0, 1);
    guint i, j;
    for(i = 0; i < mesh->f_num; i++)
    {
        if(moto_mesh_selection_is_face_selected(selection, i))
        {
            glBegin(GL_POLYGON);
            guint start = (0 == i) ? 0: f_data[i-1].v_offset;
            guint v_num = f_data[i].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                glVertex3fv((GLfloat *)( & mesh->v_coords[f_verts[start + j]]));
            }
            glEnd();
        }
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glColor3f(0.2, 0.2, 0.2);

    if(moto_gl_is_vbo_supported() && ! force_arrays) // vbo
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_LINE_ELEMENT]);
        glDrawElements(GL_LINES, 2*mesh->e_num, mesh->index_gl_type, 0);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    else
    {
        glDrawElements(GL_LINES, 2*mesh->e_num, mesh->index_gl_type, mesh->e_verts);
        glDisable(GL_POLYGON_OFFSET_LINE);
    }

    glPopAttrib();
    glPopClientAttrib();
}

static void moto_mesh_view_node_draw(MotoGeomViewNode *self)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(self), "mesh"));
    if( ! mesh)
        return;

    MotoWorld* world = moto_node_get_world(MOTO_NODE(self));
    MotoDrawMode draw_mode = moto_world_get_draw_mode(world);
    switch(draw_mode)
    {
        case MOTO_DRAW_MODE_BBOX_WIREFRAME:
            moto_mesh_view_node_draw_bound((MotoMeshViewNode*)self, TRUE);
        break;
        case MOTO_DRAW_MODE_BBOX_SOLID:
            moto_mesh_view_node_draw_bound((MotoMeshViewNode*)self, FALSE);
        break;
        default:
            if( ! moto_geom_view_node_get_prepared(self))
                moto_mesh_view_node_prepare_for_draw(self);
            else
                glCallList(priv->dlist);
        break;
    }
}

static void moto_mesh_view_node_prepare_for_draw(MotoGeomViewNode *self)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);
    moto_mesh_view_node_delete_buffers((MotoMeshViewNode*)self);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(self), "mesh"));
    if( ! mesh)
        return;

    if( ! glIsList(priv->dlist))
        priv->dlist = glGenLists(1);
    glNewList(priv->dlist, GL_COMPILE_AND_EXECUTE);

    MotoGeomViewState *state = moto_geom_view_node_get_state(self);
    if(state)
        moto_geom_view_state_draw(state, self);
    else
        draw_mesh_as_object((MotoMeshViewNode *)self, mesh);

    glEndList();

    moto_geom_view_node_set_prepared(self, TRUE);
}

static gboolean moto_mesh_view_node_select(MotoGeomViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(self), "mesh"));
    if( ! mesh)
        return TRUE;

    if( ! glIsList(priv->dlist))
        priv->dlist = glGenLists(1);

    MotoGeomViewState *state = moto_geom_view_node_get_state((MotoGeomViewNode *)self);
    if(state)
        return moto_geom_view_state_select(state, (MotoGeomViewNode *)self,
                x, y, width, height, ray, tinfo);
    return FALSE;
}

static MotoGeometryNode *moto_mesh_view_node_get_geometry(MotoGeomViewNode *self)
{
    MotoParam *p = moto_node_get_param((MotoNode *)self, "mesh");
    MotoParam *s = moto_param_get_source(p);
    if( ! s)
        return NULL;
    return (MotoGeometryNode *)moto_param_get_node(s);
}

static void moto_mesh_view_node_draw_bound(MotoMeshViewNode *self, gboolean wireframe)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);

    glPushAttrib(GL_ENABLE_BIT);
    glPushMatrix();

    gfloat scale[3];
    moto_bound_get_scale(priv->bound, scale);

    gfloat center[3];
    moto_bound_get_center(priv->bound, center);

    glTranslatef(center[0], center[1], center[2]);
    glScalef(scale[0], scale[1], scale[2]);

    gfloat A = 0.5;

    if(wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_LIGHTING);
    }

    glBegin(GL_QUADS);

    // 0
    glNormal3f(0, 0, 1);
    glVertex3f(A, A, A);
    glVertex3f(A, -A, A);
    glVertex3f(-A, -A, A);
    glVertex3f(-A, A, A);

    // 1
    glNormal3f(0, 0, -1);
    glVertex3f(-A, A, -A);
    glVertex3f(-A, -A, -A);
    glVertex3f(A, -A, -A);
    glVertex3f(A, A, -A);

    // 2
    glNormal3f(0, 1, 0);
    glVertex3f(A, A, A);
    glVertex3f(A, A, -A);
    glVertex3f(-A, A, -A);
    glVertex3f(-A, A, A);

    // 3
    glNormal3f(0, -1, 0);
    glVertex3f(-A, -A, A);
    glVertex3f(-A, -A, -A);
    glVertex3f(A, -A, -A);
    glVertex3f(A, -A, A);

    // 4
    glNormal3f(1, 0, 0);
    glVertex3f(A, A, A);
    glVertex3f(A, A, -A);
    glVertex3f(A, -A, -A);
    glVertex3f(A, -A, A);

    // 5
    glNormal3f(-1, 0, 0);
    glVertex3f(-A, -A, A);
    glVertex3f(-A, -A, -A);
    glVertex3f(-A, A, -A);
    glVertex3f(-A, A, A);

    glEnd();

    glPopMatrix();
    glPopAttrib();
}

/* states */

static void moto_mesh_view_node_draw_as_object(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));

    if( ! mesh)
        return;

    draw_mesh_as_object((MotoMeshViewNode *)geom, mesh);
}

static void moto_mesh_view_node_draw_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    draw_mesh_as_verts((MotoMeshViewNode*)geom, mesh, geom_priv->selection);
}

static gboolean
moto_mesh_view_node_select_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
    {
        // g_print("No mesh\n");
        return FALSE;
    }

    /* Array of intersected verts. */
    GArray *hits = \
        g_array_sized_new(FALSE, FALSE, sizeof(guint), max(64, min(1024, mesh->v_num/10)));

    guint index = 0;
    gfloat dist, dist_tmp;
    dist = MACRO;
    gfloat square_radius = 0.25*0.25;
    gfloat fovy = atan((1/tinfo->proj[5]))*2;

    guint i;
    for(i = 0; i < mesh->v_num; i++)
    {
        gfloat *xyz = (gfloat *)(mesh->v_coords + i);

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

    if(hits->len > 0)
    {
        /* Detecting which of intersected verts is nearest to cursor. */
        GLdouble win_dist,
                 min_win_dist = MACRO;
        GLdouble win_x, win_y, win_z, xx, yy;
        guint i, ii;
        for(i = 0; i < hits->len; i++)
        {
            ii = g_array_index(hits, gint, i);
            gluProject(mesh->v_coords[ii].x, mesh->v_coords[ii].y, mesh->v_coords[ii].z,
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

        MotoMeshSelection *sel = moto_mesh_view_node_get_selection(mv);
        if(sel)
        {
            moto_mesh_selection_toggle_vertex_selection(sel, index);
        }
        moto_geom_view_node_set_prepared((MotoGeomViewNode *)mv, FALSE);
        moto_geom_view_node_draw((MotoGeomViewNode *)mv);
    }

    g_array_free(hits, TRUE);
    return TRUE;
}

static void moto_mesh_view_node_grow_selection_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if(! mesh)
        return;

    moto_mesh_selection_select_more_verts(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_select_less_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    moto_mesh_selection_select_less_verts(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_invert_selection_as_verts(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    moto_mesh_selection_select_inverse_verts(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_draw_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    draw_mesh_as_edges((MotoMeshViewNode *)geom, mesh, geom_priv->selection);
}

static gboolean
moto_mesh_view_node_select_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
    {
        // g_print("No mesh\n");
        return FALSE;
    }

    /* Array of intersected verts. */
    GArray *hits = \
        g_array_sized_new(FALSE, FALSE, sizeof(guint), max(64, min(1024, mesh->v_num/10)));

    guint index = 0;
    gfloat dist, dist_tmp;
    dist = MACRO;
    gfloat square_radius = 0.25*0.25;
    gfloat fovy = atan((1/tinfo->proj[5]))*2;

    guint i;
    for(i = 0; i < mesh->v_num; i++)
    {
        gfloat *xyz = (gfloat *)(mesh->v_coords + i);

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

    if(hits->len > 0)
    {
        /* Detecting which of intersected verts is nearest to cursor. */
        GLdouble win_dist,
                 min_win_dist = MACRO;
        GLdouble win_x, win_y, win_z, xx, yy;
        guint i, ii;
        for(i = 0; i < hits->len; i++)
        {
            ii = g_array_index(hits, gint, i);
            gluProject(mesh->v_coords[ii].x, mesh->v_coords[ii].y, mesh->v_coords[ii].z,
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

        MotoMeshSelection *sel = moto_mesh_view_node_get_selection((MotoMeshViewNode *)geom);
        if(sel)
        {
            moto_mesh_selection_toggle_edge_selection(sel, index);
        }
        moto_geom_view_node_set_prepared((MotoGeomViewNode *)geom, FALSE);
        moto_geom_view_node_draw((MotoGeomViewNode *)geom);
    }

    g_array_free(hits, TRUE);
    return TRUE;
}

static void moto_mesh_view_node_grow_selection_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    moto_mesh_selection_select_more_edges(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_select_less_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    moto_mesh_selection_select_less_edges(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_invert_selection_as_edges(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    moto_mesh_selection_select_inverse_edges(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_draw_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if(! mesh)
        return;

    draw_mesh_as_faces((MotoMeshViewNode *)geom, mesh, geom_priv->selection);
}

static gboolean
moto_mesh_view_node_select_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
    {
        // g_print("No mesh\n");
        return FALSE;
    }


    guint index = moto_mesh_invalid_index(mesh);
    guint num = 0;
    gfloat dist = MACRO;
    gfloat dist_tmp;

    guint i;
    for(i = 0; i < mesh->f_num; i++)
    {
        if( ! moto_mesh_intersect_face(mesh, i, ray, & dist_tmp))
            continue;
        num++;

        if(dist_tmp < dist)
        {
            dist = dist_tmp;
            index = i;
        }
    }

    MotoMeshSelection *sel = moto_mesh_view_node_get_selection(mv);
    if(num > 0 && sel)
    {
        moto_mesh_selection_toggle_face_selection(sel, index);
        moto_geom_view_node_set_prepared((MotoGeomViewNode *)mv, FALSE);
        moto_geom_view_node_draw((MotoGeomViewNode *)mv);
    }

    return TRUE;
}

static void moto_mesh_view_node_grow_selection_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    moto_mesh_selection_select_more_faces(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_select_less_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    moto_mesh_selection_select_less_faces(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_invert_selection_as_faces(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(MOTO_NODE(geom), "mesh"));
    if( ! mesh)
        return;

    moto_mesh_selection_select_inverse_faces(geom_priv->selection, mesh);
}

static void moto_mesh_view_node_update(MotoNode *self)
{
    MotoGeomViewNode *gv = (MotoGeomViewNode *)self;
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);

    moto_mesh_view_node_delete_buffers((MotoMeshViewNode *)self);
    moto_geom_view_node_set_prepared(gv, FALSE);

    MotoMesh *mesh = \
        MOTO_MESH(moto_node_get_param_object(self, "mesh"));
    if( ! mesh)
        return;

    moto_mesh_calc_bound(mesh, priv->bound);

    if( ! priv->selection)
    {
        priv->selection = moto_mesh_selection_new_for_mesh(mesh);

        return;
    }

    if( ! moto_mesh_selection_is_valid(priv->selection, mesh))
    {
        MotoMeshSelection *old = priv->selection;
        priv->selection = moto_mesh_selection_new_for_mesh(mesh);
        moto_mesh_selection_copy_smth(priv->selection, old);

        moto_mesh_selection_free(old);
    }
}

static void moto_mesh_view_node_delete_buffers(MotoMeshViewNode *self)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);

    if(moto_gl_is_vbo_supported())
    {
        glDeleteBuffersARB(VBUF_NUMBER, priv->vbufs);
        bzero(priv->vbufs, VBUF_NUMBER);
    }
}

const MotoBound *moto_mesh_view_node_get_bound(MotoMeshViewNode *self)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);
    return priv->bound;
}

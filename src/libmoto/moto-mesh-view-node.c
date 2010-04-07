#include <stdio.h>
#include <string.h>

#include "libmotoutil/moto-gl.h"
#include "libmotoutil/numdef.h"
#include "libmotoutil/xform.h"

#include "moto-mesh-view-node.h"
#include "moto-enums.h"
#include "moto-mesh.h"
#include "moto-scene-node.h"

typedef enum
{
    VBUF_VERTEX,
    VBUF_NORMAL,
    VBUF_ELEMENT,
    VBUF_LINE_ELEMENT,
    VBUF_NUMBER
} MotoVBufs;

/* forwards */

static void moto_mesh_view_node_draw(MotoShapeViewNode *self);
static void moto_mesh_view_node_prepare_for_draw(MotoShapeViewNode *self);
static gboolean moto_mesh_view_node_select(MotoShapeViewNode *self, gint x, gint y,
        gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
static MotoShapeNode *moto_mesh_view_node_get_shape(MotoShapeViewNode *self);

static void moto_mesh_view_node_draw_bound(MotoMeshViewNode *self, gboolean wireframe);

static void moto_mesh_view_node_draw_as_object(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_draw_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom);
static gboolean
moto_mesh_view_node_select_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
static void moto_mesh_view_node_grow_selection_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_select_less_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_select_less_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_select_less_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom);

static void moto_mesh_view_node_draw_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom);
static gboolean
moto_mesh_view_node_select_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);

static void moto_mesh_view_node_grow_selection_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_grow_selection_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom);

static void moto_mesh_view_node_invert_selection_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_invert_selection_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_invert_selection_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom);

static void moto_mesh_view_node_leave_from_verts(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_leave_from_edges(MotoShapeViewState *self, MotoShapeViewNode *geom);
static void moto_mesh_view_node_leave_from_faces(MotoShapeViewState *self, MotoShapeViewNode *geom);

static void moto_mesh_view_node_draw_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom);
static gboolean
moto_mesh_view_node_select_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);

static void moto_mesh_view_node_update(MotoNode *self);
static void moto_mesh_view_node_delete_buffers(MotoMeshViewNode *self);

/* class MeshViewNode */

typedef struct _MotoMeshViewNodePriv MotoMeshViewNodePriv;

struct _MotoMeshViewNodePriv
{
    gboolean disposed;

    MotoShapeSelection *selection;

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

    glDeleteLists(priv->dlist, 1);
    moto_mesh_view_node_delete_buffers((MotoMeshViewNode *)obj);

    moto_shape_selection_free(priv->selection);
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
            "subdiv", "Subdiv", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, FALSE, NULL, "Shape",
            "subdiv_render", "Subdiv Render", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, FALSE, NULL, "Shape",
            "mesh", "Mesh", MOTO_TYPE_SHAPE, MOTO_PARAM_MODE_IN, NULL, pspec, "Shape",
            "view", "View", MOTO_TYPE_SHAPE_VIEW_NODE, MOTO_PARAM_MODE_OUT, self, pspec, "Shape",
            NULL);

    moto_shape_view_node_set_state((MotoShapeViewNode *)self, "verts");
}

static void
moto_mesh_view_node_class_init(MotoMeshViewNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoMeshViewNodePriv));

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = MOTO_NODE_CLASS(klass);
    MotoShapeViewNodeClass *gvclass = \
        MOTO_SHAPE_VIEW_NODE_CLASS(klass);

    mesh_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_view_node_dispose;
    goclass->finalize   = moto_mesh_view_node_finalize;

    nclass->update = moto_mesh_view_node_update;

    gvclass->draw               = moto_mesh_view_node_draw;
    gvclass->prepare_for_draw   = moto_mesh_view_node_prepare_for_draw;
    gvclass->select             = moto_mesh_view_node_select;
    gvclass->get_shape       = moto_mesh_view_node_get_shape;

    gvclass->states = g_slist_append(gvclass->states,
            moto_shape_view_state_new("object", "Object",
                moto_mesh_view_node_draw_as_object, NULL, NULL, NULL, NULL, NULL));
    gvclass->states = g_slist_append(gvclass->states,
            moto_shape_view_state_new("verts", "Vertex",
                moto_mesh_view_node_draw_as_verts, moto_mesh_view_node_select_as_verts,
                moto_mesh_view_node_grow_selection_as_verts,
                moto_mesh_view_node_select_less_as_verts,
                moto_mesh_view_node_invert_selection_as_verts,
                moto_mesh_view_node_leave_from_verts));
    gvclass->states = g_slist_append(gvclass->states,
            moto_shape_view_state_new("edges", "Edges",
                moto_mesh_view_node_draw_as_edges, moto_mesh_view_node_select_as_edges,
                moto_mesh_view_node_grow_selection_as_edges,
                moto_mesh_view_node_select_less_as_edges,
                moto_mesh_view_node_invert_selection_as_edges,
                moto_mesh_view_node_leave_from_edges));
    gvclass->states = g_slist_append(gvclass->states,
            moto_shape_view_state_new("faces", "Faces",
                moto_mesh_view_node_draw_as_faces, moto_mesh_view_node_select_as_faces,
                moto_mesh_view_node_grow_selection_as_faces,
                moto_mesh_view_node_select_less_as_faces,
                moto_mesh_view_node_invert_selection_as_faces,
                moto_mesh_view_node_leave_from_faces));
}

G_DEFINE_TYPE(MotoMeshViewNode, moto_mesh_view_node, MOTO_TYPE_SHAPE_VIEW_NODE);

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
    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)self, "mesh", (GObject**)&mesh);
    return mesh;
}

MotoShapeSelection *moto_mesh_view_node_get_selection(MotoMeshViewNode *self)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);
    return priv->selection;
}

inline static void draw_mesh_as_object(MotoMeshViewNode *mv, MotoMesh *mesh)
{
    MotoMeshViewNodePriv *priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(mv);

    MotoSceneNode* scene_node = moto_node_get_scene_node((MotoNode*)mv);

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    glPushAttrib(GL_ENABLE_BIT);

    glColor4f(1, 1, 1, 1);

    glEnable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    if(moto_scene_node_get_cull_faces(scene_node))
        glEnable(GL_CULL_FACE);

    gboolean force_arrays = FALSE;
    force_arrays = ! moto_scene_node_get_use_vbo(scene_node);
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
        glVertexPointer(3, GL_FLOAT, sizeof(MotoVector), 0);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_NORMAL]);
        glNormalPointer(GL_FLOAT, sizeof(MotoVector), 0);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, priv->vbufs[VBUF_ELEMENT]);
        glDrawElements(GL_TRIANGLES, 3*mesh->f_tess_num, mesh->index_gl_type, 0);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    else // Vertex arrays
    {
        MotoDrawMode draw_mode = moto_scene_node_get_draw_mode(scene_node);
        if(MOTO_DRAW_MODE_SOLID == draw_mode)
        {
            MotoMeshFace16 *f_data = mesh->f_data16;
            guint16 *f_verts = (guint16 *)mesh->f_verts;

            guint i;
            for(i = 0; i < mesh->f_num; ++i)
            {
                guint start = (0 == i) ? 0: f_data[i-1].v_offset;
                guint v_num = f_data[i].v_offset - start;

                glNormal3fv((GLfloat*)( & mesh->f_normals[i]));
                guint j;
                glBegin(GL_POLYGON);
                for(j = 0; j < v_num; ++j)
                {
                    glVertex3fv((GLfloat *)( & mesh->v_coords[f_verts[start + j]]));
                }
                glEnd();
            }
        }
        else
        {
            if(!moto_scene_node_get_use_arrays(scene_node))
            {
                moto_info("Using simple face cycles");
                MotoMeshFace16 *f_data = mesh->f_data16;
                guint16 *f_verts = (guint16 *)mesh->f_verts;

                guint i;
                for(i = 0; i < mesh->f_num; ++i)
                {
                    guint start = (0 == i) ? 0: f_data[i-1].v_offset;
                    guint v_num = f_data[i].v_offset - start;

                    guint j;
                    glBegin(GL_POLYGON);
                    for(j = 0; j < v_num; ++j)
                    {
                        glNormal3fv((GLfloat*)( & mesh->v_normals[f_verts[start + j]]));
                        glVertex3fv((GLfloat*)( & mesh->v_coords[f_verts[start + j]]));
                    }
                    glEnd();
                }
            }
            else
            {
                moto_info("Using vertex arrays with glDrawElements");
                glVertexPointer(3, GL_FLOAT, sizeof(MotoVector), mesh->v_coords);
                glNormalPointer(GL_FLOAT, sizeof(MotoVector), mesh->v_normals);
                glDrawElements(GL_TRIANGLES, 3*mesh->f_tess_num, mesh->index_gl_type, mesh->f_tess_verts);
            }
        }
    }

    glPopAttrib();
    glPopClientAttrib();
}

inline static void draw_mesh_as_verts(MotoMeshViewNode *mv, MotoMesh *mesh, MotoShapeSelection *selection)
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
    MotoSceneNode *scene_node = moto_node_get_scene_node((MotoNode *)mv);
    if(scene_node)
        force_arrays = ! moto_scene_node_get_use_vbo(scene_node);

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
            glVertexPointer(3, GL_FLOAT, sizeof(MotoVector), 0);

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
        glVertexPointer(3, GL_FLOAT, sizeof(MotoVector), mesh->v_coords);

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
        if(moto_shape_selection_is_vertex_selected(selection, i))
        {
            glVertex3fv((GLfloat *)(& mesh->v_coords[i]));
        }
    }
    glEnd();

    glPopAttrib();
    glPopClientAttrib();
}

inline static void draw_mesh_as_edges(MotoMeshViewNode *mv, MotoMesh *mesh, MotoShapeSelection *selection)
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
    MotoSceneNode *scene_node = moto_node_get_scene_node((MotoNode *)mv);
    if(scene_node)
        force_arrays = ! moto_scene_node_get_use_vbo(scene_node);
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
        glVertexPointer(3, GL_FLOAT, sizeof(MotoVector), 0);

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
        glVertexPointer(3, GL_FLOAT, sizeof(MotoVector), mesh->v_coords);

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
            if(moto_shape_selection_is_edge_selected(selection, i))
            {
                glVertex3fv((GLfloat *)(& mesh->v_coords[e_verts[i*2]]));
                glVertex3fv((GLfloat *)(& mesh->v_coords[e_verts[i*2 + 1]]));
            }
        }
        else
        {
            guint16 *e_verts = (guint16 *)mesh->e_verts;
            if(moto_shape_selection_is_edge_selected(selection, i))
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

static void tessCallbackVertex(void* vertex_data)
{
    glVertex3dv(vertex_data);
}

static void tessCallbackVertexNormal(void* vertex_data)
{
    glNormal3dv(((GLdouble*)vertex_data)+3);
    glVertex3dv(vertex_data);
}

static void tessCallbackBegin(GLenum which)
{
    glBegin(which);
}

static void tessCallbackEnd(void)
{
    glEnd();
}

static void tessCallbackError(GLenum errno)
{
    g_print("Tesselation error: %d\n", errno);
}

static unsigned long stipple_mask[] = {
  0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
  0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
  0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
  0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
  0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
  0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
  0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
  0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
};

inline static void draw_mesh_as_faces(MotoMeshViewNode *mv, MotoMesh *mesh, MotoShapeSelection *selection)
{
    MotoMeshViewNodePriv *priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(mv);
    MotoSceneNode *scene_node = moto_node_get_scene_node((MotoNode *)mv);

    glPushAttrib(GL_ENABLE_BIT);
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
    glColor3f(1, 1, 1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(MotoVector), mesh->v_coords);

    glDisable(GL_CULL_FACE);
    if(moto_scene_node_get_cull_faces(scene_node))
        glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    MotoMeshFace16 *f_data = (MotoMeshFace16 *)mesh->f_data;
    guint16 *f_verts = (guint16 *)mesh->f_verts;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2, 1);

    GLUtesselator* tess = gluNewTess();

    gluTessCallback(tess, GLU_TESS_VERTEX, tessCallbackVertex);
    gluTessCallback(tess, GLU_TESS_BEGIN, tessCallbackBegin);
    gluTessCallback(tess, GLU_TESS_END, tessCallbackEnd);
    gluTessCallback(tess, GLU_TESS_ERROR, tessCallbackError);
    // gluTessCallback(tess, GLU_TESS_COMBINE, tessCallbackCombine);

    gboolean show_normals = moto_scene_node_get_show_normals(scene_node);
    MotoDrawMode draw_mode = moto_scene_node_get_draw_mode(scene_node);

    glEnable(GL_LIGHTING);

    guint i, j;
    if(MOTO_DRAW_MODE_SOLID == draw_mode)
    {
        gluTessCallback(tess, GLU_TESS_VERTEX, tessCallbackVertex);
        for(i = 0; i < mesh->f_num; ++i)
        {
            guint start = (0 == i) ? 0: f_data[i-1].v_offset;
            guint v_num = f_data[i].v_offset - start;

            glColor4f(1, 1, 1, 1);

            if(!moto_shape_selection_is_face_selected(selection, i))
            {
                GLdouble verts[v_num*3];

                glNormal3fv((GLfloat*)( & mesh->f_normals[i]));
                gluTessBeginPolygon(tess, NULL);
                gluTessBeginContour(tess);
                guint j;
                for(j = 0; j < v_num; ++j)
                {
                    float* v0 = (float*) & mesh->v_coords[f_verts[start + j]];
                    float* n0 = (float*) & mesh->f_normals[i];
                    GLdouble* v = verts + j*3;
                    v[0] = v0[0];
                    v[1] = v0[1];
                    v[2] = v0[2];
                    gluTessVertex(tess, v, v);
                }
                gluTessEndContour(tess);
                gluTessEndPolygon(tess);
            }

            /*
            if(show_normals)
            {
                gfloat ox = 0;
                gfloat oy = 0;
                gfloat oz = 0;

                for(j = 0; j < v_num; ++j)
                {
                    ox += mesh->v_coords[f_verts[start + j]].x;
                    oy += mesh->v_coords[f_verts[start + j]].y;
                    oz += mesh->v_coords[f_verts[start + j]].z;
                }
                ox /= v_num;
                oy /= v_num;
                oz /= v_num;

                gfloat nx = ox + mesh->f_normals[i].x;
                gfloat ny = oy + mesh->f_normals[i].y;
                gfloat nz = oz + mesh->f_normals[i].z;

                glColor3f(1, 1, 0);
                glBegin(GL_LINES);
                glVertex3f(ox, oy, oz);
                glVertex3f(nx, ny, nz);
                glEnd();
            }
            */
        }
    }
    else if(MOTO_DRAW_MODE_WIREFRAME == draw_mode)
    {
        glEnable(GL_POLYGON_STIPPLE);
        glPolygonStipple(stipple_mask);
    }
    else
    {
        gluTessCallback(tess, GLU_TESS_VERTEX, tessCallbackVertexNormal);
        for(i = 0; i < mesh->f_num; ++i)
        {
            guint start = (0 == i) ? 0: f_data[i-1].v_offset;
            guint v_num = f_data[i].v_offset - start;

            glColor4f(1, 1, 1, 1);

            if(!moto_shape_selection_is_face_selected(selection, i))
            {
                GLdouble verts[v_num*6];

                gluTessBeginPolygon(tess, NULL);
                gluTessBeginContour(tess);
                guint j;
                for(j = 0; j < v_num; ++j)
                {
                    guint vi = f_verts[start + j];
                    float* v0 = (float*) & mesh->v_coords[vi];
                    float* n0 = (float*) & mesh->v_normals[vi];
                    GLdouble* v = verts + j*6;
                    v[0] = v0[0];
                    v[1] = v0[1];
                    v[2] = v0[2];
                    v[3] = n0[0];
                    v[4] = n0[1];
                    v[5] = n0[2];
                    gluTessVertex(tess, v, v);
                }
                gluTessEndContour(tess);
                gluTessEndPolygon(tess);
            }
        }
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    for(i = 0; i < mesh->f_num; ++i)
    {
        guint start = (0 == i) ? 0: f_data[i-1].v_offset;
        guint v_num = f_data[i].v_offset - start;

        glColor4f(0, 1, 0, 1);

        if(moto_shape_selection_is_face_selected(selection, i))
        {
            glBegin(GL_POLYGON);
            for(j = 0; j < v_num; j++)
            {
                glVertex3fv((GLfloat *)( & mesh->v_coords[f_verts[start + j]]));
            }
            glEnd();
        }
    }

    gluDeleteTess(tess);

    glDisable(GL_POLYGON_OFFSET_FILL);

    glColor3f(0.2, 0.2, 0.2);

    glDrawElements(GL_LINES, 2*mesh->e_num, mesh->index_gl_type, mesh->e_verts);

    glPopAttrib();
    glPopClientAttrib();
}

static void moto_mesh_view_node_draw(MotoShapeViewNode *self)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)self, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    glShadeModel(GL_SMOOTH);

    MotoSceneNode* scene_node = moto_node_get_scene_node(MOTO_NODE(self));
    MotoDrawMode draw_mode = moto_scene_node_get_draw_mode(scene_node);
    switch(draw_mode)
    {
        case MOTO_DRAW_MODE_BBOX_WIREFRAME:
            moto_mesh_view_node_draw_bound((MotoMeshViewNode*)self, TRUE);
        break;
        case MOTO_DRAW_MODE_BBOX_SOLID:
            moto_mesh_view_node_draw_bound((MotoMeshViewNode*)self, FALSE);
        break;
        default:
            if( ! moto_shape_view_node_get_prepared(self))
                moto_mesh_view_node_prepare_for_draw(self);
            else
                glCallList(priv->dlist);
        break;
    }
}

static void moto_mesh_view_node_prepare_for_draw(MotoShapeViewNode *self)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);
    moto_mesh_view_node_delete_buffers((MotoMeshViewNode*)self);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)self, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    if( ! glIsList(priv->dlist))
        priv->dlist = glGenLists(1);
    glNewList(priv->dlist, GL_COMPILE_AND_EXECUTE);

    MotoShapeViewState *state = moto_shape_view_node_get_state(self);
    if(state)
        moto_shape_view_state_draw(state, self);
    else
        draw_mesh_as_object((MotoMeshViewNode *)self, mesh);

    glEndList();

    moto_shape_view_node_set_prepared(self, TRUE);
}

static gboolean moto_mesh_view_node_select(MotoShapeViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)self, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return TRUE;

    MotoShapeViewState *state = moto_shape_view_node_get_state((MotoShapeViewNode *)self);
    if(state)
        return moto_shape_view_state_select(state, (MotoShapeViewNode *)self,
                x, y, width, height, ray, tinfo);
    return FALSE;
}

static MotoShapeNode *moto_mesh_view_node_get_shape(MotoShapeViewNode *self)
{
    MotoParam *p = moto_node_get_param((MotoNode *)self, "mesh");
    MotoParam *s = moto_param_get_source(p);
    if( ! s)
        return NULL;
    return (MotoShapeNode *)moto_param_get_node(s);
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

static void moto_mesh_view_node_draw_as_object(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    draw_mesh_as_object((MotoMeshViewNode *)geom, mesh);
}

static void moto_mesh_view_node_draw_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    draw_mesh_as_verts((MotoMeshViewNode*)geom, mesh, geom_priv->selection);
}

static gboolean
moto_mesh_view_node_select_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
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

        MotoShapeSelection *sel = moto_mesh_view_node_get_selection(mv);
        if(sel)
        {
            moto_shape_selection_toggle_vertex_selection(sel, index);
        }
        moto_shape_view_node_set_prepared((MotoShapeViewNode *)mv, FALSE);
        moto_shape_view_node_draw((MotoShapeViewNode *)mv);
    }

    g_array_free(hits, TRUE);
    return TRUE;
}

static void moto_mesh_view_node_grow_selection_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if(! mesh)
        return;

    moto_mesh_select_more_verts(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_select_less_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_select_less_verts(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_invert_selection_as_verts(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_select_inverse_verts(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_leave_from_verts(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_update_selection_from_verts(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_leave_from_edges(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_update_selection_from_edges(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_leave_from_faces(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_update_selection_from_faces(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_draw_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    draw_mesh_as_edges((MotoMeshViewNode *)geom, mesh, geom_priv->selection);
}

/*
static gboolean
moto_mesh_view_node_select_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
    {
        // g_print("No mesh\n");
        return FALSE;
    }

    // Array of intersected verts.
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

        // perspective compensatioin for sphere radius
        gfloat p2v[3]; // Vector from ray origin to vertex.
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
        // Detecting which of intersected verts is nearest to cursor.
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

        MotoShapeSelection *sel = moto_mesh_view_node_get_selection((MotoMeshViewNode *)geom);
        if(sel)
        {
            moto_shape_selection_toggle_edge_selection(sel, index);
        }
        moto_shape_view_node_set_prepared((MotoShapeViewNode *)geom, FALSE);
        moto_shape_view_node_draw((MotoShapeViewNode *)geom);
    }

    g_array_free(hits, TRUE);
    return TRUE;
}
*/

static gboolean
moto_mesh_view_node_select_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
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

        MotoShapeSelection *sel = moto_mesh_view_node_get_selection(mv);
        if(sel)
        {
            moto_shape_selection_toggle_vertex_selection(sel, index);
        }
        moto_shape_view_node_set_prepared((MotoShapeViewNode *)mv, FALSE);
        moto_shape_view_node_draw((MotoShapeViewNode *)mv);
    }

    MotoShapeSelection *sel = moto_mesh_view_node_get_selection(mv);
    if(sel)
        moto_mesh_update_selection_from_verts(mesh, sel);

    g_array_free(hits, TRUE);
    return TRUE;
}

static void moto_mesh_view_node_grow_selection_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_select_more_edges(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_select_less_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_select_less_edges(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_invert_selection_as_edges(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_select_inverse_edges(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_draw_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if(! mesh)
        return;

    draw_mesh_as_faces((MotoMeshViewNode *)geom, mesh, geom_priv->selection);
}

static gboolean
moto_mesh_view_node_select_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoMeshViewNode *mv = (MotoMeshViewNode *)geom;

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
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

    MotoShapeSelection *sel = moto_mesh_view_node_get_selection(mv);
    if(num > 0 && sel)
    {
        moto_shape_selection_toggle_face_selection(sel, index);
        moto_shape_view_node_set_prepared((MotoShapeViewNode *)mv, FALSE);
        moto_shape_view_node_draw((MotoShapeViewNode *)mv);
    }

    return TRUE;
}

static void moto_mesh_view_node_grow_selection_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_select_more_faces(self, geom_priv->selection);
}

static void moto_mesh_view_node_select_less_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_select_less_faces(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_invert_selection_as_faces(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    MotoMeshViewNodePriv* geom_priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(geom);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)geom, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_select_inverse_faces(mesh, geom_priv->selection);
}

static void moto_mesh_view_node_update(MotoNode *self)
{
    MotoShapeViewNode *gv = (MotoShapeViewNode *)self;
    MotoMeshViewNodePriv* priv = MOTO_MESH_VIEW_NODE_GET_PRIVATE(self);

    moto_mesh_view_node_delete_buffers((MotoMeshViewNode *)self);
    moto_shape_view_node_set_prepared(gv, FALSE);

    MotoMesh *mesh;
    moto_node_get_param_object((MotoNode*)self, "mesh", (GObject**)&mesh);
    if( ! mesh)
        return;

    moto_mesh_calc_bound(mesh, priv->bound);

    if( ! priv->selection)
    {
        priv->selection = moto_mesh_create_selection(mesh);

        return;
    }

    if(!moto_mesh_is_selection_valid(mesh, priv->selection))
    {
        MotoShapeSelection *old = priv->selection;
        priv->selection = moto_mesh_adapt_selection(mesh, old);
        moto_shape_selection_free(old);
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

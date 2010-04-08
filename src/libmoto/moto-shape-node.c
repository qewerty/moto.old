#include <string.h>
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/numdef.h"
#include "libmotoutil/xform.h"

#include "moto-scene-node.h"
#include "moto-shape-node.h"
#include "moto-shape.h"
#include "moto-mesh.h"

static MotoBound*
moto_shape_node_get_bound_DEFAULT(MotoShapeNode* self);

void moto_shape_node_draw_DEFUALT(MotoShapeNode* self, MotoDrawMode draw_mode,
    MotoShapeSelection* selection, MotoSelectionMode selection_mode);
static void
moto_shape_node_select_more_DEFAULT(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
static void
moto_shape_node_select_less_DEFAULT(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
static void
moto_shape_node_select_inverse_DEFAULT(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);

/* class ShapeNode */

typedef enum
{
    VBUF_VERTEX,
    VBUF_NORMAL,
    VBUF_ELEMENT,
    VBUF_LINE_ELEMENT,
    VBUF_NUMBER
} MotoVBufs;

typedef struct _MotoShapeNodePriv MotoShapeNodePriv;

#define MOTO_SHAPE_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_SHAPE_NODE, MotoShapeNodePriv)

static GObjectClass *shape_node_parent_class = NULL;

struct _MotoShapeNodePriv
{
    gboolean prepared;

    MotoBound* bound;

    GLuint dlist;
    GLuint vbufs[VBUF_NUMBER];
};

static void
moto_shape_node_dispose(GObject *obj)
{
    MotoShapeNodePriv *priv = MOTO_SHAPE_NODE_GET_PRIVATE(obj);

    g_object_unref(priv->bound);

    shape_node_parent_class->dispose(obj);
}

static void
moto_shape_node_finalize(GObject *obj)
{
    shape_node_parent_class->finalize(obj);
}

static void
moto_shape_node_init(MotoShapeNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoShapeNodePriv *priv = MOTO_SHAPE_NODE_GET_PRIVATE(self);

    priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);

    moto_node_add_params(node,
            "active", "Active", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "visible", "Visible",   MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "lock", "Lock", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, NULL, "Status",
            "out", "Output Shape", MOTO_TYPE_SHAPE, MOTO_PARAM_MODE_OUT,   NULL, NULL, "Shape",
            NULL);

    priv->prepared = FALSE;
}

static void
moto_shape_node_class_init(MotoShapeNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    shape_node_parent_class = (GObjectClass *)(g_type_class_peek_parent(klass));

    goclass->dispose = moto_shape_node_dispose;
    goclass->finalize = moto_shape_node_finalize;

    klass->get_bound = moto_shape_node_get_bound_DEFAULT;
    klass->draw = moto_shape_node_draw_DEFUALT;
    klass->select_more = moto_shape_node_select_more_DEFAULT;
    klass->select_less = moto_shape_node_select_less_DEFAULT;
    klass->select_inverse = moto_shape_node_select_inverse_DEFAULT;

    g_type_class_add_private(klass, sizeof(MotoShapeNodePriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoShapeNode, moto_shape_node, MOTO_TYPE_NODE);

/* Methods of class ShapeNode */

MotoBound *moto_shape_node_get_bound(MotoShapeNode *self)
{
    MotoShapeNodeClass *klass = MOTO_SHAPE_NODE_GET_CLASS(self);

    if(klass->get_bound)
        return klass->get_bound(self);

    return NULL;
}

MotoShape* moto_shape_node_get_shape(MotoShapeNode* self)
{
    GObject* obj = NULL;
    moto_node_get_param_object((MotoNode*)self, "out", &obj);
    return (MotoShape*)obj;
}

void moto_shape_node_draw(MotoShapeNode* self, MotoDrawMode draw_mode,
    MotoShapeSelection* selection, MotoSelectionMode selection_mode)
{
    MOTO_SHAPE_NODE_GET_CLASS(self)->draw(self, draw_mode, selection, selection_mode);
}

static void moto_shape_node_delete_buffers(MotoShapeNode *self)
{
    MotoShapeNodePriv* priv = MOTO_SHAPE_NODE_GET_PRIVATE(self);

    if(moto_gl_is_vbo_supported())
    {
        glDeleteBuffersARB(VBUF_NUMBER, priv->vbufs);
        memset(priv->vbufs, 0, VBUF_NUMBER);
    }
}

static void moto_shape_node_draw_BBOX_WIREFRAME_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_WIREFRAME_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_WIREFRAME_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_WIREFRAME_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_SOLID_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_SOLID_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_SOLID_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_BBOX_SOLID_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{
    MotoShapeNodePriv *priv = MOTO_SHAPE_NODE_GET_PRIVATE(self);

    MotoSceneNode* scene_node = \
        moto_node_get_scene_node((MotoNode*)self);

    MotoShape* shape = moto_shape_node_get_shape(self);
    if(MOTO_IS_MESH(shape))
    {
        MotoMesh* mesh = (MotoMesh*)shape;

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
}

static void moto_shape_node_draw_WIREFRAME_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{
    MotoShapeNodePriv *priv = MOTO_SHAPE_NODE_GET_PRIVATE(self);

    MotoSceneNode *scene_node = \
        moto_node_get_scene_node((MotoNode *)self);

    MotoShape* shape = moto_shape_node_get_shape(self);
    if(MOTO_IS_MESH(shape))
    {
        MotoMesh* mesh = (MotoMesh*)shape;

        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
        glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

        if(moto_gl_is_glsl_supported())
            glUseProgramObjectARB(0);
        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDepthFunc(GL_LEQUAL);

        gboolean force_arrays = FALSE;
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
                        moto_shape_node_delete_buffers(self);
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
                        moto_shape_node_delete_buffers(self);
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
}

static void moto_shape_node_draw_WIREFRAME_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{
    MotoShapeNodePriv *priv = MOTO_SHAPE_NODE_GET_PRIVATE(self);

    MotoSceneNode* scene_node = \
        moto_node_get_scene_node((MotoNode*)self);

    MotoShape* shape = moto_shape_node_get_shape(self);
    if(MOTO_IS_MESH(shape))
    {
        MotoMesh* mesh = (MotoMesh*)shape;

        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
        glPushAttrib(GL_ENABLE_BIT);

        if(moto_gl_is_glsl_supported())
            glUseProgramObjectARB(0);
        glDisable(GL_LIGHTING);
        glColor4f(1, 1, 1, 0.25);

        glEnableClientState(GL_VERTEX_ARRAY);

        gboolean force_arrays = FALSE;
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

static void moto_shape_node_draw_WIREFRAME_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{
    MotoShapeNodePriv *priv = MOTO_SHAPE_NODE_GET_PRIVATE(self);

    MotoSceneNode *scene_node = \
        moto_node_get_scene_node((MotoNode *)self);

    MotoShape* shape = moto_shape_node_get_shape(self);
    if(MOTO_IS_MESH(shape))
    {
        MotoMesh* mesh = (MotoMesh*)shape;

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
}

static void moto_shape_node_draw_WIREFRAME_TEX_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_TEX_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_TEX_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_WIREFRAME_TEX_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_TEX_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_TEX_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_TEX_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SOLID_TEX_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_TEX_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_TEX_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_TEX_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SMOOTH_TEX_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SHADED_OBJECT(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SHADED_VERTEX(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SHADED_EDGE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

static void moto_shape_node_draw_SHADED_FACE(MotoShapeNode* self, MotoShapeSelection* selection)
{}

void moto_shape_node_draw_DEFUALT(MotoShapeNode* self, MotoDrawMode draw_mode,
    MotoShapeSelection* selection, MotoSelectionMode selection_mode)
{
    switch(draw_mode)
    {
        case MOTO_DRAW_MODE_BBOX_WIREFRAME:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_BBOX_WIREFRAME_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_BBOX_WIREFRAME_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_BBOX_WIREFRAME_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_BBOX_WIREFRAME_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_BBOX_SOLID:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_BBOX_SOLID_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_BBOX_SOLID_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_BBOX_SOLID_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_BBOX_SOLID_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_WIREFRAME:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_WIREFRAME_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_WIREFRAME_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_WIREFRAME_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_WIREFRAME_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_WIREFRAME_TEX:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_WIREFRAME_TEX_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_WIREFRAME_TEX_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_WIREFRAME_TEX_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_WIREFRAME_TEX_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SOLID:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SOLID_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SOLID_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SOLID_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SOLID_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SOLID_TEX:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SOLID_TEX_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SOLID_TEX_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SOLID_TEX_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SOLID_TEX_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SMOOTH:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SMOOTH_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SMOOTH_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SMOOTH_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SMOOTH_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SMOOTH_TEX:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SMOOTH_TEX_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SMOOTH_TEX_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SMOOTH_TEX_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SMOOTH_TEX_FACE(self, selection);
                break;
            }
        }
        break;
        case MOTO_DRAW_MODE_SHADED:
        {
            switch(selection_mode)
            {
                case MOTO_SELECTION_MODE_OBJECT:
                    moto_shape_node_draw_SHADED_OBJECT(self, selection);
                break;
                case MOTO_SELECTION_MODE_VERTEX:
                    moto_shape_node_draw_SHADED_VERTEX(self, selection);
                break;
                case MOTO_SELECTION_MODE_EDGE:
                    moto_shape_node_draw_SHADED_EDGE(self, selection);
                break;
                case MOTO_SELECTION_MODE_FACE:
                    moto_shape_node_draw_SHADED_FACE(self, selection);
                break;
            }
        }
        break;
        default:
        break;
    }
}

void moto_shape_node_select_more(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MOTO_SHAPE_NODE_GET_CLASS(self)->select_more(self, selection, mode);
}

static void
moto_shape_node_select_more_DEFAULT(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MotoShape* shape = moto_shape_node_get_shape(self);
    if(shape)
        moto_shape_select_more(shape, selection, mode);
}

void moto_shape_node_select_less(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MOTO_SHAPE_NODE_GET_CLASS(self)->select_less(self, selection, mode);
}

static void
moto_shape_node_select_less_DEFAULT(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MotoShape* shape = moto_shape_node_get_shape(self);
    if(shape)
        moto_shape_select_less(shape, selection, mode);
}

void moto_shape_node_select_inverse(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MOTO_SHAPE_NODE_GET_CLASS(self)->select_inverse(self, selection, mode);
}

static void
moto_shape_node_select_inverse_DEFAULT(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MotoShape* shape = moto_shape_node_get_shape(self);
    if(shape)
        moto_shape_select_inverse(shape, selection, mode);
}

static gboolean
moto_shape_node_select_VERTEX(MotoShapeNode *self,
        MotoShapeSelection* selection,
        gint x, gint y, gint width, gint height,
        MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoShape* shape = moto_shape_node_get_shape(self);

    if(!MOTO_IS_MESH(shape))
        return FALSE;

    MotoMesh* mesh = (MotoMesh*)shape;

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

        moto_shape_selection_toggle_vertex_selection(selection, index);
        // moto_shape_node_set_prepared((MotoShapeViewNode *)shape_node, FALSE);
        // moto_shape_node_draw(shape_node);
    }

    g_array_free(hits, TRUE);
    return TRUE;
}

static gboolean
moto_shape_node_select_EDGE(MotoShapeNode *self,
        MotoShapeSelection* selection,
        gint x, gint y, gint width, gint height,
        MotoRay *ray, MotoTransformInfo *tinfo)
{
}

static gboolean
moto_shape_node_select_FACE(MotoShapeNode *self,
        MotoShapeSelection* selection,
        gint x, gint y, gint width, gint height,
        MotoRay *ray, MotoTransformInfo *tinfo)
{}

gboolean
moto_shape_node_select(MotoShapeNode *self,
        MotoShapeSelection* selection, MotoSelectionMode mode,
        gint x, gint y, gint width, gint height,
        MotoRay *ray, MotoTransformInfo *tinfo)
{
    switch(mode)
    {
        case MOTO_SELECTION_MODE_OBJECT:
        break;
        case MOTO_SELECTION_MODE_VERTEX:
            return moto_shape_node_select_VERTEX(self, selection,
                x, y, width, height, ray, tinfo);
        break;
        case MOTO_SELECTION_MODE_EDGE:
            return moto_shape_node_select_EDGE(self, selection,
                x, y, width, height, ray, tinfo);
        break;
        case MOTO_SELECTION_MODE_FACE:
            return moto_shape_node_select_FACE(self, selection,
                x, y, width, height, ray, tinfo);
        break;
        default:
        break;
    }

    return FALSE;
}

static MotoBound*
moto_shape_node_get_bound_DEFAULT(MotoShapeNode* self)
{
    MotoShape* shape = moto_shape_node_get_shape(self);
    if(shape)
        return moto_shape_get_bound(shape);
    return MOTO_SHAPE_NODE_GET_PRIVATE(self)->bound;
}

#include <string.h>

#ifdef __SSE__
#include <xmmintrin.h>
#endif

#include "moto-mesh.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-edge-list.h"
#include "moto-messager.h"
#include "libmotoutil/xform.h"

#ifndef CALLBACK
#define CALLBACK
#endif

/* forwards */

static MotoBound* moto_mesh_update_bound(MotoShape* self);

static void moto_mesh_copyable_init(MotoCopyableIface *iface);
static void moto_mesh_pointcloud_init(MotoPointCloudIface *iface);

gboolean moto_mesh_prepare(MotoMesh *self);
gboolean moto_mesh_is_struct_the_same(MotoMesh *self, MotoMesh *other);

static void moto_mesh_select_more(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
static void moto_mesh_select_less(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
static void moto_mesh_select_inverse(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);

/* MotoMesh */

static GObjectClass *mesh_parent_class = NULL;

static void
free_attr(GQuark key_id, gpointer data, gpointer user_data)
{
    MotoMeshVertAttr *attr = (MotoMeshVertAttr *)data;

    g_free(attr->data);
    g_slice_free(MotoMeshVertAttr, attr);
}

static void
moto_mesh_dispose(GObject *obj)
{
    MotoMesh *self = (MotoMesh *)obj;

    // Free verts
    g_free(self->v_data);
#ifdef __SSE__
    _mm_free(self->v_coords);
    _mm_free(self->v_normals);
#else
    g_free(self->v_coords);
    g_free(self->v_normals);
#endif
    g_datalist_foreach(& self->v_attrs, free_attr, NULL);
    g_datalist_clear(& self->v_attrs);

    // Free edges
    g_free(self->e_verts);
    g_free(self->e_hard_flags);
    g_free(self->e_creases);

    // Free faces
    g_free(self->f_data);
    g_free(self->f_verts);
    g_free(self->f_tess_verts);
    g_free(self->f_normals);
    g_free(self->f_hidden_flags);

    // Free half-edge data
    g_free(self->he_data);

    G_OBJECT_CLASS(mesh_parent_class)->dispose(obj);
}

static void
moto_mesh_finalize(GObject *obj)
{
    mesh_parent_class->finalize(obj);
}

static void
moto_mesh_init(MotoMesh *self)
{
    self->v_num     = 0;
    self->v_data    = NULL;
    self->v_coords  = NULL;
    self->v_normals = NULL;
    g_datalist_init(& self->v_attrs);

    self->e_num     = 0;
    self->e_verts   = NULL;
    self->e_hard_flags = NULL;
    self->e_use_creases = FALSE;
    self->e_creases = NULL;

    self->f_num     = 0;
    self->f_tess_num = 0;
    self->f_data    = NULL;
    self->f_verts   = NULL;
    self->tesselated = FALSE;
    self->f_tess_verts   = NULL;
    self->f_normals = NULL;
    self->f_use_hidden = FALSE;
    self->f_hidden_flags = NULL;

    self->he_calculated = FALSE;
    self->he_data = NULL;
}

static void
moto_mesh_class_init(MotoMeshClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    mesh_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_dispose;
    goclass->finalize   = moto_mesh_finalize;

    MotoShapeClass *shape_class = (MotoShapeClass*)klass;

    shape_class->update_bound = moto_mesh_update_bound;
    shape_class->prepare = (MotoShapePrepareMethod)moto_mesh_prepare;
    shape_class->is_struct_the_same = (MotoShapeIsStructTheSameMethod)moto_mesh_is_struct_the_same;
    shape_class->select_more = moto_mesh_select_more;
    shape_class->select_less = moto_mesh_select_less;
    shape_class->select_inverse = moto_mesh_select_inverse;
}

// TODO: Remove interfaces.
G_DEFINE_TYPE_WITH_CODE(MotoMesh, moto_mesh, MOTO_TYPE_SHAPE,
                        G_IMPLEMENT_INTERFACE(MOTO_TYPE_COPYABLE,
                                              moto_mesh_copyable_init);
                        G_IMPLEMENT_INTERFACE(MOTO_TYPE_POINTCLOUD,
                                              moto_mesh_pointcloud_init));

/* Methods of class Mesh */

MotoMesh *moto_mesh_new(guint v_num, guint e_num, guint f_num, guint f_verts_num)
{
    if( ! v_num || ! f_num)
        return NULL;

    if( ! e_num) // Mesh without edges
        ;//

    MotoMesh *self = (MotoMesh *)g_object_new(MOTO_TYPE_MESH, NULL);

    guint he_num = (e_num) ? e_num*2 : v_num*2 /* assumption for edgeless mesh */;
    self->b32 = max(he_num, max(v_num, f_num)) > (G_MAXUINT16 - 1);
    self->index_gl_type = (self->b32) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;

    guint num, i;

    self->v_num = v_num;
    self->v_data    = (self->b32) ? g_try_malloc(sizeof(MotoMeshVert32) * v_num):
                                    g_try_malloc(sizeof(MotoMeshVert16) * v_num);
    if( ! self->v_data)
        ; // TODO
#ifdef __SSE__
    self->v_coords  = (MotoVector *)_mm_malloc(sizeof(MotoVector) * v_num, 16);
#else
    self->v_coords  = (MotoVector *)g_try_malloc(sizeof(MotoVector) * v_num);
#endif
    if( ! self->v_coords)
        ; // TODO
#ifdef __SSE__
    self->v_normals = (MotoVector *)_mm_malloc(sizeof(MotoVector) * v_num, 16);
#else
    self->v_normals = (MotoVector *)g_try_malloc(sizeof(MotoVector) * v_num);
#endif
    if( ! self->v_normals)
        ; // TODO

    num = f_num/32 + 1;
    self->f_num     = f_num;
    self->f_v_num   = f_verts_num;
    self->f_verts   = (self->b32) ? g_try_malloc(sizeof(guint32) * f_verts_num):
                                    g_try_malloc(sizeof(guint16) * f_verts_num);
    if( ! self->f_verts)
        ; // TODO
    self->f_data    = (self->b32) ? g_try_malloc(sizeof(MotoMeshFace32) * f_num):
                                    g_try_malloc(sizeof(MotoMeshFace16) * f_num);
    if( ! self->f_data)
        ; // TODO
    self->f_normals = (MotoVector *)g_try_malloc(sizeof(MotoVector) * f_num);
    if( ! self->f_normals)
        ; // TODO
    /*
    self->f_use_hidden = TRUE;
    self->f_hidden_flags  = (guint32 *)g_try_malloc(sizeof(guint32) * num);
    for(i = 0; i < num; i++)
        self->f_hidden_flags[i] = 0;
    */

    self->f_tess_verts = NULL;

    if(self->b32)
    {
        for(i = 0; i < v_num; i++)
            self->v_data32[i].half_edge = G_MAXUINT32;
        for(i = 0; i < f_num; i++)
            self->f_data32[i].half_edge = G_MAXUINT32;
    }
    else
    {
        for(i = 0; i < v_num; i++)
            self->v_data16[i].half_edge = G_MAXUINT16;
        for(i = 0; i < f_num; i++)
            self->f_data16[i].half_edge = G_MAXUINT16;
    }

    if(e_num)
    {
        num = e_num/32 + 1;
        self->e_num     = e_num;
        self->e_verts = g_try_malloc(moto_mesh_get_index_size(self) * e_num * 2);
        self->e_hard_flags  = (guint32 *)g_try_malloc(sizeof(guint32) * num);
        if( ! self->e_verts)
            ; // TODO
        for(i = 0; i < num; i++)
            self->e_hard_flags[i] = 0;

        self->he_data = (self->b32) ? g_try_malloc(sizeof(MotoHalfEdge32) * he_num):
                                      g_try_malloc(sizeof(MotoHalfEdge16) * he_num);
        if( ! self->he_data)
            ; // TODO
        // Fill in half edges with invalid indecies.
        if(self->b32)
        {
            MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
            guint32 *e_verts= (guint32 *)self->e_verts;
            for(i = 0; i < he_num; i++)
                he_data[i].next = he_data[i].prev = he_data[i].f_left = e_verts[i] = G_MAXUINT32;
        }
        else
        {
            MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;
            guint16 *e_verts = (guint16 *)self->e_verts;
            for(i = 0; i < he_num; i++)
                he_data[i].next = he_data[i].prev = he_data[i].f_left = e_verts[i] = G_MAXUINT16;
        }
    }
    else
    {
        self->e_verts = NULL;
        self->he_data = NULL;
    }

    return self;
}

MotoMesh *moto_mesh_new_copy(MotoMesh *other)
{
    MotoMesh *self = moto_mesh_new(other->v_num, other->e_num, other->f_num, moto_mesh_get_f_v_num(other));

    self->he_calculated = other->he_calculated;
    self->tesselated = other->tesselated;
    self->f_tess_num = other->f_tess_num;

    memcpy(self->v_coords, other->v_coords, sizeof(MotoVector)*self->v_num);
    memcpy(self->v_normals, other->v_normals, sizeof(MotoVector)*self->v_num);

    if(self->b32)
    {
        memcpy(self->v_data, other->v_data, sizeof(MotoMeshVert32)*self->v_num);
        memcpy(self->f_data, other->f_data, sizeof(MotoMeshFace32)*self->f_num);
        memcpy(self->e_verts, other->e_verts, sizeof(guint32) * self->e_num * 2);
        memcpy(self->f_verts, other->f_verts, sizeof(guint32) * self->f_v_num);
        memcpy(self->f_normals, other->f_normals, sizeof(MotoVector) * self->f_num);
        if(self->f_tess_num > 0)
        {
            guint mem_size =  sizeof(guint32) * self->f_tess_num * 3;
            self->f_tess_verts = g_try_malloc(mem_size);
            memcpy(self->f_tess_verts, other->f_tess_verts, mem_size);
        }
        memcpy(self->he_data, other->he_data, sizeof(MotoHalfEdge32) * self->e_num * 2);
    }
    else
    {
        memcpy(self->v_data, other->v_data, sizeof(MotoMeshVert16)*self->v_num);
        memcpy(self->f_data, other->f_data, sizeof(MotoMeshFace16)*self->f_num);
        memcpy(self->e_verts, other->e_verts, sizeof(guint16) * self->e_num * 2);
        memcpy(self->f_verts, other->f_verts, sizeof(guint16) * self->f_v_num);
        memcpy(self->f_normals, other->f_normals, sizeof(MotoVector) * self->f_num);
        if(self->f_tess_num > 0)
        {
            guint mem_size =  sizeof(guint32) * self->f_tess_num * 3;
            self->f_tess_verts = g_try_malloc(mem_size);
            memcpy(self->f_tess_verts, other->f_tess_verts, mem_size);
        }
        memcpy(self->he_data, other->he_data, sizeof(MotoHalfEdge16) * self->e_num * 2);
    }

    // moto_mesh_prepare(self);
    return self;
}

void moto_mesh_copy(MotoMesh *self, MotoMesh *other)
{
    /*
    guint min_v = min(self->v_num, other->v_num);
    guint min_e = min(self->e_num, other->e_num);
    guint min_f = min(self->f_num, other->f_num);

    guint i;
    for(i = 0; i < min_v; i++)
    {
        self->v_coords[i] = other->v_coords[i];
        self->v_normals[i] = other->v_normals[i];
    }
    */
}

MotoMeshVertAttr *moto_mesh_add_attr(MotoMesh *self, const gchar *attr_name, guint chnum)
{
    /*
    MotoMeshVertAttr *attr = moto_mesh_get_attr(self, attr_name);

    if(attr)
    {
        GString *msg = g_string_new("Mesh already has attribute with name \"");
        g_string_append(msg, attr_name);
        g_string_append(msg, "\". I won't create it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return attr;
    }

    attr = g_slice_new(MotoMeshVertAttr);

    attr->name = g_string_new(attr_name);
    attr->chnum = chnum;
    attr->data = (gfloat *)g_try_malloc(sizeof(gfloat)*self->v_num);

    self->verts_attrs = g_slist_append(self->verts_attrs, attr);

    return attr;
    */
    return NULL;
}

MotoMeshVertAttr *moto_mesh_get_attr(MotoMesh *self, const gchar *attr_name)
{
    /*
    GSList *attr = self->verts_attrs;
    for(; attr; attr = g_slist_next(attr))
    {
        if(g_utf8_collate(attr_name, ((MotoMeshVertAttr *)attr->data)->name->str) == 0)
        {
            return (MotoMeshVertAttr *)attr->data;
        }
    }
    return NULL;
    */
    return NULL;
}

/*
void moto_mesh_foreach_vertex(MotoMesh *self,
        MotoMeshForeachVertexFunc func)
{
    MotoMeshVert *vert;

    guint i;
    for(i = 0; i < self->v_num; i++)
    {
        vert = & self->verts[i];
        func(self, vert);
    }
}
*/

/*
void moto_mesh_foreach_edge(MotoMesh *self,
        MotoMeshForeachEdgeFunc func, gpointer user_data)
{
    MotoMeshEdge *edge;

    guint i;
    for(i = 0; i < self->e_num; i++)
    {
        edge = & self->edges[i];
        func(self, edge);
    }
}
*/

typedef struct _MotoTessData
{
    gint index_size;
    union
    {
        guint16* tess_verts16;
        guint32* tess_verts32;
    };
    guint tess_num;
} MotoTessData;

typedef struct _MotoTessVertexData
{
    GLdouble coords[3];
    guint index;
} MotoTessVertexData;

void CALLBACK tess_cb_vertex_data_count(void* vertex_data, void* user_data)
{
    MotoTessData* td = (MotoTessData*)user_data;
    ++td->tess_num;
}

void CALLBACK tess_cb_vertex_data(void* vertex_data, void* user_data)
{
    MotoTessData* td        = (MotoTessData*)user_data;
    MotoTessVertexData* vtd = (MotoTessVertexData*)vertex_data;

    ++td->tess_num;

    if(1 == td->index_size)
    {
        *td->tess_verts16 = (guint16)vtd->index;
        ++td->tess_verts16;
    }
    else if(2 == td->index_size)
    {
        *td->tess_verts32 = vtd->index;
        ++td->tess_verts32;
    }
    else
    {
        // TODO: Error.
    }
}

static void tess_cb_edge_flag(GLboolean flag)
{
    // Just do nothing to force GL_TRIANGLES.
}

/*
static void tess_cb_begin_data(GLenum type, void* user_data)
{
    MotoTessData* td = (MotoTessData*)user_data;
    static int x = 0;
    g_print("tess_cb_begin_data: %d\n", x++);
}
*/

void moto_mesh_tesselate_faces(MotoMesh *self)
{
    if(self->f_tess_verts)
    {
        g_free(self->f_tess_verts);
        self->f_tess_verts = NULL;
    }
    self->f_tess_num = 0;

    guint i;
    if(self->b32)
    {
    }
    else
    {
        MotoMeshFace16 *f_data = (MotoMeshFace16 *)self->f_data;
        guint16 *f_verts = self->f_verts16;

        GLUtesselator* tess = gluNewTess();

        gluTessCallback(tess, GLU_TESS_VERTEX_DATA, tess_cb_vertex_data_count);
        gluTessCallback(tess, GLU_TESS_EDGE_FLAG, tess_cb_edge_flag); // Force GL_TRIANGLES.
        // gluTessCallback(tess, GLU_TESS_BEGIN_DATA, tess_cb_begin_data);
        // gluTessCallback(tess, GLU_TESS_END, tess_cb_end);
        // gluTessCallback(tess, GLU_TESS_ERROR, tess_cb_error);
        // gluTessCallback(tess, GLU_TESS_COMBINE, tess_cb_combine);

        MotoTessData td = {1, NULL, 0};

        for(i = 0; i < self->f_num; i++)
        {
            guint start = (0 == i) ? 0: f_data[i-1].v_offset;
            guint v_num = f_data[i].v_offset - start;

            guint16 *f  = f_verts + start;
            guint16 *tf = td.tess_verts16;

            MotoTessVertexData tess_verts[v_num];

            gluTessBeginPolygon(tess, &td);
            gluTessBeginContour(tess);

            guint j;
            for(j = 0; j < v_num; ++j)
            {
                MotoTessVertexData* tv = tess_verts + j;
                tv->index = f[j];
                float* v = (float*) & self->v_coords[tv->index];
                tv->coords[0] = v[0];
                tv->coords[1] = v[1];
                tv->coords[2] = v[2];

                gluTessVertex(tess, tv, tv);
            }

            gluTessEndContour(tess);
            gluTessEndPolygon(tess);
        }

        guint mem_size = moto_mesh_get_index_size(self) * td.tess_num;
        self->f_tess_verts = g_try_malloc(mem_size);
        guint16 *f_tess_verts = self->f_tess_verts16;

        gluTessCallback(tess, GLU_TESS_VERTEX_DATA, tess_cb_vertex_data);
        gluTessCallback(tess, GLU_TESS_EDGE_FLAG, tess_cb_edge_flag); // Force GL_TRIANGLES.
        // gluTessCallback(tess, GLU_TESS_BEGIN_DATA, tess_cb_begin_data);
        // gluTessCallback(tess, GLU_TESS_END, tess_cb_end);
        // gluTessCallback(tess, GLU_TESS_ERROR, tess_cb_error);
        // gluTessCallback(tess, GLU_TESS_COMBINE, tess_cb_combine);

        td.index_size = 1;
        td.tess_verts16 = f_tess_verts;
        td.tess_num = 0;

        for(i = 0; i < self->f_num; i++)
        {
            guint start = (0 == i) ? 0: f_data[i-1].v_offset;
            guint v_num = f_data[i].v_offset - start;

            guint16 *f  = f_verts + start;
            guint16 *tf = td.tess_verts16;

            MotoTessVertexData tess_verts[v_num];

            gluTessBeginPolygon(tess, &td);
            gluTessBeginContour(tess);

            guint j;
            for(j = 0; j < v_num; ++j)
            {
                MotoTessVertexData* tv = tess_verts + j;
                tv->index = f[j];
                float* v = (float*) & self->v_coords[tv->index];
                tv->coords[0] = v[0];
                tv->coords[1] = v[1];
                tv->coords[2] = v[2];

                gluTessVertex(tess, tv, tv);
            }

            gluTessEndContour(tess);
            gluTessEndPolygon(tess);

            f_data[i].v_tess_offset = td.tess_num;
        }

        gluDeleteTess(tess);

        if((td.tess_num % 3) != 0)
        {
            // TODO: Error.
        }

        self->f_tess_num = td.tess_num/3;
    }
    self->tesselated = TRUE;
}

/* Newell's method */
void moto_mesh_calc_faces_normals(MotoMesh *self)
{
    gfloat tmp;

    if(self->b32)
    {
        guint32 fi;
        guint32 vi;
        for(fi = 0; fi < self->f_num; fi++)
        {
            self->f_normals[fi].x = 0;
            self->f_normals[fi].y = 0;
            self->f_normals[fi].z = 0;

            MotoVector *vert, *nvert;

            MotoMeshFace32 *f_data = (MotoMeshFace32 *)self->f_data;
            guint32 *f_verts = (guint32 *)self->f_verts;
            guint start = (0 == fi) ? 0: f_data[fi-1].v_offset;
            guint v_num = f_data[fi].v_offset - start;
            for(vi = 0; vi < v_num-1; vi++)
            {
                vert  = & self->v_coords[f_verts[start + vi]];
                nvert = & self->v_coords[f_verts[start + (vi + 1)%v_num]];

                self->f_normals[fi].x += (vert->y - nvert->y)*(vert->z + nvert->z);
                self->f_normals[fi].y += (vert->z - nvert->z)*(vert->x + nvert->x);
                self->f_normals[fi].z += (vert->x - nvert->x)*(vert->y + nvert->y);
            }

            gfloat *normal = (gfloat *)(self->f_normals + fi);
            vector3_normalize(normal, tmp);
        }
    }
    else
    {
        guint16 fi;
        guint16 vi;
        for(fi = 0; fi < self->f_num; fi++)
        {
            self->f_normals[fi].x = 0;
            self->f_normals[fi].y = 0;
            self->f_normals[fi].z = 0;

            MotoVector *vert, *nvert;

            MotoMeshFace16 *f_data = (MotoMeshFace16 *)self->f_data;
            guint16 *f_verts = (guint16 *)self->f_verts;
            guint start = (0 == fi) ? 0: f_data[fi-1].v_offset;
            guint v_num = f_data[fi].v_offset - start;
            for(vi = 0; vi < v_num; vi++)
            {
                vert  = & self->v_coords[f_verts[start + vi]];
                nvert = & self->v_coords[f_verts[start + (vi + 1)%v_num]];

                self->f_normals[fi].x += (vert->y - nvert->y)*(vert->z + nvert->z);
                self->f_normals[fi].y += (vert->z - nvert->z)*(vert->x + nvert->x);
                self->f_normals[fi].z += (vert->x - nvert->x)*(vert->y + nvert->y);
            }

            gfloat *normal = (gfloat *)(self->f_normals + fi);
            vector3_normalize(normal, tmp);
        }
    }
}

void moto_mesh_calc_verts_normals(MotoMesh *self)
{
    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;

        guint32 vi;
        for(vi = 0; vi < self->v_num; vi++)
        {
            gfloat *normal = (gfloat *) & self->v_normals[vi];
            vector3_zero(normal);

            guint32 begin   = v_data[vi].half_edge;
            guint32 he      = begin;
            do
            {
                if(moto_mesh_is_index_valid(self, he))
                {
                    if(moto_mesh_is_index_valid(self, he_data[he].f_left))
                    {
                        gfloat *fnormal = (gfloat *) & self->f_normals[he_data[he].f_left];
                        vector3_add(normal, fnormal);
                    }
                }
                else
                    break;

                guint32 next = he_data[moto_half_edge_pair(he)].next;
                he = next;
            }
            while(he != begin);

            gfloat len = vector3_length(normal);
            if(len)
            {
                vector3_normalize(normal, len);
            }
            else
            {
                vector3_set(normal, 0, 1, 0); // fake normal for null vector
            }
        }
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)self->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;

        guint16 vi;
        for(vi = 0; vi < self->v_num; vi++)
        {
            gfloat *normal = (gfloat *) & self->v_normals[vi];
            vector3_zero(normal);

            guint16 begin   = v_data[vi].half_edge;
            guint16 he      = begin;
            do
            {
                if(moto_mesh_is_index_valid(self, he))
                {
                    if(moto_mesh_is_index_valid(self, he_data[he].f_left))
                    {
                        gfloat *fnormal = (gfloat *) & self->f_normals[he_data[he].f_left];
                        vector3_add(normal, fnormal);
                    }
                }
                else
                    break;

                guint16 next = he_data[moto_half_edge_pair(he)].next;
                he = next;
            }
            while(he != begin);

            gfloat len = vector3_length(normal);
            if(len)
            {
                vector3_normalize(normal, len);
            }
            else
            {
                vector3_set(normal, 0, 1, 0); // fake normal for null vector
            }
        }
    }
}

void moto_mesh_calc_normals(MotoMesh *self)
{
    moto_mesh_calc_faces_normals(self);
    moto_mesh_calc_verts_normals(self);
}

gboolean moto_mesh_set_face(MotoMesh *self, guint32 fi, guint32 v_offset, guint32 *verts)
{
    if(self->b32)
    {
        MotoMeshFace32 *f_data = self->f_data32;
        guint32 *f_verts = self->f_verts32;

        f_data[fi].v_offset = v_offset;
        guint32 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
        guint32 v_num = f_data[fi].v_offset - start;

        guint32 i;
        for(i = 0; i < v_num; i++)
        {
            if(G_MAXUINT32 == verts[i])
                return FALSE;
            f_verts[start + i] = verts[i];
        }
    }
    else
    {
        MotoMeshFace16 *f_data = self->f_data16;
        guint16 *f_verts = self->f_verts16;

        f_data[fi].v_offset = v_offset;
        guint16 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
        guint16 v_num = f_data[fi].v_offset - start;

        guint16 i;
        for(i = 0; i < v_num; i++)
        {
            if(G_MAXUINT16 == (guint16)verts[i])
                return FALSE;
            f_verts[start + i] = (guint16)verts[i];
        }
    }
    return TRUE;
}

static void moto_mesh_convert_16to32(MotoMesh *self)
{
    if(self->b32)
        return;

    if( ! self->v_num || ! self->f_num)
        return;

    self->b32 = TRUE;
    self->index_gl_type = GL_UNSIGNED_INT;

    guint32 i;

    /* Converting verts. */
    MotoMeshVert32 *v_data32 = (MotoMeshVert32 *)g_try_malloc(sizeof(MotoMeshVert32)*self->v_num);
    MotoMeshVert16 *v_data16 = (MotoMeshVert16 *)self->v_data;
    for(i = 0; i < self->v_num; i++)
    {
        v_data32[i].half_edge = v_data16[i].half_edge;
    }
    g_free(self->v_data);
    self->v_data = v_data32;

    /* Converting faces. */
    MotoMeshFace32 *f_data32 = (MotoMeshFace32 *)g_try_malloc(sizeof(MotoMeshFace32)*self->f_num);
    MotoMeshFace16 *f_data16 = (MotoMeshFace16 *)self->f_data;
    for(i = 0; i < self->f_num; i++)
    {
        f_data32[i].v_offset  = f_data16[i].v_offset;
        f_data32[i].half_edge = f_data16[i].half_edge;
    }
    g_free(self->f_data);
    self->f_data = f_data32;

    guint32 *f_verts32 = (guint32 *)g_try_malloc(sizeof(guint32)*self->f_num);
    guint16 *f_verts16 = (guint16 *)self->f_verts;
    for(i = 0; i < self->f_num; i++)
    {
        f_verts32[i] = f_verts16[i];
    }
    g_free(self->f_verts);
    self->f_verts = f_verts32;

    if(self->tesselated)
    {
        guint32 *f_tess_verts32 = (guint32 *)g_try_malloc(sizeof(guint32)*self->f_tess_num);
        guint16 *f_tess_verts16 = (guint16 *)self->f_tess_verts;
        for(i = 0; i < self->f_tess_num; i++)
        {
            f_tess_verts32[i] = f_tess_verts16[i];
        }
        g_free(self->f_tess_verts);
        self->f_tess_verts = f_tess_verts32;
    }

    /* Converting edges. */
    if(self->e_num)
    {
        guint32 *e_verts32 = (guint32 *)g_try_malloc(sizeof(guint32)*self->e_num*2);
        guint16 *e_verts16 = (guint16 *)self->e_verts;
        for(i = 0; i < self->e_num*2; i++)
        {
            e_verts32[i] = e_verts16[i];
        }
        g_free(self->e_verts);
        self->e_verts = e_verts32;

        if(self->he_data)
        {
            MotoHalfEdge32 *he_data32 = (MotoHalfEdge32 *)g_try_malloc(sizeof(MotoHalfEdge32)*self->e_num*2);
            MotoHalfEdge16 *he_data16 = (MotoHalfEdge16 *)self->he_data;
            for(i = 0; i < self->e_num*2; i++)
            {
                he_data32[i].next   = he_data16[i].next;
                he_data32[i].prev   = he_data16[i].prev;
                he_data32[i].f_left = he_data16[i].f_left;
            }
            g_free(self->he_data);
            self->he_data = he_data32;
        }
    }

    /* Converting half edges. */
}

static gboolean moto_mesh_calc_e_data(MotoMesh *self)
{
    if(self->b32)
    {
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)self->f_data;
        guint32 *f_verts  = (guint32 *)self->f_verts;

        MotoEdgeList *v_edges[self->v_num];
        memset(v_edges, 0, sizeof(gpointer)*self->v_num);

        guint32 e_num  = 0;
        guint32 index  = G_MAXUINT32,
                index2 = G_MAXUINT32;
        guint32 fi, i;
        for(fi = 0; fi < self->f_num; fi++)
        {
            if( ! moto_mesh_is_index_valid(self, fi))
                continue;

            guint32 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
            guint32 v_num = f_data[fi].v_offset - start;
            for(i = 0; i < v_num; i++)
            {
                guint32 vi  = f_verts[start+i];
                guint32 nvi = f_verts[start + ((i < v_num-1)?i+1:0)];

                if( ! moto_mesh_is_index_valid(self, vi) || ! moto_mesh_is_index_valid(self, nvi))
                    return FALSE;

                index  = moto_edge_list_find_and_remove( & v_edges[vi], nvi);
                index2 = moto_edge_list_find_and_remove( & v_edges[nvi], vi);
                if(G_MAXUINT32 == index || G_MAXUINT32 == index2)
                {
                    e_num++;
                    v_edges[vi]  = moto_edge_list_new(vi, v_edges[vi]);
                    v_edges[nvi] = moto_edge_list_new(nvi, v_edges[nvi]);
                }
            }
        }
        for(i = 0; i < self->v_num; i++)
            moto_edge_list_remove_all(v_edges[i]);

        if(e_num >= G_MAXUINT32/2)
            return FALSE;
        self->e_num = e_num;
        guint32 he_num = e_num*2;

        self->e_num   = e_num;
        self->e_verts = g_try_malloc(moto_mesh_get_index_size(self) * e_num * 2);

        guint32 num = e_num/32 + 1;
        self->e_hard_flags  = (guint32 *)g_try_malloc(sizeof(guint32) * num);
        for(i = 0; i < num; i++)
            ((guint32*)self->e_hard_flags)[i] = 0;

        self->he_data = g_try_malloc(sizeof(MotoHalfEdge32) * he_num);

        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
        guint32 *e_verts = (guint32 *)self->e_verts;
        for(i = 0; i < he_num; i++)
            he_data[i].next = he_data[i].prev = he_data[i].f_left = e_verts[i] = G_MAXUINT32;
    }
    else
    {
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)self->f_data;
        guint16 *f_verts  = (guint16 *)self->f_verts;

        MotoEdgeList16 *v_edges[self->v_num];
        memset(v_edges, 0, sizeof(gpointer)*self->v_num);

        guint32 e_num  = 0;
        guint16 index  = G_MAXUINT16,
                index2 = G_MAXUINT16;
        guint16 fi, i;
        for(fi = 0; fi < self->f_num; fi++)
        {
            if( ! moto_mesh_is_index_valid(self, fi))
                continue;

            guint16 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
            guint16 v_num = f_data[fi].v_offset - start;
            for(i = 0; i < v_num; i++)
            {
                guint16 vi  = f_verts[start+i];
                guint16 nvi = f_verts[start + ((i < v_num-1)?i+1:0)];

                if( ! moto_mesh_is_index_valid(self, vi) || ! moto_mesh_is_index_valid(self, nvi))
                    return FALSE;

                index  = moto_edge_list16_find_and_remove( & v_edges[vi], nvi);
                index2 = moto_edge_list16_find_and_remove( & v_edges[nvi], vi);
                if(G_MAXUINT16 == index || G_MAXUINT16 == index2)
                {
                    e_num++;
                    v_edges[vi]  = moto_edge_list16_new(nvi, v_edges[vi]);
                    v_edges[nvi] = moto_edge_list16_new(vi, v_edges[nvi]);
                }
                index  = G_MAXUINT16;
                index2 = G_MAXUINT16;
            }
        }

        for(i = 0; i < self->v_num; i++)
            moto_edge_list16_remove_all(v_edges[i]);

        if(e_num >= G_MAXUINT32/2)
            return FALSE;

        self->e_num = e_num;
        guint32 he_num = e_num*2;

        if(he_num > (G_MAXUINT16 - 1))
        {
            // Converting mesh from 16bit to 32bit indecies.
            moto_mesh_convert_16to32(self);

            self->e_verts = g_try_malloc(moto_mesh_get_index_size(self) * e_num * 2);

            guint32 num = e_num/32 + 1;
            self->e_hard_flags  = (guint32 *)g_try_malloc(sizeof(guint32) * num);
            for(i = 0; i < num; i++)
                ((guint32*)self->e_hard_flags)[i] = 0;

            self->he_data = g_try_malloc(sizeof(MotoHalfEdge32) * he_num);

            MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
            guint32 *e_verts = (guint32 *)self->e_verts;
            for(i = 0; i < he_num; i++)
                he_data[i].next = he_data[i].prev = he_data[i].f_left = e_verts[i] = G_MAXUINT32;
        }
        else
        {
            self->e_num   = e_num;
            self->e_verts = g_try_malloc(moto_mesh_get_index_size(self) * e_num * 2);

            guint16 num = e_num/16 + 1;
            self->e_hard_flags  = (guint32 *)g_try_malloc(sizeof(guint16) * num);
            for(i = 0; i < num; i++)
                ((guint16*)self->e_hard_flags)[i] = 0;

            self->he_data = g_try_malloc(sizeof(MotoHalfEdge16) * he_num);

            MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;
            guint16 *e_verts = (guint16 *)self->e_verts;
            for(i = 0; i < he_num; i++)
                he_data[i].next = he_data[i].prev = he_data[i].f_left = e_verts[i] = G_MAXUINT16;
        }
    }

    return TRUE;
}

gboolean moto_mesh_update_he_data(MotoMesh *self)
{
    if(self->he_calculated)
        return TRUE;

    if( ! self->v_num || ! self->f_num)
        return FALSE;

    if(!self->e_num)
    {
        if(!moto_mesh_calc_e_data(self))
        {
            return FALSE;
        }
    }

    if(self->b32)
    {
        MOTO_DECLARE_MESH_DATA_32(self);

        MotoEdgeList** v_edges = \
            (MotoEdgeList**)g_try_malloc(sizeof(MotoEdgeList*)*self->v_num);
        memset(v_edges, 0, sizeof(MotoEdgeList*)*self->v_num);

        guint32 cei = 0;
        guint32 fi;
        for(fi = 0; fi < self->f_num; fi++)
        {
            if(!moto_mesh_is_index_valid(self, fi))
                return FALSE;

            guint32 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
            guint32 v_num = f_data[fi].v_offset - start;

            guint32 first_hei = G_MAXUINT32;
            guint32 prev_hei  = G_MAXUINT32;
            guint32 i;
            for(i = 0; i < v_num; i++)
            {
                guint32 vi  = f_verts[start+i];
                guint32 nvi = f_verts[start + ((i < v_num-1)?i+1:0)];
                guint32 ei;
                ei = moto_edge_list_find_edge_and_remove( & v_edges[nvi], e_verts, vi, nvi);
                if(ei != moto_edge_list_find_edge_and_remove( & v_edges[vi], e_verts, vi, nvi))
                {
                    // Error
                }

                if(!moto_mesh_is_index_valid(self, ei))
                {
                    guint32 hei  = cei*2;
                    guint32 pair = hei+1;

                    if(i)
                    {
                        he_data[prev_hei].next = hei;
                        he_data[hei].prev = prev_hei;
                        if(v_num-1 == i)
                        {
                            he_data[hei].next = first_hei;
                            he_data[first_hei].prev = hei;
                        }
                    }
                    else
                        first_hei = hei;

                    he_data[hei].f_left     = fi;
                    e_verts[cei*2]          = vi;
                    e_verts[cei*2+1]        = nvi;
                    v_data[vi].half_edge    = hei;
                    v_data[nvi].half_edge   = pair;
                    f_data[fi].half_edge    = hei;

                    prev_hei = hei;
                    v_edges[nvi] = moto_edge_list_new(cei, v_edges[nvi]);
                    v_edges[vi]  = moto_edge_list_new(cei, v_edges[vi]);
                    cei++;
                    continue;
                }

                guint32 hei = ei*2+1;
                f_data[fi].half_edge = hei;

                if(i)
                {
                    he_data[prev_hei].next = hei;
                    he_data[hei].prev = prev_hei;
                    if(v_num-1 == i)
                    {
                        he_data[hei].next = first_hei;
                        he_data[first_hei].prev = hei;
                    }
                }
                else
                    first_hei = hei;

                he_data[hei].f_left = fi;

                prev_hei = hei;
            }
        }
        guint i;
        for(i = 0; i < self->v_num; i++)
            moto_edge_list_remove_all(v_edges[i]);
        g_free(v_edges);
    }
    else
    {
        MOTO_DECLARE_MESH_DATA_16(self);

        MotoEdgeList16** v_edges = \
            (MotoEdgeList16**)g_try_malloc(sizeof(MotoEdgeList16*)*self->v_num);
        memset(v_edges, 0, sizeof(MotoEdgeList16*)*self->v_num);

        guint16 cei = 0;
        guint16 fi;
        for(fi = 0; fi < self->f_num; fi++)
        {
            if(!moto_mesh_is_index_valid(self, fi))
            {
                return FALSE;
            }

            guint16 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
            guint16 v_num = f_data[fi].v_offset - start;

            guint16 first_hei = G_MAXUINT16;
            guint16 prev_hei  = G_MAXUINT16;
            guint16 i;
            for(i = 0; i < v_num; i++)
            {
                guint16 vi  = f_verts[start+i];
                guint16 nvi = f_verts[start + ((i < v_num-1)?i+1:0)];
                guint16 ei;
                ei = moto_edge_list16_find_edge_and_remove( & v_edges[nvi], e_verts, vi, nvi);
                if(ei != moto_edge_list16_find_edge_and_remove( & v_edges[vi], e_verts, vi, nvi))
                {
                    g_free(v_edges);
                    return FALSE;
                }

                if(!moto_mesh_is_index_valid(self, ei))
                {
                    guint16 hei  = cei*2;
                    guint16 pair = hei+1;

                    if(i)
                    {
                        he_data[prev_hei].next = hei;
                        he_data[hei].prev = prev_hei;
                        if(v_num-1 == i)
                        {
                            he_data[hei].next = first_hei;
                            he_data[first_hei].prev = hei;
                        }
                    }
                    else
                        first_hei = hei;

                    he_data[hei].f_left     = fi;
                    e_verts[cei*2]          = vi;
                    e_verts[cei*2+1]        = nvi;
                    v_data[vi].half_edge    = hei;
                    v_data[nvi].half_edge   = pair;
                    f_data[fi].half_edge    = hei;

                    prev_hei = hei;
                    v_edges[nvi] = moto_edge_list16_new(cei, v_edges[nvi]);
                    v_edges[vi]  = moto_edge_list16_new(cei, v_edges[vi]);
                    cei++;
                    continue;
                }

                guint16 hei = ei*2+1;
                f_data[fi].half_edge = hei;

                if(i)
                {
                    he_data[prev_hei].next = hei;
                    he_data[hei].prev = prev_hei;
                    if(v_num-1 == i)
                    {
                        he_data[hei].next = first_hei;
                        he_data[first_hei].prev = hei;
                    }
                }
                else
                    first_hei = hei;

                he_data[hei].f_left = fi;

                prev_hei = hei;
            }
        }
        guint i;
        for(i = 0; i < self->v_num; i++)
            moto_edge_list16_remove_all(v_edges[i]);
        g_free(v_edges);
    }

    self->he_calculated = TRUE;
    return TRUE;
}

gboolean moto_mesh_prepare(MotoMesh *self)
{
    if(!moto_mesh_update_he_data(self))
        return FALSE;

    moto_mesh_calc_normals(self);
    moto_mesh_tesselate_faces(self);

    moto_mesh_update_bound(self);

    return TRUE;
}

gboolean moto_mesh_is_struct_the_same(MotoMesh *self, MotoMesh* other)
{
    if(self->v_num != other->v_num || self->e_num != other->e_num ||
       self->f_num != other->f_num)
    {
        return FALSE;
    }

    return TRUE;
}

gboolean moto_mesh_intersect_face(MotoMesh *self, guint fi, MotoRay *ray, gfloat *dist)
{
    if(self->b32)
    {
    }
    else
    {
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)self->f_data;
        guint16 *f_tess_verts  = (guint16 *)self->f_tess_verts;
        MotoVector *v_coords  = (MotoVector *)self->v_coords;

        guint16 start = (0 == fi) ? 0: f_data[fi-1].v_tess_offset;
        guint16 v_num = f_data[fi].v_tess_offset - start;

        guint16 i;
        for(i = 0; i < v_num - 2; i++)
        {
            if(moto_ray_intersect_triangle_dist(ray, dist,
               (gfloat *)( & v_coords[f_tess_verts[start + i]]),
               (gfloat *)( & v_coords[f_tess_verts[start + i + 1]]),
               (gfloat *)( & v_coords[f_tess_verts[start + i + 2]])))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void moto_mesh_calc_bound(MotoMesh* self, MotoBound* bound)
{
    gfloat min_x = 0;
    gfloat max_x = 0;
    gfloat min_y = 0;
    gfloat max_y = 0;
    gfloat min_z = 0;
    gfloat max_z = 0;

    guint i;
    for(i = 0; i < self->v_num; i++)
    {
        if(self->v_coords[i].x < min_x)
            min_x = self->v_coords[i].x;
        if(self->v_coords[i].x > max_x)
            max_x = self->v_coords[i].x;

        if(self->v_coords[i].y < min_y)
            min_y = self->v_coords[i].y;
        if(self->v_coords[i].y > max_y)
            max_y = self->v_coords[i].y;

        if(self->v_coords[i].z < min_z)
            min_z = self->v_coords[i].z;
        if(self->v_coords[i].z > max_z)
            max_z = self->v_coords[i].z;
    }

    moto_bound_set(bound, min_x, max_x, min_y, max_y, min_z, max_z);
}

MotoBound* moto_mesh_create_bound(MotoMesh* self) // TODO: Remove.
{
    MotoBound* bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    moto_mesh_calc_bound(self, bound);
    return bound;
}

static MotoBound* moto_mesh_update_bound(MotoShape* self)
{
    MotoBound* bound = moto_shape_get_bound(self);
    moto_mesh_calc_bound((MotoMesh*)self, bound);
    return bound;
}

/* is selection valid */

MotoShapeSelection *moto_mesh_create_selection(MotoMesh *self)
{
    return moto_shape_selection_new(self->v_num, self->e_num, self->f_num);
}

gboolean moto_mesh_is_selection_valid(MotoMesh *self, MotoShapeSelection *selection)
{
    if(moto_shape_selection_get_v_num(selection) != self->v_num || \
       moto_shape_selection_get_e_num(selection) != self->e_num || \
       moto_shape_selection_get_f_num(selection) != self->f_num)
        return FALSE;

    return TRUE;
}

MotoShapeSelection *moto_mesh_adapt_selection(MotoMesh *self, MotoShapeSelection *selection)
{
    MotoShapeSelection *new_selection = moto_mesh_create_selection(self);
    moto_shape_selection_copy_smth(new_selection, selection);
    return new_selection;
}

static void moto_mesh_select_more(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MotoMesh* mesh = MOTO_MESH(self);
    switch(mode)
    {
        case MOTO_SELECTION_MODE_VERTEX:
            moto_mesh_select_more_verts(mesh, selection);
        break;
        case MOTO_SELECTION_MODE_EDGE:
            moto_mesh_select_more_edges(mesh, selection);
        break;
        case MOTO_SELECTION_MODE_FACE:
            moto_mesh_select_more_faces(mesh, selection);
        break;
        default:
            return;
    }
}

static void moto_mesh_select_less(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MotoMesh* mesh = MOTO_MESH(self);
    switch(mode)
    {
        case MOTO_SELECTION_MODE_VERTEX:
            moto_mesh_select_less_verts(mesh, selection);
        break;
        case MOTO_SELECTION_MODE_EDGE:
            moto_mesh_select_less_edges(mesh, selection);
        break;
        case MOTO_SELECTION_MODE_FACE:
            moto_mesh_select_less_faces(mesh, selection);
        break;
        default:
            return;
    }
}

static void moto_mesh_select_inverse(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode)
{
    MotoMesh* mesh = MOTO_MESH(self);
    switch(mode)
    {
        case MOTO_SELECTION_MODE_VERTEX:
            moto_mesh_select_inverse_verts(mesh, selection);
        break;
        case MOTO_SELECTION_MODE_EDGE:
            moto_mesh_select_inverse_edges(mesh, selection);
        break;
        case MOTO_SELECTION_MODE_FACE:
            moto_mesh_select_inverse_faces(mesh, selection);
        break;
        default:
            return;
    }
}

void moto_mesh_select_more_verts(MotoMesh *self, MotoShapeSelection *selection)
{
    if(moto_shape_selection_get_selected_v_num(selection) == self->v_num)
        return;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
        guint32 *e_verts = (guint32 *)self->e_verts;

        guint32* selected = \
            (guint32*)g_try_malloc(sizeof(guint32)*moto_shape_selection_get_selected_v_num(selection));

        guint32 i, j = 0;
        for(i = 0; i < self->v_num; i++)
            if(moto_shape_selection_check_vertex(selection, i))
                selected[j++] = i;

        guint sv_num = moto_shape_selection_get_selected_v_num(selection);
        for(i = 0; i < sv_num; i++)
        {
            guint32 he = v_data[selected[i]].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
                continue;

            guint32 begin = he;
            do
            {
                guint32 pair = moto_half_edge_pair(he);
                guint32 vi = e_verts[pair];
                if(moto_mesh_is_index_valid(self, vi))
                    moto_shape_selection_select_vertex(selection, vi);

                guint32 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(self, next))
                    break;
                he = next;
            }
            while(he != begin);
        }

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)self->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;
        guint16 *e_verts = (guint16 *)self->e_verts;

        guint16* selected = \
            (guint16*)g_try_malloc(sizeof(guint16)*moto_shape_selection_get_selected_v_num(selection));

        guint16 i, j = 0;
        for(i = 0; i < self->v_num; i++)
            if(moto_shape_selection_check_vertex(selection, i))
                selected[j++] = i;

        guint sv_num = moto_shape_selection_get_selected_v_num(selection);
        for(i = 0; i < sv_num; i++)
        {
            guint16 he = v_data[selected[i]].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
                continue;

            gboolean broken = FALSE;
            guint16 begin = he;
            do
            {
                guint16 pair = moto_half_edge_pair(he);
                guint16 vi = e_verts[pair];
                if(moto_mesh_is_index_valid(self, vi))
                    moto_shape_selection_select_vertex(selection, vi);

                guint16 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    broken = TRUE;
                    break;
                }
                he = next;
            }
            while(he != begin);

            if(broken)
            {
                he = he_data[v_data[selected[i]].half_edge].prev;
                if( ! moto_mesh_is_index_valid(self, he))
                    continue;
                begin = he;
                do
                {
                    guint16 pair = moto_half_edge_pair(he);
                    guint16 vi = e_verts[he];
                    if(moto_mesh_is_index_valid(self, vi))
                        moto_shape_selection_select_vertex(selection, vi);

                    guint16 prev = he_data[pair].prev;
                    if( ! moto_mesh_is_index_valid(self, prev))
                        break;
                    he = prev;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
}

void moto_mesh_select_less_verts(MotoMesh *self, MotoShapeSelection *selection)
{
    if(0 == moto_shape_selection_get_selected_v_num(selection))
        return;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
        guint32 *e_verts = (guint32 *)self->e_verts;

        guint32 num = moto_shape_selection_get_selected_v_num(selection);
        guint32* selected = (guint32*)g_try_malloc(sizeof(guint32)*num*2);
        guint32* for_deselection = selected + num;

        guint32 i, j = 0;
        for(i = 0; i < self->v_num; i++)
            if(moto_shape_selection_check_vertex(selection, i))
                selected[j++] = i;

        j = 0;
        guint sv_num = moto_shape_selection_get_selected_v_num(selection);
        for(i = 0; i < sv_num; i++)
        {
            guint32 he = v_data[selected[i]].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
                continue;

            guint32 begin = he;
            do
            {
                guint32 pair = moto_half_edge_pair(he);
                guint32 vi = e_verts[pair];
                if( ! moto_mesh_is_index_valid(self, vi) || \
                    ! moto_shape_selection_check_vertex(selection, vi))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }

                guint32 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);
        }

        for(i = 0; i < j; i++)
            moto_shape_selection_deselect_vertex(selection, for_deselection[i]);

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)self->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;
        guint16 *e_verts = (guint16 *)self->e_verts;

        guint16 num = moto_shape_selection_get_selected_v_num(selection);
        guint16* selected = (guint16*)g_try_malloc(sizeof(guint16)*num*2);
        guint16* for_deselection = selected + num;

        guint16 i, j = 0;
        for(i = 0; i < self->v_num; i++)
            if(moto_shape_selection_check_vertex(selection, i))
                selected[j++] = i;

        j = 0;
        guint sv_num = moto_shape_selection_get_selected_v_num(selection);
        for(i = 0; i < sv_num; i++)
        {
            guint16 he = v_data[selected[i]].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
                continue;

            guint16 begin = he;
            do
            {
                guint16 pair = moto_half_edge_pair(he);
                guint16 vi = e_verts[pair];
                if( ! moto_mesh_is_index_valid(self, vi) || \
                    ! moto_shape_selection_check_vertex(selection, vi))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }

                guint16 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);
        }

        for(i = 0; i < j; i++)
            moto_shape_selection_deselect_vertex(selection, for_deselection[i]);

        g_free(selected);
    }
}

void moto_mesh_select_inverse_verts(MotoMesh *self, MotoShapeSelection *selection)
{
    moto_bitmask_inverse(selection->verts);
}

void moto_mesh_select_more_edges(MotoMesh *self, MotoShapeSelection *selection)
{
    if(moto_shape_selection_get_selected_e_num(selection) == self->e_num)
        return;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
        guint32 *e_verts = (guint32 *)self->e_verts;

        guint32* selected = \
            (guint32*)g_try_malloc(sizeof(guint32)*moto_shape_selection_get_selected_e_num(selection));

        guint32 i, j = 0;
        for(i = 0; i < self->e_num; i++)
            if(moto_shape_selection_check_edge(selection, i))
                selected[j++] = i;

        guint se_num = moto_shape_selection_get_selected_e_num(selection);
        for(i = 0; i < se_num; i++)
        {
            guint32 vi = e_verts[selected[i]*2];
            if( ! moto_mesh_is_index_valid(self, vi))
                continue;

            guint32 he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
                continue;

            guint32 begin = he;
            do
            {
                moto_shape_selection_select_edge(selection, moto_half_edge_edge(he));

                guint32 next = he_data[moto_half_edge_pair(he)].next; // Pair is always valid for valid half edge.
                if( ! moto_mesh_is_index_valid(self, next))
                    break;
                he = next;
            }
            while(he != begin);

            // pair
            vi = e_verts[selected[i]*2+1];
            if( ! moto_mesh_is_index_valid(self, vi))
                continue;

            he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
                continue;

            begin   = he;
            do
            {
                moto_shape_selection_select_edge(selection, moto_half_edge_edge(he));

                guint32 next = he_data[moto_half_edge_pair(he)].next; // Pair is always valid valid for half edge.
                if( ! moto_mesh_is_index_valid(self, next))
                    break;
                he = next;
            }
            while(he != begin);
        }

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16*)self->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16*)self->he_data;
        guint16 *e_verts = (guint16 *)self->e_verts;

        guint16* selected = \
            (guint16*)g_try_malloc(sizeof(guint16)*moto_shape_selection_get_selected_e_num(selection));

        guint16 i, j = 0;
        for(i = 0; i < self->e_num; i++)
            if(moto_shape_selection_check_edge(selection, i))
                selected[j++] = i;

        guint se_num = moto_shape_selection_get_selected_e_num(selection);
        for(i = 0; i < se_num; i++)
        {
            guint16 vi = e_verts[selected[i]*2];
            if( ! moto_mesh_is_index_valid(self, vi))
                continue;

            guint16 he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
                continue;

            gboolean broken = FALSE;
            guint16 begin = he;
            do
            {
                moto_shape_selection_select_edge(selection, moto_half_edge_edge(he));

                guint16 next = he_data[moto_half_edge_pair(he)].next; // Pair is always valid for valid half edge.
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    broken = TRUE;
                    break;
                }
                he = next;
            }
            while(he != begin);

            if(0&&broken)
            {
                he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(self, he))
                    continue;
                begin = he;
                do
                {
                    moto_shape_selection_select_edge(selection, moto_half_edge_edge(he));

                    guint16 prev = he_data[moto_half_edge_pair(he)].prev; // Pair is always valid for valid half edge.
                    if( ! moto_mesh_is_index_valid(self, prev))
                        break;
                    he = prev;
                }
                while(he != begin);
            }

            // pair
            vi = e_verts[selected[i]*2+1];
            if( ! moto_mesh_is_index_valid(self, vi))
                continue;

            he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
                continue;

            broken = FALSE;
            begin   = he;
            do
            {
                moto_shape_selection_select_edge(selection, moto_half_edge_edge(he));

                guint16 next = he_data[moto_half_edge_pair(he)].next; // Pair is always valid valid for half edge.
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    broken = TRUE;
                    break;
                }
                he = next;
            }
            while(he != begin);

            if(0&&broken)
            {
                he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(self, he))
                    continue;
                begin = he;
                do
                {
                    moto_shape_selection_select_edge(selection, moto_half_edge_edge(he));

                    guint16 prev = he_data[moto_half_edge_pair(he)].prev; // Pair is always valid for valid half edge.
                    if( ! moto_mesh_is_index_valid(self, prev))
                        break;
                    he = prev;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
}

void moto_mesh_select_less_edges(MotoMesh *self, MotoShapeSelection *selection)
{
    if(0 == moto_shape_selection_get_selected_e_num(selection))
        return;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
        guint32 *e_verts = (guint32 *)self->e_verts;

        guint32 num = moto_shape_selection_get_selected_e_num(selection);
        guint32* selected = (guint32*)g_try_malloc(sizeof(guint32)*num*2);
        guint32* for_deselection = selected + num;

        guint32 i, j = 0;
        for(i = 0; i < self->e_num; i++)
        {
            if(moto_shape_selection_check_edge(selection, i))
            {
                selected[j++] = i;
            }
        }

        j = 0;
        guint se_num = moto_shape_selection_get_selected_e_num(selection);
        for(i = 0; i < se_num; i++)
        {
            guint32 vi;
            vi = e_verts[selected[i]*2];
            if( ! moto_mesh_is_index_valid(self, vi))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            guint32 he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            guint32 begin = he;
            do
            {
                if( ! moto_shape_selection_check_edge(selection, moto_half_edge_edge(he)))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                guint32 next = he_data[moto_half_edge_pair(he)].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);

            // pair
            vi = e_verts[selected[i]*2+1];
            if( ! moto_mesh_is_index_valid(self, vi))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            begin = he;
            do
            {
                if( ! moto_shape_selection_check_edge(selection, moto_half_edge_edge(he)))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                guint32 next = he_data[moto_half_edge_pair(he)].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);
        }

        for(i = 0; i < j; i++)
            moto_shape_selection_deselect_edge(selection, for_deselection[i]);

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)self->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;
        guint16 *e_verts = (guint16 *)self->e_verts;

        guint16 num = moto_shape_selection_get_selected_e_num(selection);
        guint16* selected = (guint16*)g_try_malloc(sizeof(guint16)*num*2);
        guint16* for_deselection = selected + num;

        guint16 i, j = 0;
        for(i = 0; i < self->e_num; i++)
        {
            if(moto_shape_selection_check_edge(selection, i))
            {
                selected[j++] = i;
            }
        }

        j = 0;
        guint se_num = moto_shape_selection_get_selected_e_num(selection);
        for(i = 0; i < se_num; i++)
        {
            guint16 vi;
            vi = e_verts[selected[i]*2];
            if( ! moto_mesh_is_index_valid(self, vi))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            guint16 he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            guint16 begin = he;
            do
            {
                if( ! moto_shape_selection_check_edge(selection, moto_half_edge_edge(he)))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                guint16 next = he_data[moto_half_edge_pair(he)].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);

            // pair
            vi = e_verts[selected[i]*2+1];
            if( ! moto_mesh_is_index_valid(self, vi))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(self, he))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            begin = he;
            do
            {
                if( ! moto_shape_selection_check_edge(selection, moto_half_edge_edge(he)))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                guint16 next = he_data[moto_half_edge_pair(he)].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);
        }

        for(i = 0; i < j; i++)
            moto_shape_selection_deselect_edge(selection, for_deselection[i]);

        g_free(selected);
    }
}

void moto_mesh_select_inverse_edges(MotoMesh *self, MotoShapeSelection *selection)
{
    moto_bitmask_inverse(selection->edges);
}

void moto_mesh_select_more_faces(MotoMesh *self, MotoShapeSelection *selection)
{
    if(moto_shape_selection_get_selected_f_num(selection) == self->f_num)
        return;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)self->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
        guint32 *f_verts  = (guint32 *)self->f_verts;

        guint32* selected = \
            (guint32*)g_try_malloc(sizeof(guint32)*moto_shape_selection_get_selected_f_num(selection));

        guint32 i, j = 0;
        for(i = 0; i < self->f_num; i++)
            if(moto_shape_selection_check_face(selection, i))
                selected[j++] = i;

        guint32 sf_num = moto_shape_selection_get_selected_f_num(selection);
        for(i = 0; i < sf_num; i++)
        {
            guint32 start = (0 == selected[i]) ? 0: f_data[selected[i]-1].v_offset;
            guint32 v_num = f_data[selected[i]].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                guint32 vi = f_verts[start + j];
                if( ! moto_mesh_is_index_valid(self, vi))
                    continue;

                guint32 he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(self, he))
                    continue;

                guint32 begin = he;
                do
                {
                    if(moto_mesh_is_index_valid(self, he_data[he].f_left))
                        moto_shape_selection_select_face(selection, he_data[he].f_left);

                    guint32 next = he_data[moto_half_edge_pair(he)].next;
                    if( ! moto_mesh_is_index_valid(self, next))
                        break;
                    he = next;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)self->v_data;
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)self->f_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;
        guint16 *f_verts  = (guint16 *)self->f_verts;

        guint16* selected = \
            (guint16*)g_try_malloc(sizeof(guint16)*moto_shape_selection_get_selected_f_num(selection));

        guint16 i, j = 0;
        for(i = 0; i < self->f_num; i++)
            if(moto_shape_selection_check_face(selection, i))
                selected[j++] = i;

        guint16 sf_num = moto_shape_selection_get_selected_f_num(selection);
        for(i = 0; i < sf_num; i++)
        {
            guint16 start = (0 == selected[i]) ? 0: f_data[selected[i]-1].v_offset;
            guint16 v_num = f_data[selected[i]].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                guint16 vi = f_verts[start + j];
                if( ! moto_mesh_is_index_valid(self, vi))
                    continue;

                guint16 he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(self, he))
                    continue;

                guint16 begin = he;
                do
                {
                    if(moto_mesh_is_index_valid(self, he_data[he].f_left))
                        moto_shape_selection_select_face(selection, he_data[he].f_left);

                    guint16 next = he_data[moto_half_edge_pair(he)].next;
                    if( ! moto_mesh_is_index_valid(self, next))
                        break;
                    he = next;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
}

void moto_mesh_select_less_faces(MotoMesh *self, MotoShapeSelection *selection)
{
    if(0 == moto_shape_selection_get_selected_f_num(selection))
        return;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)self->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;
        guint32 *f_verts  = (guint32 *)self->f_verts;

        guint32 num = moto_shape_selection_get_selected_f_num(selection);
        guint32* selected = (guint32*)g_try_malloc(sizeof(guint32)*num*2);
        guint32* for_deselection = selected + num;

        guint32 i, j = 0, k = 0;
        for(i = 0; i < self->f_num; i++)
            if(moto_shape_selection_check_face(selection, i))
                selected[j++] = i;

        guint32 sf_num = moto_shape_selection_get_selected_f_num(selection);
        for(i = 0; i < sf_num; i++)
        {
            guint32 start = (0 == selected[i]) ? 0: f_data[selected[i]-1].v_offset;
            guint32 v_num = f_data[selected[i]].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                guint32 vi = f_verts[start + j];
                if( ! moto_mesh_is_index_valid(self, vi))
                {
                    for_deselection[k++] = selected[i];
                    break;
                }

                guint32 he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(self, he))
                {
                    for_deselection[k++] = selected[i];
                    break;
                }

                guint32 begin = he;
                do
                {
                    if( ! moto_mesh_is_index_valid(self, he_data[he].f_left) || \
                        ! moto_shape_selection_check_face(selection, he_data[he].f_left))
                    {
                        for_deselection[k++] = selected[i];
                        goto next_face32;
                    }

                    guint32 next = he_data[moto_half_edge_pair(he)].next;
                    if( ! moto_mesh_is_index_valid(self, next))
                    {
                        for_deselection[k++] = selected[i];
                        goto next_face32;
                    }
                    he = next;
                }
                while(he != begin);
            }
            next_face32: continue;
        }

        for(i = 0; i < k; i++)
            moto_shape_selection_deselect_face(selection, for_deselection[i]);

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)self->v_data;
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)self->f_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;
        guint16 *f_verts  = (guint16 *)self->f_verts;

        guint16 num = moto_shape_selection_get_selected_f_num(selection);
        guint16* selected = (guint16*)g_try_malloc(sizeof(guint16)*num*2);
        guint16* for_deselection = selected + num;

        guint16 i, j = 0, k = 0;
        for(i = 0; i < self->f_num; i++)
            if(moto_shape_selection_check_face(selection, i))
                selected[j++] = i;

        guint16 sf_num = moto_shape_selection_get_selected_f_num(selection);
        for(i = 0; i < sf_num; i++)
        {
            guint16 start = (0 == selected[i]) ? 0: f_data[selected[i]-1].v_offset;
            guint16 v_num = f_data[selected[i]].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                guint16 vi = f_verts[start + j];
                if( ! moto_mesh_is_index_valid(self, vi))
                {
                    for_deselection[k++] = selected[i];
                    break;
                }

                guint16 he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(self, he))
                {
                    for_deselection[k++] = selected[i];
                    break;
                }

                guint16 begin = he;
                do
                {
                    if( ! moto_mesh_is_index_valid(self, he_data[he].f_left) || \
                        ! moto_shape_selection_check_face(selection, he_data[he].f_left))
                    {
                        for_deselection[k++] = selected[i];
                        goto next_face16;
                    }

                    guint16 next = he_data[moto_half_edge_pair(he)].next;
                    if( ! moto_mesh_is_index_valid(self, next))
                    {
                        for_deselection[k++] = selected[i];
                        goto next_face16;
                    }
                    he = next;
                }
                while(he != begin);
            }
            next_face16: continue;
        }

        for(i = 0; i < k; i++)
            moto_shape_selection_deselect_face(selection, for_deselection[i]);

        g_free(selected);
    }
}

void moto_mesh_select_inverse_faces(MotoMesh *self, MotoShapeSelection *selection)
{
    moto_bitmask_inverse(selection->faces);
}

void moto_mesh_update_selection_from_verts(MotoMesh *self, MotoShapeSelection *selection)
{
    moto_bitmask_unset_all(selection->edges);
    moto_bitmask_unset_all(selection->faces);

    if(moto_bitmask_get_set_num(selection->verts) < 1)
        return;

    if(self->b32)
    {
        MOTO_DECLARE_MESH_DATA_32(self);

        guint selected_v_num = moto_bitmask_get_set_num(selection->verts);
        guint32 *selected    = moto_bitmask_create_array_32(selection->verts);

        guint32 i;
        for(i = 0; i < selected_v_num; ++i)
        {
            guint32 si = selected[i];
            guint32 he = v_data[si].half_edge;
            if(G_MAXUINT32 == he)
                continue;

            gboolean broken = FALSE;
            guint32 begin = he;
            do
            {
                guint32 pair = moto_half_edge_pair(he);

                moto_bitmask_set(selection->edges, moto_half_edge_edge(he)); // edge always valid
                if(G_MAXUINT32 != he_data[he].f_left)
                    moto_bitmask_set(selection->faces, he_data[he].f_left);
                if(G_MAXUINT32 != he_data[pair].f_left)
                    moto_bitmask_set(selection->faces, he_data[pair].f_left);

                guint32 next = he_data[pair].next;
                if(G_MAXUINT32 == next)
                {
                    broken = TRUE;
                    break;
                }

                he = next;
            }
            while(he != begin);

            if(broken)
            {
                he = he_data[v_data[si].half_edge].prev;
                if(G_MAXUINT32 == he)
                    continue;
                begin = he;
                do
                {
                    guint32 pair = moto_half_edge_pair(he);

                    moto_bitmask_set(selection->edges, moto_half_edge_edge(he)); // edge always valid
                    if(G_MAXUINT32 != he_data[he].f_left)
                        moto_bitmask_set(selection->faces, he_data[he].f_left);
                    if(G_MAXUINT32 != he_data[pair].f_left)
                        moto_bitmask_set(selection->faces, he_data[pair].f_left);

                    guint32 prev = he_data[pair].prev;
                    if(G_MAXUINT32 == prev)
                        break;
                    he = prev;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
    else
    {
        MOTO_DECLARE_MESH_DATA_16(self);

        guint selected_v_num = moto_bitmask_get_set_num(selection->verts);
        guint16 *selected    = moto_bitmask_create_array_16(selection->verts);

        guint16 i;
        for(i = 0; i < selected_v_num; ++i)
        {
            guint16 si = selected[i];
            guint16 he = v_data[si].half_edge;
            if(G_MAXUINT16 == he)
                continue;

            gboolean broken = FALSE;
            guint16 begin = he;
            do
            {
                guint16 pair = moto_half_edge_pair(he);

                moto_bitmask_set(selection->edges, moto_half_edge_edge(he)); // edge always valid
                if(G_MAXUINT16 != he_data[he].f_left)
                    moto_bitmask_set(selection->faces, he_data[he].f_left);
                if(G_MAXUINT16 != he_data[pair].f_left)
                    moto_bitmask_set(selection->faces, he_data[pair].f_left);

                guint16 next = he_data[pair].next;
                if(G_MAXUINT16 == next)
                {
                    broken = TRUE;
                    break;
                }

                he = next;
            }
            while(he != begin);

            if(broken)
            {
                he = he_data[v_data[si].half_edge].prev;
                if(G_MAXUINT16 == he)
                    continue;
                begin = he;
                do
                {
                    guint16 pair = moto_half_edge_pair(he);

                    moto_bitmask_set(selection->edges, moto_half_edge_edge(he)); // edge always valid
                    if(G_MAXUINT16 != he_data[he].f_left)
                        moto_bitmask_set(selection->faces, he_data[he].f_left);
                    if(G_MAXUINT16 != he_data[pair].f_left)
                        moto_bitmask_set(selection->faces, he_data[pair].f_left);

                    guint16 prev = he_data[pair].prev;
                    if(G_MAXUINT16 == prev)
                        break;
                    he = prev;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
}

void moto_mesh_update_selection_from_edges(MotoMesh *self, MotoShapeSelection *selection)
{
    moto_bitmask_unset_all(selection->verts);
    moto_bitmask_unset_all(selection->faces);

    if(moto_bitmask_get_set_num(selection->edges) < 1)
        return;

    if(self->b32)
    {
        MOTO_DECLARE_MESH_DATA_32(self);

        guint selected_e_num = moto_bitmask_get_set_num(selection->edges);
        guint32 *selected    = moto_bitmask_create_array_32(selection->edges);

        guint32 i;
        for(i = 0; i < selected_e_num; ++i)
        {
            guint32 si   = selected[i];
            guint32 he   = si*2;
            guint32 pair = si*2+1;

            moto_bitmask_set(selection->verts, e_verts[he]);
            moto_bitmask_set(selection->verts, e_verts[pair]);

            if(G_MAXUINT32 != he_data[he].f_left)
                moto_bitmask_set(selection->faces, he_data[he].f_left);
            if(G_MAXUINT32 != he_data[pair].f_left)
                moto_bitmask_set(selection->faces, he_data[pair].f_left);
        }

        g_free(selected);
    }
    else
    {
        MOTO_DECLARE_MESH_DATA_16(self);

        guint selected_e_num = moto_bitmask_get_set_num(selection->edges);
        guint16 *selected    = moto_bitmask_create_array_16(selection->edges);

        guint16 i;
        for(i = 0; i < selected_e_num; ++i)
        {
            guint16 si   = selected[i];
            guint16 he   = si*2;
            guint16 pair = si*2+1;

            moto_bitmask_set(selection->verts, e_verts[he]);
            moto_bitmask_set(selection->verts, e_verts[pair]);

            if(G_MAXUINT16 != he_data[he].f_left)
                moto_bitmask_set(selection->faces, he_data[he].f_left);
            if(G_MAXUINT16 != he_data[pair].f_left)
                moto_bitmask_set(selection->faces, he_data[pair].f_left);
        }

        g_free(selected);
    }
}

void moto_mesh_update_selection_from_faces(MotoMesh *self, MotoShapeSelection *selection)
{
    moto_bitmask_unset_all(selection->verts);
    moto_bitmask_unset_all(selection->edges);

    if(moto_bitmask_get_set_num(selection->faces) < 1)
        return;

    if(self->b32)
    {
        MOTO_DECLARE_MESH_DATA_32(self);

        guint32 selected_f_num = moto_bitmask_get_set_num(selection->faces);
        guint32 *selected = moto_bitmask_create_array_32(selection->faces);

        guint32 i;
        for(i = 0; i < selected_f_num; ++i)
        {
            guint32 si = selected[i];
            guint32 he = f_data[si].half_edge;
            guint32 begin = he;
            do
            {
                moto_bitmask_set(selection->verts, e_verts[he]);
                moto_bitmask_set(selection->edges, moto_half_edge_edge(he));

                guint32 next = he_data[he].next;;
                if(G_MAXUINT32 == next)
                    break;
                he = next;
            }
            while(he != begin);
        }

        g_free(selected);
    }
    else
    {
        MOTO_DECLARE_MESH_DATA_16(self);

        guint16 selected_f_num = moto_bitmask_get_set_num(selection->faces);
        guint16 *selected = moto_bitmask_create_array_16(selection->faces);

        guint16 i;
        for(i = 0; i < selected_f_num; ++i)
        {
            guint16 si = selected[i];
            guint16 he = f_data[si].half_edge;
            guint16 begin = he;
            int x = 0;
            do
            {
                guint16 vi = e_verts[he];
                if(G_MAXUINT16 == vi || vi >= self->v_num)
                    break;

                moto_bitmask_set(selection->verts, vi);
                moto_bitmask_set(selection->edges, moto_half_edge_edge(he));

                guint16 next = he_data[he].next;;
                if(G_MAXUINT16 == next)
                    break;
                he = next;
            }
            while(he != begin);
        }

        g_free(selected);
    }
}

guint moto_mesh_get_face_v_num(MotoMesh *self, guint fi)
{
    guint result = 0;
    if(self->b32)
    {
        guint32 start = (0 == fi) ? 0: self->f_data32[fi-1].v_offset;
        result = self->f_data32[fi].v_offset - start;
    }
    else
    {
        guint16 start = (0 == fi) ? 0: self->f_data16[fi-1].v_offset;
        result = self->f_data16[fi].v_offset - start;
    }
    return result;
}

guint moto_mesh_get_v_edges_num(MotoMesh *self, guint vi)
{
    guint result = 0;
    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32*)self->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32*)self->he_data;
        guint32 he = v_data[vi].half_edge;
        if(moto_mesh_is_index_valid(self, he))
        {
            gboolean broken = FALSE;
            guint32 begin = he;
            do
            {
                guint32 pair = moto_half_edge_pair(he);
                guint32 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    broken = TRUE;
                    break;
                }
                he = next;
                ++result;
            }
            while(he != begin);

            if(broken)
            {
                he = he_data[v_data[vi].half_edge].prev;
                if(moto_mesh_is_index_valid(self, he))
                {
                    begin = he;
                    do
                    {
                        guint32 pair = moto_half_edge_pair(he);
                        guint32 prev = he_data[pair].prev;
                        if( ! moto_mesh_is_index_valid(self, prev))
                            break;
                        he = prev;
                        ++result;
                    }
                    while(he != begin);
                }
            }
        }
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16*)self->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16*)self->he_data;
        guint16 he = v_data[vi].half_edge;
        if(moto_mesh_is_index_valid(self, he))
        {
            gboolean broken = FALSE;
            guint16 begin = he;
            do
            {
                guint16 pair = moto_half_edge_pair(he);
                guint16 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(self, next))
                {
                    broken = TRUE;
                    break;
                }
                he = next;
                ++result;
            }
            while(he != begin);

            if(broken)
            {
                he = he_data[v_data[vi].half_edge].prev;
                if(moto_mesh_is_index_valid(self, he))
                {
                    begin = he;
                    do
                    {
                        guint16 pair = moto_half_edge_pair(he);
                        guint16 prev = he_data[pair].prev;
                        if( ! moto_mesh_is_index_valid(self, prev))
                            break;
                        he = prev;
                        ++result;
                    }
                    while(he != begin);
                }
            }
        }
    }
    return result;
}

MotoMesh* moto_mesh_extrude_faces(MotoMesh *self,
    MotoShapeSelection *selection, guint sections,
    gfloat ltx, gfloat lty, gfloat ltz,
    gfloat lrx, gfloat lry, gfloat lrz,
    gfloat lsx, gfloat lsy, gfloat lsz)
{
    if(!selection)
        return moto_mesh_new_copy(self);

    guint selected_f_num = moto_shape_selection_get_selected_f_num(selection);
    if(sections < 1 || selected_f_num < 1)
    {
        return moto_mesh_new_copy(self);
    }

    selection = moto_mesh_adapt_selection(self, selection);
    selected_f_num = moto_shape_selection_get_selected_f_num(selection);
    if(selected_f_num < 1)
    {
        moto_shape_selection_free(selection);
        return moto_mesh_new_copy(self);
    }

    guint f_num = self->f_num;
    guint e_num = self->e_num;
    guint v_num = self->v_num;
    guint f_v_num = self->f_v_num;

    guint16 *selected = moto_bitmask_create_array_16(selection->faces);

    MotoMeshFace16 *self_f_data = (MotoMeshFace16*)self->f_data;
    guint16 *self_f_verts = (guint16*)self->f_verts;

    guint16 i;
    for(i = 0; i < self->f_num; i++)
    {
        if(moto_shape_selection_check_face(selection, i))
        {
            guint16 num = moto_mesh_get_face_v_num(self, i)*sections;
            v_num += num;
            e_num += num*2;
            f_num += num;
            f_v_num += num*4;
        }
    }

    MotoMesh *mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);

    memcpy(mesh->v_coords, self->v_coords, sizeof(MotoVector) * self->v_num);

    memcpy(mesh->v_data, self->v_data,   sizeof(MotoMeshVert16)*self->v_num);
    memcpy(mesh->f_data, self->f_data,   sizeof(MotoMeshFace16)*self->f_num);
    memcpy(mesh->e_verts, self->e_verts, sizeof(guint16)*self->e_num*2);
    memcpy(mesh->f_verts, self->f_verts, sizeof(guint16)*self->f_v_num);

    gfloat sltx = ltx/sections;
    gfloat slty = lty/sections;
    gfloat sltz = ltz/sections;

    lrx *= RAD_PER_DEG;
    lry *= RAD_PER_DEG;
    lrz *= RAD_PER_DEG;

    guint16 fi = self->f_num;
    guint16 vi = self->v_num;
    guint16 v_offset = mesh->f_data16[self->f_num - 1].v_offset;
    guint j, k;
    for(i = 0; i < selected_f_num; ++i)
    {
        guint16 si = selected[i];
        guint v_num = moto_mesh_get_face_v_num(mesh, si);
        guint16 vs = self_f_data[si].v_offset - v_num;
        gfloat *normal = (gfloat*)(self->f_normals + si);

        guint16 prev_vloop[v_num];
        guint16 vloop[v_num];
        guint16 prev_vloop_v0, prev_vloop_v1;
        guint16 vloop_v0, vloop_v1;

        // Fill initial vloop with vertex indecies of extruded face.
        size_t loop_size = sizeof(guint16)*v_num;
        memcpy(vloop, self_f_verts + vs, loop_size);

        gfloat z[] = {0, 0, 1};
        gfloat axis[3];
        gfloat tmp;
        vector3_cross(axis, normal, z);
        vector3_normalize(axis, tmp);
        gfloat c = vector3_dot(normal, z);
        gfloat s = sin(acos(c));

        MotoVector v_coords[v_num];
        MotoVector center = {0, 0, 0};
        for(j = 0; j < v_num; ++j)
        {
            v_coords[j].x = mesh->v_coords[vloop[j]].x;
            v_coords[j].y = mesh->v_coords[vloop[j]].y;
            v_coords[j].z = mesh->v_coords[vloop[j]].z;
            center.x += v_coords[j].x;
            center.y += v_coords[j].y;
            center.z += v_coords[j].z;
        }
        center.x /= v_num;
        center.y /= v_num;
        center.z /= v_num;

        gfloat m[16], im[16], tmpm[16];
        if(c == 1)
        {
            matrix44_identity(im);
        }
        else if(c == -1)
        {
            matrix44_rotate_from_axis_sincos(im, sin(acos(-1)), -1, 0, 1, 0);
        }
        else
        {
            matrix44_rotate_from_axis_sincos(im, s, c, axis[0], axis[1], axis[2]);
        }
        matrix44_translate(tmpm, -center.x, -center.y, -center.z);
        matrix44_mult(m, im, tmpm);

        matrix44_inverse(im, m, tmpm, tmp);

        for(j = 0; j < v_num; ++j)
        {
            gfloat tmp[3] = {v_coords[j].x, v_coords[j].y, v_coords[j].z};
            point3_transform((gfloat*)&v_coords[j], m, tmp);
        }

        gfloat lrxm[16];
        matrix44_rotate_x(lrxm, lrx);
        gfloat lrym[16];
        matrix44_rotate_y(lrym, lry);
        gfloat lrzm[16];
        matrix44_rotate_z(lrzm, lrz);
        gfloat lrm[16];
        matrix44_mult(lrm, lrxm, lrym);
        memcpy(tmpm, lrm, sizeof(gfloat)*16);
        matrix44_mult(lrm, tmpm, lrzm);

        for(j = 0; j < sections; ++j)
        {
            // Update vertex loops.
            memcpy(prev_vloop, vloop, loop_size);
            for(k = 0; k < v_num; ++k)
            {
                vloop[k] = vi++;
            }

            // Calculating side faces and moving verts.
            for(k = 0; k < v_num; ++k)
            {
                prev_vloop_v0 = prev_vloop[k];
                prev_vloop_v1 = prev_vloop[(k+1)%v_num];
                vloop_v0      = vloop[k];
                vloop_v1      = vloop[(k+1)%v_num];

                gfloat *c = (gfloat*)(v_coords + k);
                gfloat *p = (gfloat*)(mesh->v_coords + vloop_v0);

                gfloat jj = j + 1;
                gfloat fac = jj/sections;

                // SRT
                gfloat pp[3] = {(1-fac + fac*lsx)*c[0], (1-fac + fac*lsy)*c[1], (1-fac + fac*lsz)*c[2]};
                point3_transform(p, lrm, pp);
                pp[0] = p[0] + sltx*jj;
                pp[1] = p[1] + slty*jj;
                pp[2] = p[2] + sltz*jj;

                point3_transform(p, im, pp);

                mesh->f_data16[fi].v_offset = v_offset + 4;
                mesh->f_verts16[v_offset+3] = prev_vloop_v0;
                mesh->f_verts16[v_offset+2] = vloop_v0;
                mesh->f_verts16[v_offset+1] = vloop_v1;
                mesh->f_verts16[v_offset]   = prev_vloop_v1;
                v_offset += 4;

                ++fi;
            }
        }

        // Setting hat. Value of v_offset for extruded face is not changed.
        memcpy(mesh->f_verts16 + mesh->f_data16[si].v_offset - v_num, vloop, loop_size);
    }
    g_assert(vi == mesh->v_num);
    g_assert(fi == mesh->f_num);

    g_free(selected);
    moto_shape_selection_free(selection);
    if(!moto_mesh_prepare(mesh))
    {
        g_object_unref(mesh);
        return NULL;
    }
    return mesh;
}

MotoMesh* moto_mesh_extrude_verts(MotoMesh *self,
    MotoShapeSelection *selection, guint sections,
    gfloat length)
{
    guint selected_v_num = moto_shape_selection_get_selected_v_num(selection);
    if(sections < 1 || selected_v_num  < 1)
    {
        return moto_mesh_new_copy(self);
    }

    selection = moto_mesh_adapt_selection(self, selection);

    guint v_num   = self->v_num;
    guint e_num   = self->e_num;
    guint f_num   = self->f_num;
    guint f_v_num = self->f_v_num;

    guint16 *selected = moto_bitmask_create_array_16(selection->verts);

    guint16 i;
    for(i = 0; i < v_num; ++i)
    {
        if(moto_shape_selection_check_vertex(selection, i))
        {
            guint v_e_num = moto_mesh_get_v_edges_num(self, i);
            guint num = v_e_num*sections;
            v_num   += num;
            e_num   += (num-1)*2 + v_e_num;
            f_num   += num;
            f_v_num += (num-1)*4 + v_e_num*3;
        }
    }

    MotoMesh *mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);

    memcpy(mesh->v_data, self->v_data,   sizeof(MotoMeshVert16)*self->v_num);
    memcpy(mesh->f_data, self->f_data,   sizeof(MotoMeshFace16)*self->f_num);
    memcpy(mesh->e_verts, self->e_verts, sizeof(guint16)*self->e_num * 2);
    memcpy(mesh->f_verts, self->f_verts, sizeof(guint16)*self->f_v_num);

    MOTO_DECLARE_MESH_DATA_16(mesh);

    gfloat section_length = length/sections;

    guint16 fi = self->f_num;
    guint16 vi = self->v_num;
    guint16 v_offset = f_data[self->f_num - 1].v_offset;
    for(i = 0; i < selected_v_num; ++i)
    {
        guint16 si = selected[i];
        guint v_e_num = moto_mesh_get_v_edges_num(mesh, si);
        gfloat *normal = (gfloat*)(self->v_normals + si);

        guint16 prev_vloop[v_num];
        guint16 vloop[v_num];
        guint16 prev_vloop_v0, prev_vloop_v1;
        guint16 vloop_v0, vloop_v1;

        guint j, k;
        for(j = 0; j < sections; ++j)
        {
        
        }
    }
    g_assert(vi == mesh->v_num);
    g_assert(fi == mesh->f_num);

    g_free(selected);
    moto_shape_selection_free(selection);
    if(!moto_mesh_prepare(mesh))
    {
        g_object_unref(mesh);
        return NULL;
    }
    return mesh;
}

MotoMesh* moto_mesh_bevel_faces(MotoMesh *self, MotoShapeSelection *selection)
{
    guint selected_f_num = moto_shape_selection_get_selected_f_num(selection);
    if(selected_f_num  < 1)
    {
        return moto_mesh_new_copy(self);
    }

    selection = moto_mesh_adapt_selection(self, selection);

    guint v_num   = self->v_num;
    guint e_num   = self->e_num;
    guint f_num   = self->f_num;
    guint f_v_num = self->f_v_num;

    guint16 *selected = moto_bitmask_create_array_16(selection->faces);


}

MotoMesh* moto_mesh_remove(MotoMesh *self,
    MotoShapeSelection *selection)
{
    guint selected_f_num = 0;
    if(selection)
    {
        selected_f_num = moto_shape_selection_get_selected_f_num(selection);
        if(selected_f_num  < 1)
        {
            return moto_mesh_new_copy(self);
        }
        selection = moto_mesh_adapt_selection(self, selection);
        selected_f_num = moto_shape_selection_get_selected_f_num(selection);
        if(selected_f_num  < 1)
        {
            moto_shape_selection_free(selection);
            return moto_mesh_new_copy(self);
        }
    }
    else
    {
        return moto_mesh_new_copy(self);
    }

    guint16 v_num   = self->v_num;
    guint16 e_num   = self->e_num;
    guint16 f_num   = self->f_num - selected_f_num;
    guint16 f_v_num = self->f_v_num;

    guint16 *selected = moto_bitmask_create_array_16(selection->faces);

    MOTO_DECLARE_MESH_DATA_16(self);

    MotoShapeSelection *for_removing = moto_mesh_create_selection(self);

    guint i;
    for(i = 0; i < selected_f_num; ++i)
    {
        guint16 si = selected[i];
        guint16 he = f_data[si].half_edge;

        f_v_num -= moto_mesh_get_face_v_num(self, si);

        guint16 begin = he;
        do
        {
            gint16 ei = moto_half_edge_edge(he);
            guint16 pair = moto_half_edge_pair(he);
            guint16 fi   = he_data[pair].f_left;
            if(G_MAXUINT16 == fi)
            {
                if( ! moto_shape_selection_check_edge(for_removing, ei))
                    --e_num;
                moto_shape_selection_select_edge(for_removing, ei);
            }
            else if(moto_shape_selection_check_face(selection, fi))
            {
                if( ! moto_shape_selection_check_edge(for_removing, ei))
                    --e_num;
                moto_shape_selection_select_edge(for_removing, ei);
            }

            guint16 next = he_data[he].next;
            if(G_MAXUINT16 == next)
                break;
            he = next;
        }
        while(he != begin);
    }

    for(i = 0; i < self->v_num; ++i)
    {
        gboolean vertex_free = TRUE;

        guint16 he = v_data[i].half_edge;
        if(G_MAXUINT16 == he)
            continue;

        gboolean broken = FALSE;
        guint16 begin = he;
        do
        {
            guint16 ei = moto_half_edge_edge(he);
            if( ! moto_shape_selection_check_edge(for_removing, ei))
            {
                vertex_free = FALSE;
                break;
            }

            guint16 pair = moto_half_edge_pair(he);
            guint16 next = he_data[pair].next;
            if(G_MAXUINT16 == next)
            {
                broken = TRUE;
                break;
            }
            he = next;
        }
        while(he != begin);

        if(broken)
        {
            he = he_data[v_data[i].half_edge].prev;
            if(G_MAXUINT16 != he)
            {
                begin = he;
                do
                {
                    guint16 ei = moto_half_edge_edge(he);
                    if( ! moto_shape_selection_check_edge(for_removing, ei))
                    {
                        vertex_free = FALSE;
                        break;
                    }

                    guint16 pair = moto_half_edge_pair(he);
                    guint16 prev = he_data[pair].prev;
                    if(G_MAXUINT16 == prev)
                        break;
                    he = prev;
                }
                while(he != begin);
            }
        }

        if(vertex_free)
        {
            if( ! moto_shape_selection_check_vertex(for_removing, i))
                --v_num;
            moto_shape_selection_select_vertex(for_removing, i);
        }
    }

    MotoMesh *mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);

    guint16 fi = 0;
    guint16 v_offset = 0;
    for(i = 0; i < self->f_num; ++i)
    {
        if(moto_shape_selection_check_face(selection, i))
            continue;

        guint f_v_num = moto_mesh_get_face_v_num(self, i);

        guint16 *f_verts = (guint16*)mesh->f_verts;
        memcpy(f_verts + v_offset,
            ((guint16*)self->f_verts) + f_data[i].v_offset - f_v_num,
            sizeof(guint16)*f_v_num);

        v_offset += f_v_num;
        ((MotoMeshFace16*)mesh->f_data)[fi].v_offset = v_offset;
        ++fi;
    }

    guint16 vi = 0;
    for(i = 0; i < self->v_num; ++i)
    {
        if( ! moto_shape_selection_check_vertex(for_removing, i))
        {
            mesh->v_coords[vi] = self->v_coords[i];

            guint16 *f_verts = (guint16*)mesh->f_verts;
            guint16 j;
            for(j = 0; j < mesh->f_v_num; ++j)
            {
                if(f_verts[j] == i)
                    f_verts[j] = vi;
            }

            ++vi;
        }
    }

    g_free(selected);
    moto_shape_selection_free(for_removing);
    moto_shape_selection_free(selection);
    if(!moto_mesh_prepare(mesh))
    {
        g_object_unref(mesh);
        return NULL;
    }
    return mesh;
}

/* Implementation of MotoPointCloudIface for MotoMesh */

static void
__moto_mesh_foreach_point(MotoPointCloud *self,
    MotoPointCloudForeachPointFunc func, gpointer user_data)
{
    MotoMesh *mesh = MOTO_MESH(self);

    guint32 i;
    for(i = 0; i < mesh->v_num; i++)
        func(self, (gfloat *)(mesh->v_coords + i), (gfloat *)(mesh->v_normals + i), user_data);
}

static gboolean
__moto_mesh_can_provide_plain_data(MotoPointCloud *self)
{
    return TRUE;
}

static void
__moto_mesh_get_plain_data(MotoPointCloud *self,
    gfloat **points, gfloat **normals, gsize *size)
{
    MotoMesh *mesh = MOTO_MESH(self);

    *points  = (gfloat *)(mesh->v_coords);
    *normals = (gfloat *)(mesh->v_normals);
    *size    = mesh->v_num;
}

static void
moto_mesh_pointcloud_init(MotoPointCloudIface *iface)
{
    iface->foreach_point = __moto_mesh_foreach_point;

    iface->can_provide_plain_data = __moto_mesh_can_provide_plain_data;
    iface->get_plain_data   = __moto_mesh_get_plain_data;
}

/* Implmentation on MotoCopyableIface for MotoMesh */

static MotoCopyable*
__moto_mesh_copy_for_copyable(MotoCopyable *self)
{
    return MOTO_COPYABLE(moto_mesh_new_copy(MOTO_MESH(self)));
}

static void
moto_mesh_copyable_init(MotoCopyableIface *iface)
{
    iface->copy = __moto_mesh_copy_for_copyable;
}

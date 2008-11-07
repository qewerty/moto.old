#include <string.h>

#include "moto-mesh.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-edge-list.h"
#include "moto-messager.h"
#include "libmotoutil/matrix.h"

/* forwards */

static void moto_mesh_copyable_init(MotoCopyableIface *iface);
static void moto_mesh_point_cloud_init(MotoPointCloudIface *iface);

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
    g_free(self->v_coords);
    g_free(self->v_normals);
    g_datalist_foreach(& self->v_attrs, free_attr, NULL);
    g_datalist_clear(& self->v_attrs);

    // Free edges
    g_free(self->e_data);
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
    self->e_data    = NULL;
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
}

G_DEFINE_TYPE_WITH_CODE(MotoMesh, moto_mesh, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(MOTO_TYPE_COPYABLE,
                                              moto_mesh_copyable_init);
                        G_IMPLEMENT_INTERFACE(MOTO_TYPE_POINT_CLOUD,
                                              moto_mesh_point_cloud_init));

/* methods of class Mesh */

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
    self->v_coords  = (MotoVector *)g_try_malloc(sizeof(MotoVector) * v_num);
    if( ! self->v_coords)
        ; // TODO
    self->v_normals = (MotoVector *)g_try_malloc(sizeof(MotoVector) * v_num);
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

    if(self->b32)
    {
        MotoMeshVert32 *v_data = (MotoMeshVert32 *)self->v_data;
        for(i = 0; i < v_num; i++)
            v_data[i].half_edge = G_MAXUINT32;
    }
    else
    {
        MotoMeshVert16 *v_data = (MotoMeshVert16 *)self->v_data;
        for(i = 0; i < v_num; i++)
            v_data[i].half_edge = G_MAXUINT16;
    }

    if(e_num)
    {
        num = e_num/32 + 1;
        self->e_num     = e_num;
        self->e_data        = (self->b32) ? g_try_malloc(sizeof(MotoMeshEdge32) * e_num):
                                            g_try_malloc(sizeof(MotoMeshEdge16) * e_num);
        if( ! self->e_data)
            ; // TODO
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

    /*
    guint i, j;

    GData *attr = other->v_attrs;
    for(; attr; attr = g_slist_next(attr))
    {
        MotoMeshVertAttr *va  = (MotoMeshVertAttr *)attr->data;
        MotoMeshVertAttr *va2 = moto_mesh_add_attr(self, va->name->str, va->chnum);

        for(i = 0; i < self->v_num; i++)
        {
            for(j = 0; j < va->chnum; j++)
            {
                guint index = i*va->chnum + j;
                va2->data[index] = va->data[index];
            }
        }
    }
    */

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

void moto_mesh_tesselate_faces(MotoMesh *self)
{
    // FIXME: Temporary only for quads!

    guint mem_size = moto_mesh_get_index_size(self) * self->f_num * 3 * 2;
    self->f_tess_verts = g_try_malloc(mem_size);

    self->f_tess_num = 0;

    guint i;
    if(self->b32)
    {
        guint32 *f_verts = (guint32 *)self->f_verts;
        guint32 *f_tess_verts = (guint32 *)self->f_tess_verts;
        for(i = 0; i < self->f_num; i++)
        {
            guint32 *f  = f_verts + i*4;
            guint32 *tf = f_tess_verts + i*6;

            tf[0] = f[0];
            tf[1] = f[1];
            tf[2] = f[2];

            tf[3] = f[0];
            tf[4] = f[2];
            tf[5] = f[3];

            self->f_tess_num += 2;
        }
    }
    else
    {
        MotoMeshFace16 *f_data = (MotoMeshFace16 *)self->f_data;
        guint16 *f_verts = (guint16 *)self->f_verts;
        guint16 *f_tess_verts = (guint16 *)self->f_tess_verts;
        for(i = 0; i < self->f_num; i++)
        {
            guint start = (0 == i) ? 0: f_data[i-1].v_offset;
            guint v_num = f_data[i].v_offset - start;

            guint16 *f  = f_verts + start;
            guint16 *tf = f_tess_verts + i*6;
            if(3 == v_num)
            {
                tf[0] = f[0];
                tf[1] = f[1];
                tf[2] = f[2];
                tf[3] = f[0];
                tf[4] = f[1];
                tf[5] = f[2];
            }
            else
            {
                tf[0] = f[0];
                tf[1] = f[1];
                tf[2] = f[2];

                tf[3] = f[0];
                tf[4] = f[2];
                tf[5] = f[3];
            }

            self->f_tess_num += 2;
        }
    }
    self->tesselated = TRUE;
}

/* Newell's method */
void moto_mesh_calc_faces_normals(MotoMesh *self)
{
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
                nvert = & self->v_coords[f_verts[start + vi+1]];

                self->f_normals[fi].x += (vert->y - nvert->y)*(vert->z + nvert->z);
                self->f_normals[fi].y += (vert->z - nvert->z)*(vert->x + nvert->x);
                self->f_normals[fi].z += (vert->x - nvert->x)*(vert->y + nvert->y);
            }

            gfloat *normal = (gfloat *)(self->f_normals + fi);

            // FIXME: Temporary! Newell's method should be reimplemented.
            gfloat a[3], b[3];
            vector3_set(a, self->v_coords[f_verts[start + 1]].x - self->v_coords[f_verts[start + 0]].x,
                           self->v_coords[f_verts[start + 1]].y - self->v_coords[f_verts[start + 0]].y,
                           self->v_coords[f_verts[start + 1]].z - self->v_coords[f_verts[start + 0]].z);
            vector3_set(b, self->v_coords[f_verts[start + 2]].x - self->v_coords[f_verts[start + 1]].x,
                           self->v_coords[f_verts[start + 2]].y - self->v_coords[f_verts[start + 1]].y,
                           self->v_coords[f_verts[start + 2]].z - self->v_coords[f_verts[start + 1]].z);
            vector3_cross(normal, a, b);

            gfloat lenbuf;
            vector3_normalize(normal, lenbuf);
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
            for(vi = 0; vi < v_num-1; vi++)
            {
                vert  = & self->v_coords[f_verts[start + vi]];
                nvert = & self->v_coords[f_verts[start + vi+1]];

                self->f_normals[fi].x += (vert->y - nvert->y)*(vert->z + nvert->z);
                self->f_normals[fi].y += (vert->z - nvert->z)*(vert->x + nvert->x);
                self->f_normals[fi].z += (vert->x - nvert->x)*(vert->y + nvert->y);
            }

            gfloat *normal = (gfloat *)(self->f_normals + fi);

            // FIXME: Temporary! Newell's method should be reimplemented.
            gfloat a[3], b[3];
            vector3_set(a, self->v_coords[f_verts[start + 1]].x - self->v_coords[f_verts[start + 0]].x,
                           self->v_coords[f_verts[start + 1]].y - self->v_coords[f_verts[start + 0]].y,
                           self->v_coords[f_verts[start + 1]].z - self->v_coords[f_verts[start + 0]].z);
            vector3_set(b, self->v_coords[f_verts[start + 2]].x - self->v_coords[f_verts[start + 1]].x,
                           self->v_coords[f_verts[start + 2]].y - self->v_coords[f_verts[start + 1]].y,
                           self->v_coords[f_verts[start + 2]].z - self->v_coords[f_verts[start + 1]].z);
            vector3_cross(normal, a, b);

            gfloat lenbuf;
            vector3_normalize(normal, lenbuf);
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
        MotoMeshFace32 *f_data = (MotoMeshFace32 *)self->f_data;
        guint32 *f_verts = (guint32 *)self->f_verts;

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
        MotoMeshFace16 *f_data = (MotoMeshFace16 *)self->f_data;
        guint16 *f_verts = (guint16 *)self->f_verts;

        f_data[fi].v_offset = v_offset;
        guint16 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
        guint16 v_num = f_data[fi].v_offset - start;

        guint16 i;
        for(i = 0; i < v_num; i++)
        {
            if(G_MAXUINT16 == verts[i])
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
        self->e_data  = g_try_malloc(sizeof(MotoMeshEdge32) * e_num);
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

            self->e_data = g_try_malloc(sizeof(MotoMeshEdge32) * e_num);
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
            self->e_data  = g_try_malloc(sizeof(MotoMeshEdge16) * e_num);
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

    if( ! self->e_num)
        if( ! moto_mesh_calc_e_data(self))
            return FALSE;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32*)self->v_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)self->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32*)self->he_data;
        guint32 *e_verts = (guint32*)self->e_verts;
        guint32 *f_verts = (guint32 *)self->f_verts;

        MotoEdgeList* v_edges[self->v_num];
        memset(v_edges, 0, sizeof(gpointer)*self->v_num);

        guint32 cei = 0;
        guint32 fi;
        for(fi = 0; fi < self->f_num; fi++)
        {
            if( ! moto_mesh_is_index_valid(self, fi))
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

                if( ! moto_mesh_is_index_valid(self, ei))
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
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16*)self->v_data;
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)self->f_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16*)self->he_data;
        guint16 *e_verts = (guint16*)self->e_verts;
        guint16 *f_verts = (guint16 *)self->f_verts;

        MotoEdgeList16* v_edges[self->v_num];
        memset(v_edges, 0, sizeof(gpointer)*self->v_num);

        guint16 cei = 0;
        guint16 fi;
        for(fi = 0; fi < self->f_num; fi++)
        {
            if( ! moto_mesh_is_index_valid(self, fi))
                return FALSE;

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
                    // Error
                }

                if( ! moto_mesh_is_index_valid(self, ei))
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
    }

    self->he_calculated = TRUE;
    return TRUE;
}

gboolean moto_mesh_prepare(MotoMesh *self)
{
    if( ! moto_mesh_update_he_data(self))
        return FALSE;

    moto_mesh_calc_normals(self);
    moto_mesh_tesselate_faces(self);

    return TRUE;
}

gboolean moto_mesh_intersect_face(MotoMesh *self, guint fi, MotoRay *ray, gfloat *dist)
{
    if(self->b32)
    {
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)self->f_data;
        guint32 *f_tess_verts  = (guint32 *)self->f_tess_verts;
        MotoVector *v_coords  = (MotoVector *)self->v_coords;

        guint32 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
        guint32 v_num = f_data[fi].v_offset - start;

        guint32 i;
        for(i = 0; i < v_num - 2; i++)
        {
            if(moto_ray_intersect_triangle_dist(ray, dist,
               (gfloat *)( & v_coords[f_tess_verts[3*i+6*fi]]),
               (gfloat *)( & v_coords[f_tess_verts[3*i+6*fi+1]]),
               (gfloat *)( & v_coords[f_tess_verts[3*i+6*fi+2]])))
            {
                return TRUE;
            }
        }
    }
    else
    {
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)self->f_data;
        guint16 *f_tess_verts  = (guint16 *)self->f_tess_verts;
        MotoVector *v_coords  = (MotoVector *)self->v_coords;

        guint16 start = (0 == fi) ? 0: f_data[fi-1].v_offset;
        guint16 v_num = f_data[fi].v_offset - start;

        guint16 i;
        for(i = 0; i < v_num - 2; i++)
        {
            if(moto_ray_intersect_triangle_dist(ray, dist,
               (gfloat *)( & v_coords[f_tess_verts[3*i+6*fi]]),
               (gfloat *)( & v_coords[f_tess_verts[3*i+6*fi+1]]),
               (gfloat *)( & v_coords[f_tess_verts[3*i+6*fi+2]])))
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

MotoBound* moto_mesh_create_bound(MotoMesh* self)
{
    MotoBound* bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    moto_mesh_calc_bound(self, bound);
    return bound;
}

/* MeshSelection */

MotoMeshSelection *moto_mesh_selection_new(guint v_num, guint e_num, guint f_num)
{
    MotoMeshSelection *self = \
        (MotoMeshSelection *)g_try_malloc(sizeof(MotoMeshSelection));

    self->verts = moto_bitmask_new(v_num);
    self->edges = moto_bitmask_new(e_num);
    self->faces = moto_bitmask_new(f_num);

    return self;
}

MotoMeshSelection *moto_mesh_selection_copy(MotoMeshSelection *other)
{
    MotoMeshSelection *self = \
        moto_mesh_selection_new(moto_mesh_selection_get_v_num(other),
                                moto_mesh_selection_get_e_num(other),
                                moto_mesh_selection_get_f_num(other));

    moto_bitmask_copy(self->verts, other->verts);
    moto_bitmask_copy(self->edges, other->edges);
    moto_bitmask_copy(self->faces, other->faces);

    return self;
}

void moto_mesh_selection_copy_smth(MotoMeshSelection *self, MotoMeshSelection *other)
{
    moto_bitmask_copy_smth(self->verts, other->verts);
    moto_bitmask_copy_smth(self->edges, other->edges);
    moto_bitmask_copy_smth(self->faces, other->faces);
}

MotoMeshSelection *moto_mesh_selection_new_for_mesh(MotoMesh *mesh)
{
    return moto_mesh_selection_new(mesh->v_num, mesh->e_num, mesh->f_num);
}

void moto_mesh_selection_free(MotoMeshSelection *self)
{
    moto_bitmask_free(self->verts);
    moto_bitmask_free(self->edges);
    moto_bitmask_free(self->faces);
    g_free(self);
}

guint32 moto_mesh_selection_get_v_num(MotoMeshSelection *self)
{
    return moto_bitmask_get_bits_num(self->verts);
}

guint32 moto_mesh_selection_get_e_num(MotoMeshSelection *self)
{
    return moto_bitmask_get_bits_num(self->edges);
}

guint32 moto_mesh_selection_get_f_num(MotoMeshSelection *self)
{
    return moto_bitmask_get_bits_num(self->faces);
}

guint32 moto_mesh_selection_get_selected_v_num(MotoMeshSelection *self)
{
    return moto_bitmask_get_set_num(self->verts);
}

guint32 moto_mesh_selection_get_selected_e_num(MotoMeshSelection *self)
{
    return moto_bitmask_get_set_num(self->edges);
}

guint32 moto_mesh_selection_get_selected_f_num(MotoMeshSelection *self)
{
    return moto_bitmask_get_set_num(self->faces);
}

/* vertex selection */

void moto_mesh_selection_select_vertex(MotoMeshSelection *self, guint index)
{
    moto_bitmask_set(self->verts, index);
}

void moto_mesh_selection_deselect_vertex(MotoMeshSelection *self, guint index)
{
    moto_bitmask_unset(self->verts, index);
}

void moto_mesh_selection_deselect_all_verts(MotoMeshSelection *self)
{
    moto_bitmask_unset_all(self->verts);
}

void moto_mesh_selection_toggle_vertex_selection(MotoMeshSelection *self, guint index)
{
    moto_bitmask_toggle(self->verts, index);
}

gboolean moto_mesh_selection_is_vertex_selected(MotoMeshSelection *self, guint index)
{
    return moto_bitmask_is_set(self->verts, index);
}

/* edge selection */

void moto_mesh_selection_select_edge(MotoMeshSelection *self, guint index)
{
    moto_bitmask_set(self->edges, index);
}

void moto_mesh_selection_deselect_edge(MotoMeshSelection *self, guint index)
{
    moto_bitmask_unset(self->edges, index);
}

void moto_mesh_selection_deselect_all_edges(MotoMeshSelection *self)
{
    moto_bitmask_unset_all(self->edges);
}

void moto_mesh_selection_toggle_edge_selection(MotoMeshSelection *self, guint index)
{
    moto_bitmask_toggle(self->edges, index);
}

gboolean moto_mesh_selection_is_edge_selected(MotoMeshSelection *self, guint index)
{
    return moto_bitmask_is_set(self->edges, index);
}

/* face selection */

void moto_mesh_selection_select_face(MotoMeshSelection *self, guint index)
{
    moto_bitmask_set(self->faces, index);
}

void moto_mesh_selection_deselect_face(MotoMeshSelection *self, guint index)
{
    return moto_bitmask_unset(self->faces, index);
}

void moto_mesh_selection_deselect_all_faces(MotoMeshSelection *self)
{
    return moto_bitmask_unset_all(self->faces);
}

void moto_mesh_selection_toggle_face_selection(MotoMeshSelection *self, guint index)
{
    moto_bitmask_toggle(self->faces, index);
}

gboolean moto_mesh_selection_is_face_selected(MotoMeshSelection *self, guint index)
{
    return moto_bitmask_is_set(self->faces, index);
}

void moto_mesh_selection_deselect_all(MotoMeshSelection *self)
{
    moto_mesh_selection_deselect_all_verts(self);
    moto_mesh_selection_deselect_all_edges(self);
    moto_mesh_selection_deselect_all_faces(self);
}

/* is selection valid */

gboolean moto_mesh_selection_is_valid(MotoMeshSelection *self, MotoMesh *mesh)
{
    if(moto_mesh_selection_get_v_num(self) != mesh->v_num || \
       moto_mesh_selection_get_e_num(self) != mesh->e_num || \
       moto_mesh_selection_get_f_num(self) != mesh->f_num)
        return FALSE;

    return TRUE;
}

void moto_mesh_selection_select_more_verts(MotoMeshSelection *self, MotoMesh *mesh)
{
    if(moto_mesh_selection_get_selected_v_num(self) == mesh->v_num)
        return;

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)mesh->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)mesh->he_data;
        guint32 *e_verts = (guint32 *)mesh->e_verts;

        guint32* selected = \
            (guint32*)g_try_malloc(sizeof(guint32)*moto_mesh_selection_get_selected_v_num(self));

        guint32 i, j = 0;
        for(i = 0; i < mesh->v_num; i++)
            if(moto_mesh_selection_is_vertex_selected(self, i))
                selected[j++] = i;

        guint sv_num = moto_mesh_selection_get_selected_v_num(self);
        for(i = 0; i < sv_num; i++)
        {
            guint32 he = v_data[selected[i]].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
                continue;

            guint32 begin = he;
            do
            {
                guint32 pair = moto_half_edge_pair(he);
                guint32 vi = e_verts[pair];
                if(moto_mesh_is_index_valid(mesh, vi))
                    moto_mesh_selection_select_vertex(self, vi);

                guint32 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(mesh, next))
                    break;
                he = next;
            }
            while(he != begin);
        }

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)mesh->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)mesh->he_data;
        guint16 *e_verts = (guint16 *)mesh->e_verts;

        guint16* selected = \
            (guint16*)g_try_malloc(sizeof(guint16)*moto_mesh_selection_get_selected_v_num(self));

        guint16 i, j = 0;
        for(i = 0; i < mesh->v_num; i++)
            if(moto_mesh_selection_is_vertex_selected(self, i))
                selected[j++] = i;

        guint sv_num = moto_mesh_selection_get_selected_v_num(self);
        for(i = 0; i < sv_num; i++)
        {
            guint16 he = v_data[selected[i]].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
                continue;

            gboolean broken = FALSE;
            guint16 begin = he;
            do
            {
                guint16 pair = moto_half_edge_pair(he);
                guint16 vi = e_verts[pair];
                if(moto_mesh_is_index_valid(mesh, vi))
                    moto_mesh_selection_select_vertex(self, vi);

                guint16 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(mesh, next))
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
                if( ! moto_mesh_is_index_valid(mesh, he))
                    continue;
                begin = he;
                do
                {
                    guint16 pair = moto_half_edge_pair(he);
                    guint16 vi = e_verts[he];
                    if(moto_mesh_is_index_valid(mesh, vi))
                        moto_mesh_selection_select_vertex(self, vi);

                    guint16 prev = he_data[pair].prev;
                    if( ! moto_mesh_is_index_valid(mesh, prev))
                        break;
                    he = prev;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
}

void moto_mesh_selection_select_less_verts(MotoMeshSelection *self, MotoMesh*mesh)
{
    if(0 == moto_mesh_selection_get_selected_v_num(self))
        return;

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)mesh->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)mesh->he_data;
        guint32 *e_verts = (guint32 *)mesh->e_verts;

        guint32 num = moto_mesh_selection_get_selected_v_num(self);
        guint32* selected = (guint32*)g_try_malloc(sizeof(guint32)*num*2);
        if( ! selected)
        {
            // TODO
        }
        guint32* for_deselection = selected + num;

        guint32 i, j = 0;
        for(i = 0; i < mesh->v_num; i++)
            if(moto_mesh_selection_is_vertex_selected(self, i))
                selected[j++] = i;

        j = 0;
        guint sv_num = moto_mesh_selection_get_selected_v_num(self);
        for(i = 0; i < sv_num; i++)
        {
            guint32 he = v_data[selected[i]].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
                continue;

            guint32 begin = he;
            do
            {
                guint32 pair = moto_half_edge_pair(he);
                guint32 vi = e_verts[pair];
                if( ! moto_mesh_is_index_valid(mesh, vi) || \
                    ! moto_mesh_selection_is_vertex_selected(self, vi))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }

                guint32 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(mesh, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);
        }

        for(i = 0; i < j; i++)
            moto_mesh_selection_deselect_vertex(self, for_deselection[i]);

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)mesh->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)mesh->he_data;
        guint16 *e_verts = (guint16 *)mesh->e_verts;

        guint16 num = moto_mesh_selection_get_selected_v_num(self);
        guint16* selected = (guint16*)g_try_malloc(sizeof(guint16)*num*2);
        if( ! selected)
        {
            // TODO
        }
        guint16* for_deselection = selected + num;

        guint16 i, j = 0;
        for(i = 0; i < mesh->v_num; i++)
            if(moto_mesh_selection_is_vertex_selected(self, i))
                selected[j++] = i;

        j = 0;
        guint sv_num = moto_mesh_selection_get_selected_v_num(self);
        for(i = 0; i < sv_num; i++)
        {
            guint16 he = v_data[selected[i]].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
                continue;

            guint16 begin = he;
            do
            {
                guint16 pair = moto_half_edge_pair(he);
                guint16 vi = e_verts[pair];
                if( ! moto_mesh_is_index_valid(mesh, vi) || \
                    ! moto_mesh_selection_is_vertex_selected(self, vi))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }

                guint16 next = he_data[pair].next;
                if( ! moto_mesh_is_index_valid(mesh, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);
        }

        for(i = 0; i < j; i++)
            moto_mesh_selection_deselect_vertex(self, for_deselection[i]);

        g_free(selected);
    }
}

void moto_mesh_selection_select_inverse_verts(MotoMeshSelection *self, MotoMesh*mesh)
{
    moto_bitmask_inverse(self->verts);
}

void moto_mesh_selection_select_more_edges(MotoMeshSelection *self, MotoMesh*mesh)
{
    if(moto_mesh_selection_get_selected_e_num(self) == mesh->e_num)
        return;

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)mesh->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)mesh->he_data;
        guint32 *e_verts = (guint32 *)mesh->e_verts;

        guint32* selected = \
            (guint32*)g_try_malloc(sizeof(guint32)*moto_mesh_selection_get_selected_e_num(self));

        guint32 i, j = 0;
        for(i = 0; i < mesh->e_num; i++)
        {
            if(moto_mesh_selection_is_edge_selected(self, i))
            {
                selected[j++] = i;
            }
        }

        guint se_num = moto_mesh_selection_get_selected_e_num(self);
        for(i = 0; i < se_num; i++)
        {
            guint32 vi = e_verts[selected[i]*2];
            if( ! moto_mesh_is_index_valid(mesh, vi))
                continue;

            guint32 he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
                continue;

            guint32 begin = he;
            do
            {
                moto_mesh_selection_select_edge(self, moto_half_edge_edge(he));

                guint32 next = he_data[moto_half_edge_pair(he)].next; // Pair is always valid for valid half edge.
                if( ! moto_mesh_is_index_valid(mesh, next))
                    break;
                he = next;
            }
            while(he != begin);

            // pair
            vi = e_verts[selected[i]*2+1];
            if( ! moto_mesh_is_index_valid(mesh, vi))
                continue;

            he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
                continue;

            begin   = he;
            do
            {
                moto_mesh_selection_select_edge(self, moto_half_edge_edge(he));

                guint32 next = he_data[moto_half_edge_pair(he)].next; // Pair is always valid valid for half edge.
                if( ! moto_mesh_is_index_valid(mesh, next))
                    break;
                he = next;
            }
            while(he != begin);
        }

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)mesh->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)mesh->he_data;
        guint16 *e_verts = (guint16 *)mesh->e_verts;

        guint16* selected = \
            (guint16*)g_try_malloc(sizeof(guint16)*moto_mesh_selection_get_selected_e_num(self));

        guint16 i, j = 0;
        for(i = 0; i < mesh->e_num; i++)
        {
            if(moto_mesh_selection_is_edge_selected(self, i))
            {
                selected[j++] = i;
            }
        }

        guint se_num = moto_mesh_selection_get_selected_e_num(self);
        for(i = 0; i < se_num; i++)
        {
            guint16 vi = e_verts[selected[i]*2];
            if( ! moto_mesh_is_index_valid(mesh, vi))
                continue;

            guint16 he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
                continue;

            gboolean broken = FALSE;
            guint16 begin = he;
            do
            {
                moto_mesh_selection_select_edge(self, moto_half_edge_edge(he));

                guint16 next = he_data[moto_half_edge_pair(he)].next; // Pair is always valid for valid half edge.
                if( ! moto_mesh_is_index_valid(mesh, next))
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
                if( ! moto_mesh_is_index_valid(mesh, he))
                    continue;
                begin = he;
                do
                {
                    moto_mesh_selection_select_edge(self, moto_half_edge_edge(he));

                    guint16 prev = he_data[moto_half_edge_pair(he)].prev; // Pair is always valid for valid half edge.
                    if( ! moto_mesh_is_index_valid(mesh, prev))
                        break;
                    he = prev;
                }
                while(he != begin);
            }

            // pair
            vi = e_verts[selected[i]*2+1];
            if( ! moto_mesh_is_index_valid(mesh, vi))
                continue;

            he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
                continue;

            broken = FALSE;
            begin   = he;
            do
            {
                moto_mesh_selection_select_edge(self, moto_half_edge_edge(he));

                guint16 next = he_data[moto_half_edge_pair(he)].next; // Pair is always valid valid for half edge.
                if( ! moto_mesh_is_index_valid(mesh, next))
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
                if( ! moto_mesh_is_index_valid(mesh, he))
                    continue;
                begin = he;
                do
                {
                    moto_mesh_selection_select_edge(self, moto_half_edge_edge(he));

                    guint16 prev = he_data[moto_half_edge_pair(he)].prev; // Pair is always valid for valid half edge.
                    if( ! moto_mesh_is_index_valid(mesh, prev))
                        break;
                    he = prev;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
}

void moto_mesh_selection_select_less_edges(MotoMeshSelection *self, MotoMesh*mesh)
{
    if(0 == moto_mesh_selection_get_selected_e_num(self))
        return;

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)mesh->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)mesh->he_data;
        guint32 *e_verts = (guint32 *)mesh->e_verts;

        guint32 num = moto_mesh_selection_get_selected_e_num(self);
        guint32* selected = (guint32*)g_try_malloc(sizeof(guint32)*num*2);
        guint32* for_deselection = selected + num;

        guint32 i, j = 0;
        for(i = 0; i < mesh->e_num; i++)
        {
            if(moto_mesh_selection_is_edge_selected(self, i))
            {
                selected[j++] = i;
            }
        }

        j = 0;
        guint se_num = moto_mesh_selection_get_selected_e_num(self);
        for(i = 0; i < se_num; i++)
        {
            guint32 vi;
            vi = e_verts[selected[i]*2];
            if( ! moto_mesh_is_index_valid(mesh, vi))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            guint32 he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            guint32 begin = he;
            do
            {
                if( ! moto_mesh_selection_is_edge_selected(self, moto_half_edge_edge(he)))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                guint32 next = he_data[moto_half_edge_pair(he)].next;
                if( ! moto_mesh_is_index_valid(mesh, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);

            // pair
            vi = e_verts[selected[i]*2+1];
            if( ! moto_mesh_is_index_valid(mesh, vi))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            begin = he;
            do
            {
                if( ! moto_mesh_selection_is_edge_selected(self, moto_half_edge_edge(he)))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                guint32 next = he_data[moto_half_edge_pair(he)].next;
                if( ! moto_mesh_is_index_valid(mesh, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);
        }

        for(i = 0; i < j; i++)
            moto_mesh_selection_deselect_edge(self, for_deselection[i]);

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)mesh->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)mesh->he_data;
        guint16 *e_verts = (guint16 *)mesh->e_verts;

        guint16 num = moto_mesh_selection_get_selected_e_num(self);
        guint16* selected = (guint16*)g_try_malloc(sizeof(guint16)*num*2);
        guint16* for_deselection = selected + num;

        guint16 i, j = 0;
        for(i = 0; i < mesh->e_num; i++)
        {
            if(moto_mesh_selection_is_edge_selected(self, i))
            {
                selected[j++] = i;
            }
        }

        j = 0;
        guint se_num = moto_mesh_selection_get_selected_e_num(self);
        for(i = 0; i < se_num; i++)
        {
            guint16 vi;
            vi = e_verts[selected[i]*2];
            if( ! moto_mesh_is_index_valid(mesh, vi))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            guint16 he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            guint16 begin = he;
            do
            {
                if( ! moto_mesh_selection_is_edge_selected(self, moto_half_edge_edge(he)))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                guint16 next = he_data[moto_half_edge_pair(he)].next;
                if( ! moto_mesh_is_index_valid(mesh, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);

            // pair
            vi = e_verts[selected[i]*2+1];
            if( ! moto_mesh_is_index_valid(mesh, vi))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            he = v_data[vi].half_edge;
            if( ! moto_mesh_is_index_valid(mesh, he))
            {
                for_deselection[j++] = selected[i];
                continue;
            }

            begin = he;
            do
            {
                if( ! moto_mesh_selection_is_edge_selected(self, moto_half_edge_edge(he)))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                guint16 next = he_data[moto_half_edge_pair(he)].next;
                if( ! moto_mesh_is_index_valid(mesh, next))
                {
                    for_deselection[j++] = selected[i];
                    break;
                }
                he = next;
            }
            while(he != begin);
        }

        for(i = 0; i < j; i++)
            moto_mesh_selection_deselect_edge(self, for_deselection[i]);

        g_free(selected);
    }
}

void moto_mesh_selection_select_inverse_edges(MotoMeshSelection *self, MotoMesh*mesh)
{
    moto_bitmask_inverse(self->edges);
}

void moto_mesh_selection_select_more_faces(MotoMeshSelection *self, MotoMesh *mesh)
{
    if(moto_mesh_selection_get_selected_f_num(self) == mesh->f_num)
        return;

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)mesh->v_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)mesh->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)mesh->he_data;
        guint32 *f_verts  = (guint32 *)mesh->f_verts;

        guint32* selected = \
            (guint32*)g_try_malloc(sizeof(guint32)*moto_mesh_selection_get_selected_f_num(self));

        guint32 i, j = 0;
        for(i = 0; i < mesh->f_num; i++)
            if(moto_mesh_selection_is_face_selected(self, i))
                selected[j++] = i;

        guint32 sf_num = moto_mesh_selection_get_selected_f_num(self);
        for(i = 0; i < sf_num; i++)
        {
            guint32 start = (0 == selected[i]) ? 0: f_data[selected[i]-1].v_offset;
            guint32 v_num = f_data[selected[i]].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                guint32 vi = f_verts[start + j];
                if( ! moto_mesh_is_index_valid(mesh, vi))
                    continue;

                guint32 he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(mesh, he))
                    continue;

                guint32 begin = he;
                do
                {
                    if(moto_mesh_is_index_valid(mesh, he_data[he].f_left))
                        moto_mesh_selection_select_face(self, he_data[he].f_left);

                    guint32 next = he_data[moto_half_edge_pair(he)].next;
                    if( ! moto_mesh_is_index_valid(mesh, next))
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
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)mesh->v_data;
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)mesh->f_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)mesh->he_data;
        guint16 *f_verts  = (guint16 *)mesh->f_verts;

        guint16* selected = \
            (guint16*)g_try_malloc(sizeof(guint16)*moto_mesh_selection_get_selected_f_num(self));

        guint16 i, j = 0;
        for(i = 0; i < mesh->f_num; i++)
            if(moto_mesh_selection_is_face_selected(self, i))
                selected[j++] = i;

        guint16 sf_num = moto_mesh_selection_get_selected_f_num(self);
        for(i = 0; i < sf_num; i++)
        {
            guint16 start = (0 == selected[i]) ? 0: f_data[selected[i]-1].v_offset;
            guint16 v_num = f_data[selected[i]].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                guint16 vi = f_verts[start + j];
                if( ! moto_mesh_is_index_valid(mesh, vi))
                    continue;

                guint16 he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(mesh, he))
                    continue;

                guint16 begin = he;
                do
                {
                    if(moto_mesh_is_index_valid(mesh, he_data[he].f_left))
                        moto_mesh_selection_select_face(self, he_data[he].f_left);

                    guint16 next = he_data[moto_half_edge_pair(he)].next;
                    if( ! moto_mesh_is_index_valid(mesh, next))
                        break;
                    he = next;
                }
                while(he != begin);
            }
        }

        g_free(selected);
    }
}

void moto_mesh_selection_select_less_faces(MotoMeshSelection *self, MotoMesh *mesh)
{
    if(0 == moto_mesh_selection_get_selected_f_num(self))
        return;

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)mesh->v_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)mesh->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)mesh->he_data;
        guint32 *f_verts  = (guint32 *)mesh->f_verts;

        guint32 num = moto_mesh_selection_get_selected_f_num(self);
        guint32* selected = (guint32*)g_try_malloc(sizeof(guint32)*num*2);
        guint32* for_deselection = selected + num;

        guint32 i, j = 0, k = 0;
        for(i = 0; i < mesh->f_num; i++)
            if(moto_mesh_selection_is_face_selected(self, i))
                selected[j++] = i;

        guint32 sf_num = moto_mesh_selection_get_selected_f_num(self);
        for(i = 0; i < sf_num; i++)
        {
            guint32 start = (0 == selected[i]) ? 0: f_data[selected[i]-1].v_offset;
            guint32 v_num = f_data[selected[i]].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                guint32 vi = f_verts[start + j];
                if( ! moto_mesh_is_index_valid(mesh, vi))
                {
                    for_deselection[k++] = selected[i];
                    break;
                }

                guint32 he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(mesh, he))
                {
                    for_deselection[k++] = selected[i];
                    break;
                }

                guint32 begin = he;
                do
                {
                    if( ! moto_mesh_is_index_valid(mesh, he_data[he].f_left) || \
                        ! moto_mesh_selection_is_face_selected(self, he_data[he].f_left))
                    {
                        for_deselection[k++] = selected[i];
                        goto next_face32;
                    }

                    guint32 next = he_data[moto_half_edge_pair(he)].next;
                    if( ! moto_mesh_is_index_valid(mesh, next))
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
            moto_mesh_selection_deselect_face(self, for_deselection[i]);

        g_free(selected);
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)mesh->v_data;
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)mesh->f_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)mesh->he_data;
        guint16 *f_verts  = (guint16 *)mesh->f_verts;

        guint16 num = moto_mesh_selection_get_selected_f_num(self);
        guint16* selected = (guint16*)g_try_malloc(sizeof(guint16)*num*2);
        guint16* for_deselection = selected + num;

        guint16 i, j = 0, k = 0;
        for(i = 0; i < mesh->f_num; i++)
            if(moto_mesh_selection_is_face_selected(self, i))
                selected[j++] = i;

        guint16 sf_num = moto_mesh_selection_get_selected_f_num(self);
        for(i = 0; i < sf_num; i++)
        {
            guint16 start = (0 == selected[i]) ? 0: f_data[selected[i]-1].v_offset;
            guint16 v_num = f_data[selected[i]].v_offset - start;
            for(j = 0; j < v_num; j++)
            {
                guint16 vi = f_verts[start + j];
                if( ! moto_mesh_is_index_valid(mesh, vi))
                {
                    for_deselection[k++] = selected[i];
                    break;
                }

                guint16 he = v_data[vi].half_edge;
                if( ! moto_mesh_is_index_valid(mesh, he))
                {
                    for_deselection[k++] = selected[i];
                    break;
                }

                guint16 begin = he;
                do
                {
                    if( ! moto_mesh_is_index_valid(mesh, he_data[he].f_left) || \
                        ! moto_mesh_selection_is_face_selected(self, he_data[he].f_left))
                    {
                        for_deselection[k++] = selected[i];
                        goto next_face16;
                    }

                    guint16 next = he_data[moto_half_edge_pair(he)].next;
                    if( ! moto_mesh_is_index_valid(mesh, next))
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
            moto_mesh_selection_deselect_face(self, for_deselection[i]);

        g_free(selected);
    }
}

void moto_mesh_selection_select_inverse_faces(MotoMeshSelection *self, MotoMesh *mesh)
{
    moto_bitmask_inverse(self->faces);
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

static gfloat *
__moto_mesh_get_plain_data(MotoPointCloud *self,
    gfloat **points, gfloat **normals, gsize *size)
{
    MotoMesh *mesh = MOTO_MESH(self);

    *points  = (gfloat *)(mesh->v_coords);
    *normals = (gfloat *)(mesh->v_normals);
    *size    = mesh->v_num;
}

static void
moto_mesh_point_cloud_init(MotoPointCloudIface *iface)
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

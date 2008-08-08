#include "moto-mesh.h"
#include "moto-messager.h"
#include "common/matrix.h"

/* class Mesh */

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
    g_free(self->e_hard_flags);

    // Free faces
    g_free(self->f_data);
    g_free(self->f_verts);
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
    self->f_tess_verts   = NULL;
    self->f_normals = NULL;
    self->f_use_hidden = FALSE;
    self->f_hidden_flags = NULL;
}

static void
moto_mesh_class_init(MotoMeshClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    mesh_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_dispose;
    goclass->finalize   = moto_mesh_finalize;
}

G_DEFINE_TYPE(MotoMesh, moto_mesh, G_TYPE_OBJECT);

/* methods of class Mesh */

MotoMesh *moto_mesh_new(guint v_num, guint e_num, guint f_num)
{
    MotoMesh *self = (MotoMesh *)g_object_new(MOTO_TYPE_MESH, NULL);

    guint he_num = e_num*2;
    self->b32 = max(he_num, max(v_num, f_num)) > (G_MAXUINT16 - 1);
    self->index_gl_type = (self->b32) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;

    guint num, i;

    self->v_num = v_num;
    self->v_data    = (self->b32) ? g_try_malloc(sizeof(MotoMeshVert32) * v_num):
                                    g_try_malloc(sizeof(MotoMeshVert16) * v_num);
    self->v_coords  = (MotoMeshTriplet *)g_try_malloc(sizeof(MotoMeshTriplet) * v_num);
    self->v_normals = (MotoMeshTriplet *)g_try_malloc(sizeof(MotoMeshTriplet) * v_num);

    num = e_num/32 + 1;
    self->e_num     = e_num;
    self->e_data        = (self->b32) ? g_try_malloc(sizeof(MotoMeshEdge32) * e_num):
                                        g_try_malloc(sizeof(MotoMeshEdge16) * e_num);
    self->e_verts = g_try_malloc(moto_mesh_get_index_size(self) * e_num * 2);
    self->e_hard_flags  = (guint32 *)g_try_malloc(sizeof(guint32) * num);
    for(i = 0; i < num; i++)
        self->e_hard_flags[i] = 0;

    num = f_num/32 + 1;
    self->f_num = f_num;
    self->f_data    = (self->b32) ? g_try_malloc(sizeof(MotoMeshFace32) * f_num):
                                    g_try_malloc(sizeof(MotoMeshFace16) * f_num);
    self->f_normals = (MotoMeshTriplet *)g_try_malloc(sizeof(MotoMeshTriplet) * f_num);
    self->f_hidden_flags  = (guint32 *)g_try_malloc(sizeof(guint32) * num);
    for(i = 0; i < num; i++)
        self->f_hidden_flags[i] = 0;

    self->he_data = (self->b32) ? g_try_malloc(sizeof(MotoHalfEdge32) * 2*e_num):
                                  g_try_malloc(sizeof(MotoHalfEdge16) * 2*e_num);

    return self;
}

MotoMesh *moto_mesh_copy(MotoMesh *other)
{
    /*
    MotoMesh *self = moto_mesh_new(other->v_num, other->e_num, other->f_num);

    guint i, j;

    for(i = 0; i < self->v_num; i++)
        self->verts[i] = other->verts[i];
    for(i = 0; i < self->e_num; i++)
        self->edges[i] = other->edges[i];
    for(i = 0; i < self->f_num; i++)
        self->faces[i] = other->faces[i];

    GSList *attr = other->verts_attrs;
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

    return self;
    */
    return NULL;
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

typedef struct _FaceTessData
{
    MotoMesh *mesh;
    guint start_index,
          end_index;
} FaceTessData;

/*
static gpointer face_tesselation_thread(FaceTessData *data)
{
    MotoMesh *mesh = data->mesh;

    guint i;
    for(i = data->start_index; i < data->end_index; i++)
        moto_mesh_face_tesselate(& mesh->faces[i], mesh);

    return NULL;
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
        guint16 *f_verts = (guint16 *)self->f_verts;
        guint16 *f_tess_verts = (guint16 *)self->f_tess_verts;
        for(i = 0; i < self->f_num; i++)
        {
            guint16 *f  = f_verts + i*4;
            guint16 *tf = f_tess_verts + i*6;

            tf[0] = f[0];
            tf[1] = f[1];
            tf[2] = f[2];

            tf[3] = f[0];
            tf[4] = f[2];
            tf[5] = f[3];

            self->f_tess_num += 2;
        }
    }
}

/*
void moto_mesh_face_hole_init(MotoMeshFaceHole *self, guint v_num)
{
    self->v_num = v_num;
    self->indecies = (guint *)g_try_malloc(sizeof(guint) * self->v_num);
}
*/

/*
void moto_mesh_face_init(MotoMeshFace *self,
        guint v_num, guint holes_num)
{
    self->v_num = v_num;
    self->holes_num = holes_num;
    // self->triangles_num = triangles_num;

    self->indecies = (guint *)g_try_malloc(sizeof(guint) * self->v_num);
    self->holes = (MotoMeshFaceHole *)g_try_malloc(sizeof(MotoMeshFaceHole) * self->holes_num);
    // self->triangles = (MotoTriangle *)g_try_malloc(sizeof(MotoTriangle) * self->triangles_num);
}
*/

/*
void moto_mesh_face_alloc(MotoMeshFace *self)
{
    self->indecies = (guint *)g_try_malloc(sizeof(guint) * self->v_num);
}
*/

/*
void moto_mesh_face_free(MotoMeshFace *self)
{
    g_free(self->indecies);

    guint i;
    for(i = 0; i < self->holes_num; i++)
        g_free(self->holes[i].indecies);
    g_free(self->holes);

    // if(self->triangles)
    //    g_free(self->triangles);
}
*/

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

            MotoMeshTriplet *vert, *nvert;

            MotoMeshFace32 *f_data = (MotoMeshFace32 *)self->f_data;
            guint32 *f_verts = (guint32 *)self->f_verts;
            guint start = (0 == fi) ? 0: f_data[fi-1].v_num;
            guint v_num = f_data[fi].v_num - start;
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

            MotoMeshTriplet *vert, *nvert;

            MotoMeshFace16 *f_data = (MotoMeshFace16 *)self->f_data;
            guint16 *f_verts = (guint16 *)self->f_verts;
            guint start = (0 == fi) ? 0: f_data[fi-1].v_num;
            guint v_num = f_data[fi].v_num - start;
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
        MotoMeshVert32 *v_data = (MotoMeshVert32 *)self->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;

        guint32 vi;
        for(vi = 0; vi < self->v_num; vi++)
        {
            gfloat *normal = (gfloat *) & self->v_normals[vi];
            vector3_zero(normal);

            MotoHalfEdge32 *begin   = & he_data[v_data[vi].half_edge];
            MotoHalfEdge32 *he      = begin;
            do
            {
                gfloat *fnormal = (gfloat *) & self->f_normals[he->f_left];
                vector3_add(normal, fnormal);

                he = & he_data[he_data[he->pair].next];
            }
            while(he != begin);

            gfloat lenbuf;
            vector3_normalize(normal, lenbuf);
        }
    }
    else
    {
        MotoMeshVert16 *v_data = (MotoMeshVert16 *)self->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;

        guint16 vi;
        for(vi = 0; vi < self->v_num; vi++)
        {
            gfloat *normal = (gfloat *) & self->v_normals[vi];
            vector3_zero(normal);

            MotoHalfEdge16 *begin   = & he_data[v_data[vi].half_edge];
            MotoHalfEdge16 *he      = begin;
            do
            {
                gfloat *fnormal = (gfloat *) & self->f_normals[he->f_left];
                vector3_add(normal, fnormal);

                he = & he_data[he_data[he->pair].next];
            }
            while(he != begin);

            gfloat lenbuf;
            vector3_normalize(normal, lenbuf);
        }
    }
}

void moto_mesh_calc_normals(MotoMesh *self)
{
    moto_mesh_calc_faces_normals(self);
    moto_mesh_calc_verts_normals(self);
}

static guint32 find_edge32(MotoMesh *mesh, guint32 a, guint32 b)
{
    guint32 *e_verts  = (guint32*)mesh->e_verts;
    guint32 i;
    for(i = 0; i < mesh->e_num*2; i += 2)
    {
        if((e_verts[i] == a && e_verts[i+1] == b) || \
           (e_verts[i+1] == a && e_verts[i] == b))
            return i / 2;
    }
    return G_MAXUINT32;
}

static guint16 find_edge16(MotoMesh *mesh, guint16 a, guint16 b)
{
    guint16 *e_verts  = (guint16*)mesh->e_verts;
    guint16 i;
    for(i = 0; i < mesh->e_num*2; i += 2)
    {
        if((e_verts[i] == a && e_verts[i+1] == b) || \
           (e_verts[i+1] == a && e_verts[i] == b))
            return i / 2;
    }
    return G_MAXUINT16;
}

// he->edge must be already set

static void find_he32(MotoMesh *self, guint32 ei, guint32 *hei1, guint32 *hei2)
{
    MotoHalfEdge32 *he_data = (MotoHalfEdge32*)self->he_data;

    guint found = 0;
    guint32 i;
    for(i = 0; i < self->e_num*2; i++)
    {
        if(he_data[i].edge == ei)
        {
            if(0 == found)
            {
                *hei1 = i;
            }
            else
            {
                *hei2 = i;
                return;
            }
            found++;
        }
    }
}

static void find_he16(MotoMesh *self, guint16 ei, guint16 *hei1, guint16 *hei2)
{
    MotoHalfEdge16 *he_data = (MotoHalfEdge16*)self->he_data;

    guint found = 0;
    guint16 i;
    for(i = 0; i < self->e_num*2; i++)
    {
        if(he_data[i].edge == ei)
        {
            if(0 == found)
            {
                *hei1 = i;
            }
            else
            {
                *hei2 = i;
                return;
            }
            found++;
        }
    }
}

void moto_mesh_update_he_data(MotoMesh *self)
{
    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32*)self->v_data;
        MotoMeshEdge32 *e_data  = (MotoMeshEdge32*)self->e_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)self->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32*)self->he_data;
        guint32 *f_verts = (guint32 *)self->f_verts;
        // guint32 *e_verts = (guint32 *)self->e_verts;

        guint32 hei = 0;
        guint32 fi;
        for(fi = 0; fi < self->f_num; fi++)
        {
            if( ! moto_mesh_is_index_valid(self, fi))
                continue;

            MotoMeshFace32 *fd = & f_data[fi];
            fd->half_edge = hei;

            guint32 start = (0 == fi) ? 0: f_data[fi-1].v_num;
            guint32 v_num = f_data[fi].v_num - start;

            guint32 i;
            for(i = 0; i < v_num; i++)
            {
                guint32 vi  = f_verts[start+i];
                guint32 nvi = f_verts[start + ((i < v_num-1)?i+1:0)];

                guint32 ei = find_edge32(self, vi, nvi);

                guint32 hei2 = hei+i;
                he_data[hei+i].next = (i < v_num-1) ? hei2+1 : hei;
                // he_data[hei+vi].pair = ???;

                he_data[hei+i].v_origin = vi;
                he_data[hei+i].f_left = fi;
                he_data[hei+i].edge = ei;

                v_data[vi].half_edge = hei2;
                e_data[ei].half_edge = hei2; // WARNING: Set at least twice!

            }
            hei += v_num;
        }

        // Setup half edge pairs
        guint32 hei1, hei2;
        guint32 ei;
        for(ei = 0; ei < self->e_num; ei++)
        {
            find_he32(self, ei, & hei1, & hei2);
            he_data[hei1].pair = hei2;
            he_data[hei2].pair = hei1;
        }
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16*)self->v_data;
        MotoMeshEdge16 *e_data  = (MotoMeshEdge16*)self->e_data;
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)self->f_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16*)self->he_data;
        guint16 *f_verts = (guint16 *)self->f_verts;
        // guint16 *e_verts = (guint16 *)self->e_verts;

        guint16 hei = 0;
        guint16 fi;
        for(fi = 0; fi < self->f_num; fi++)
        {
            if( ! moto_mesh_is_index_valid(self, fi))
                continue;

            MotoMeshFace16 *fd = & f_data[fi];
            fd->half_edge = hei;

            guint16 start = (0 == fi) ? 0: f_data[fi-1].v_num;
            guint16 v_num = f_data[fi].v_num - start;

            guint16 i;
            for(i = 0; i < v_num; i++)
            {
                guint16 vi  = f_verts[start+i];
                guint16 nvi = f_verts[start + ((i < v_num-1)?i+1:0)];

                guint16 ei = find_edge16(self, vi, nvi);

                guint16 hei2 = hei+i;
                he_data[hei+i].next = (i < v_num-1) ? hei2+1 : hei;
                // he_data[hei+vi].pair = ???;

                he_data[hei+i].v_origin = vi;
                he_data[hei+i].f_left = fi;
                he_data[hei+i].edge = ei;

                v_data[vi].half_edge = hei2;
                e_data[ei].half_edge = hei2; // WARNING: Set at least twice!

            }
            hei += v_num;
        }

        // Setup half edge pairs
        guint16 hei1, hei2;
        guint16 ei;
        for(ei = 0; ei < self->e_num; ei++)
        {
            find_he16(self, ei, & hei1, & hei2);
            he_data[hei1].pair = hei2;
            he_data[hei2].pair = hei1;
        }
    }
}

void moto_mesh_prepare(MotoMesh *self)
{
    moto_mesh_update_he_data(self);
    moto_mesh_calc_normals(self);
    moto_mesh_tesselate_faces(self);
}

void moto_mesh_grow_vert_selection(MotoMesh *self, MotoMeshSelection *selection)
{
    if(selection->selected_v_num == self->v_num)
        return;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;

        guint32 selected[selection->selected_v_num];

        guint32 i, j = 0;
        for(i = 0; i < self->v_num; i++)
        {
            if(moto_mesh_selection_is_vertex_selected(selection, i))
            {
                selected[j++] = i;
            }
        }

        guint sv_num = selection->selected_v_num;
        for(i = 0; i < sv_num; i++)
        {
            MotoHalfEdge32 *begin   = & he_data[v_data[selected[i]].half_edge];
            MotoHalfEdge32 *he      = begin;

            do
            {
                moto_mesh_selection_select_vertex(selection, he_data[he->pair].v_origin);

                he = & he_data[he_data[he->pair].next];
            }
            while(he != begin);
        }
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)self->v_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;

        guint16 selected[selection->selected_v_num];

        guint16 i, j = 0;
        for(i = 0; i < self->v_num; i++)
        {
            if(moto_mesh_selection_is_vertex_selected(selection, i))
            {
                selected[j++] = i;
            }
        }

        guint sv_num = selection->selected_v_num;
        for(i = 0; i < sv_num; i++)
        {
            MotoHalfEdge16 *begin   = & he_data[v_data[selected[i]].half_edge];
            MotoHalfEdge16 *he      = begin;

            do
            {
                moto_mesh_selection_select_vertex(selection, he_data[he->pair].v_origin);

                he = & he_data[he_data[he->pair].next];
            }
            while(he != begin);
        }
    }
}

void moto_mesh_grow_edge_selection(MotoMesh *self, MotoMeshSelection *selection)
{
    if(selection->selected_e_num == self->e_num)
        return;

    if(self->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)self->v_data;
        MotoMeshEdge32 *e_data  = (MotoMeshEdge32 *)self->e_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)self->he_data;

        guint32 selected[selection->selected_e_num];

        guint32 i, j = 0;
        for(i = 0; i < self->e_num; i++)
        {
            if(moto_mesh_selection_is_edge_selected(selection, i))
            {
                selected[j++] = i;
            }
        }

        guint se_num = selection->selected_e_num;
        for(i = 0; i < se_num; i++)
        {
            guint32 vi = he_data[e_data[selected[i]].half_edge].v_origin;

            MotoHalfEdge32 *begin   = & he_data[v_data[vi].half_edge];
            MotoHalfEdge32 *he      = begin;

            do
            {
                moto_mesh_selection_select_edge(selection, he->edge);

                he = & he_data[he_data[he->pair].next];
            }
            while(he != begin);

            // pair
            vi = he_data[he_data[e_data[selected[i]].half_edge].pair].v_origin;

            begin   = & he_data[v_data[vi].half_edge];
            he      = begin;

            do
            {
                moto_mesh_selection_select_edge(selection, he->edge);

                he = & he_data[he_data[he->pair].next];
            }
            while(he != begin);
        }
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16 *)self->v_data;
        MotoMeshEdge16 *e_data  = (MotoMeshEdge16 *)self->e_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16 *)self->he_data;

        guint16 selected[selection->selected_e_num];

        guint16 i, j = 0;
        for(i = 0; i < self->e_num; i++)
        {
            if(moto_mesh_selection_is_edge_selected(selection, i))
            {
                selected[j++] = i;
            }
        }

        guint se_num = selection->selected_e_num;
        for(i = 0; i < se_num; i++)
        {
            guint16 vi = he_data[e_data[selected[i]].half_edge].v_origin;

            MotoHalfEdge16 *begin   = & he_data[v_data[vi].half_edge];
            MotoHalfEdge16 *he      = begin;

            do
            {
                moto_mesh_selection_select_edge(selection, he->edge);

                he = & he_data[he_data[he->pair].next];
            }
            while(he != begin);

            // pair
            vi = he_data[he_data[e_data[selected[i]].half_edge].pair].v_origin;

            begin   = & he_data[v_data[vi].half_edge];
            he      = begin;

            do
            {
                moto_mesh_selection_select_edge(selection, he->edge);

                he = & he_data[he_data[he->pair].next];
            }
            while(he != begin);
        }
    }
}

void moto_mesh_grow_face_selection(MotoMesh *self, MotoMeshSelection *selection)
{}

typedef struct _Vector
{
    gfloat xyz[3];
} Vector;

/*
void moto_mesh_face_tesselate(MotoMeshFace *self, MotoMesh *mesh)
{}
*/

/*
gboolean
moto_mesh_face_intersect_ray(MotoMeshFace *self,
        MotoMesh *mesh, MotoRay *ray, gfloat *dist)
{

    return FALSE;
}
*/

/*
void moto_mesh_face_foreach_vertex(MotoMeshFace *face,
        MotoMeshFaceForeachVertexFunc func, MotoMesh *mesh)
{
    MotoMeshVert *vert;

    guint i;
    for(i = 0; i < face->v_num; i++)
    {
        vert = & mesh->verts[face->indecies[i]];
        func(face, vert);
    }
}
*/

/* MeshSelection */

MotoMeshSelection *moto_mesh_selection_new(guint v_num, guint e_num, guint f_num)
{
    guint i, num;

    MotoMeshSelection *self = \
        (MotoMeshSelection *)g_try_malloc(sizeof(MotoMeshSelection));

    self->v_num = v_num;
    self->e_num = e_num;
    self->f_num = f_num;

    num = v_num/32 + 1;
    self->verts = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->verts[i] = 0;

    num = e_num/32 + 1;
    self->edges = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->edges[i] = 0;

    num = f_num/32 + 1;
    self->faces = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->faces[i] = 0;

    self->selected_v_num = 0;
    self->selected_e_num = 0;
    self->selected_f_num = 0;

    return self;
}

MotoMeshSelection *moto_mesh_selection_copy(MotoMeshSelection *other)
{
    guint i, num;

    MotoMeshSelection *self = \
        (MotoMeshSelection *)g_try_malloc(sizeof(MotoMeshSelection));

    self->v_num = other->v_num;
    self->e_num = other->e_num;
    self->f_num = other->f_num;

    self->selected_v_num = other->selected_v_num;
    self->selected_e_num = other->selected_e_num;
    self->selected_f_num = other->selected_f_num;

    num = self->v_num/32 + 1;
    self->verts = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->verts[i] = other->verts[i];

    num = self->e_num/32 + 1;
    self->edges = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->edges[i] = other->edges[i];

    num = self->f_num/32 + 1;
    self->faces = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->faces[i] = other->faces[i];

    return self;
}

MotoMeshSelection *moto_mesh_selection_for_mesh(MotoMesh *mesh)
{
    return moto_mesh_selection_new(mesh->v_num, mesh->e_num, mesh->f_num);
}

void moto_mesh_selection_free(MotoMeshSelection *self)
{
    g_free(self->verts);
    g_free(self->edges);
    g_free(self->faces);
    g_free(self);
}

/* vertex selection */

void moto_mesh_selection_select_vertex(MotoMeshSelection *self, guint index)
{
    if(index > (self->v_num - 1))
        return;

    if( ! moto_mesh_selection_is_vertex_selected(self, index))
        self->selected_v_num++;
    *(self->verts + (index/32)) |= 1 << (index % 32);
}

void moto_mesh_selection_deselect_vertex(MotoMeshSelection *self, guint index)
{
    if(index > (self->v_num - 1))
        return;

    if(moto_mesh_selection_is_vertex_selected(self, index))
        self->selected_v_num--;
    *(self->verts + (index/32)) &= ~(1 << (index % 32));
}

void moto_mesh_selection_deselect_all_verts(MotoMeshSelection *self)
{
    guint i;
    guint num = self->v_num/32 + 1;
    for(i = 0; i < num; i++)
        self->verts[i] = 0;
    self->selected_v_num = 0;
}

void moto_mesh_selection_toggle_vertex_selection(MotoMeshSelection *self, guint index)
{
    if(moto_mesh_selection_is_vertex_selected(self, index))
        moto_mesh_selection_deselect_vertex(self, index);
    else
        moto_mesh_selection_select_vertex(self, index);
}

gboolean moto_mesh_selection_is_vertex_selected(MotoMeshSelection *self, guint index)
{
    if(index > (self->v_num - 1))
        return FALSE;

    return *(self->verts + (index/32)) & (1 << (index % 32));
}

/* edge selection */

void moto_mesh_selection_select_edge(MotoMeshSelection *self, guint index)
{
    if(index > (self->e_num - 1))
        return;

    if( ! moto_mesh_selection_is_edge_selected(self, index))
        self->selected_e_num++;
    *(self->edges + (index/32)) |= 1 << (index % 32);
}

void moto_mesh_selection_deselect_edge(MotoMeshSelection *self, guint index)
{
    if(index > (self->e_num - 1))
        return;

    if(moto_mesh_selection_is_edge_selected(self, index))
        self->selected_e_num--;
    *(self->edges + (index/32)) &= ~(1 << (index % 32));
}

void moto_mesh_selection_deselect_all_edges(MotoMeshSelection *self)
{
    guint i;
    guint num = self->e_num/32 + 1;
    for(i = 0; i < num; i++)
        self->edges[i] = 0;
    self->selected_e_num = 0;
}

void moto_mesh_selection_toggle_edge_selection(MotoMeshSelection *self, guint index)
{
    if(moto_mesh_selection_is_edge_selected(self, index))
        moto_mesh_selection_deselect_edge(self, index);
    else
        moto_mesh_selection_select_edge(self, index);
}

gboolean moto_mesh_selection_is_edge_selected(MotoMeshSelection *self, guint index)
{
    if(index > (self->e_num - 1))
        return FALSE;

    return *(self->edges + (index/32)) & (1 << (index % 32));
}

/* face selection */

void moto_mesh_selection_select_face(MotoMeshSelection *self, guint index)
{
    if(index > (self->f_num - 1))
        return;

    if( ! moto_mesh_selection_is_face_selected(self, index))
        self->selected_f_num++;
    *(self->faces + (index/32)) |= 1 << (index % 32);
}

void moto_mesh_selection_deselect_face(MotoMeshSelection *self, guint index)
{
    if(index > (self->f_num - 1))
        return;

    if(moto_mesh_selection_is_face_selected(self, index))
        self->selected_f_num--;
    *(self->faces + (index/32)) &= ~(1 << (index % 32));
}

void moto_mesh_selection_deselect_all_faces(MotoMeshSelection *self)
{
    guint i;
    guint num = self->f_num/32 + 1;
    for(i = 0; i < num; i++)
        self->faces[i] = 0;
    self->selected_f_num = 0;
}

void moto_mesh_selection_toggle_face_selection(MotoMeshSelection *self, guint index)
{
    if(moto_mesh_selection_is_face_selected(self, index))
        moto_mesh_selection_deselect_face(self, index);
    else
        moto_mesh_selection_select_face(self, index);
}

gboolean moto_mesh_selection_is_face_selected(MotoMeshSelection *self, guint index)
{
    if(index > (self->f_num - 1))
        return FALSE;

    return *(self->faces + (index/32)) & (1 << (index % 32));
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
    if(self->v_num != mesh->v_num || self->e_num != mesh->e_num || self->f_num != mesh->f_num)
        return FALSE;

    return TRUE;
}

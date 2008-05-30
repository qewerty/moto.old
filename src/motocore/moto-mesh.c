#include "moto-mesh.h"
#include "moto-messager.h"
#include "common/matrix.h"

/* class Mesh */

static GObjectClass *mesh_parent_class = NULL;

static void
free_attr(gpointer data, gpointer user_data)
{
    MotoMeshVertAttr *attr = (MotoMeshVertAttr *)data;

    g_string_free(attr->name, TRUE);
    g_free(attr->data);
    g_slice_free(MotoMeshVertAttr, attr);
}

static void
moto_mesh_dispose(GObject *obj)
{
    MotoMesh *self = (MotoMesh *)obj;

    g_slist_foreach(self->verts_attrs, free_attr, NULL);
    g_slist_free(self->verts_attrs);

    g_free(self->verts);
    g_free(self->edges);
    g_free(self->faces);

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
    self->verts_num = 0;
    self->verts = NULL;

    self->edges_num = 0;
    self->edges = NULL;

    self->faces_num = 0;
    self->faces = NULL;

    self->verts_attrs = NULL;
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

MotoMesh *moto_mesh_new(guint verts_num, guint edges_num, guint faces_num)
{
    MotoMesh *self = (MotoMesh *)g_object_new(MOTO_TYPE_MESH, NULL);

    self->verts_num = verts_num;
    self->verts = (MotoMeshVert *)g_try_malloc(sizeof(MotoMeshVert) * verts_num);

    self->edges_num = edges_num;
    self->edges = (MotoMeshEdge *)g_try_malloc(sizeof(MotoMeshEdge) * edges_num);

    self->faces_num = faces_num;
    self->faces = (MotoMeshFace *)g_try_malloc(sizeof(MotoMeshFace) * faces_num);
    guint i;
    for(i = 0; i < faces_num; i++)
        self->faces[i].convexes = NULL;

    return self;
}

MotoMesh *moto_mesh_copy(MotoMesh *other)
{
    MotoMesh *self = moto_mesh_new(other->verts_num, other->edges_num, other->faces_num);

    guint i, j;

    for(i = 0; i < self->verts_num; i++)
        self->verts[i] = other->verts[i];
    for(i = 0; i < self->edges_num; i++)
        self->edges[i] = other->edges[i];
    for(i = 0; i < self->faces_num; i++)
        self->faces[i] = other->faces[i];

    GSList *attr = other->verts_attrs;
    for(; attr; attr = g_slist_next(attr))
    {
        MotoMeshVertAttr *va  = (MotoMeshVertAttr *)attr->data;
        MotoMeshVertAttr *va2 = moto_mesh_add_attr(self, va->name->str, va->chnum);

        for(i = 0; i < self->verts_num; i++)
        {
            for(j = 0; j < va->chnum; j++)
            {
                guint index = i*va->chnum + j;
                va2->data[index] = va->data[index];
            }
        }
    }

    return self;
}

MotoMeshVertAttr *moto_mesh_add_attr(MotoMesh *self, const gchar *attr_name, guint chnum)
{
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
    attr->data = (gfloat *)g_try_malloc(sizeof(gfloat)*self->verts_num);

    self->verts_attrs = g_slist_append(self->verts_attrs, attr);

    return attr;
}

MotoMeshVertAttr *moto_mesh_get_attr(MotoMesh *self, const gchar *attr_name)
{
    GSList *attr = self->verts_attrs;
    for(; attr; attr = g_slist_next(attr))
    {
        if(g_utf8_collate(attr_name, ((MotoMeshVertAttr *)attr->data)->name->str) == 0)
        {
            return (MotoMeshVertAttr *)attr->data;
        }
    }
    return NULL;
}

void moto_mesh_foreach_vertex(MotoMesh *self,
        MotoMeshForeachVertexFunc func)
{
    MotoMeshVert *vert;

    guint i;
    for(i = 0; i < self->verts_num; i++)
    {
        vert = & self->verts[i];
        func(self, vert);
    }
}

void moto_mesh_foreach_edge(MotoMesh *self,
        MotoMeshForeachEdgeFunc func)
{
    MotoMeshEdge *edge;

    guint i;
    for(i = 0; i < self->edges_num; i++)
    {
        edge = & self->edges[i];
        func(self, edge);
    }
}

typedef struct _FaceTessData
{
    MotoMesh *mesh;
    guint start_index,
          end_index;
} FaceTessData;

static gpointer face_tesselation_thread(FaceTessData *data)
{
    MotoMesh *mesh = data->mesh;

    guint i;
    for(i = data->start_index; i < data->end_index; i++)
        moto_mesh_face_tesselate(& mesh->faces[i], mesh);

    return NULL;
}

void moto_mesh_tesselate_faces(MotoMesh *self)
{
    if(self->faces_num < 5000)
    {
        /* Tesselation in the main thread. */
        guint i;
        for(i = 0; i < self->faces_num; i++)
            moto_mesh_face_tesselate(& self->faces[i], self);
    }
    else
    {
        /* Multi-threaded mesh face tesselation. */
        guint threads_num = (self->faces_num > 20000)?4:(self->faces_num > 15000)?3:2;
        GThread *threads[threads_num];
        FaceTessData tess_data[threads_num];

        guint faces_per_thread = self->faces_num / threads_num;

        guint i;
        for(i = 0; i < threads_num; i++)
        {
            tess_data[i].mesh = self;
            tess_data[i].start_index = i*faces_per_thread;
            tess_data[i].end_index = min(tess_data[i].start_index + faces_per_thread, self->faces_num);

            threads[i] = g_thread_create_full((GThreadFunc)face_tesselation_thread, & tess_data[i],
                                              524288, TRUE, TRUE, G_THREAD_PRIORITY_URGENT,  NULL);
        }
        for(i = 0; i < threads_num; i++)
            g_thread_join(threads[i]);
    }
}

void moto_mesh_face_hole_init(MotoMeshFaceHole *self, guint verts_num)
{
    self->verts_num = verts_num;
    self->indecies = (guint *)g_try_malloc(sizeof(guint) * self->verts_num);
}

void moto_mesh_face_init(MotoMeshFace *self,
        guint verts_num, guint holes_num)
{
    self->verts_num = verts_num;
    self->holes_num = holes_num;
    // self->triangles_num = triangles_num;

    self->indecies = (guint *)g_try_malloc(sizeof(guint) * self->verts_num);
    self->holes = (MotoMeshFaceHole *)g_try_malloc(sizeof(MotoMeshFaceHole) * self->holes_num);
    // self->triangles = (MotoTriangle *)g_try_malloc(sizeof(MotoTriangle) * self->triangles_num);
}

void moto_mesh_face_alloc(MotoMeshFace *self)
{
    self->indecies = (guint *)g_try_malloc(sizeof(guint) * self->verts_num);
}

void moto_mesh_face_free(MotoMeshFace *self)
{
    g_free(self->indecies);

    guint i;
    for(i = 0; i < self->holes_num; i++)
        g_free(self->holes[i].indecies);
    g_free(self->holes);

    if(self->triangles)
        g_free(self->triangles);
}

/* Newell's method */
void moto_mesh_face_calc_normal(MotoMeshFace *self, MotoMesh *mesh)
{
    self->normal[0] = 0;
    self->normal[1] = 0;
    self->normal[2] = 0;

    MotoMeshVert *vert, *nvert;

    guint i;
    for(i = 0; i < (self->verts_num-1); i++)
    {
        vert  = & mesh->verts[self->indecies[i]];
        nvert = & mesh->verts[self->indecies[i+1]];

        self->normal[0] += (vert->xyz[1] - nvert->xyz[1])*(vert->xyz[2] + nvert->xyz[2]);
        self->normal[1] += (vert->xyz[2] - nvert->xyz[2])*(vert->xyz[0] + nvert->xyz[0]);
        self->normal[2] += (vert->xyz[0] - nvert->xyz[0])*(vert->xyz[1] + nvert->xyz[1]);
    }

    gfloat lenbuf;
    vector3_normalize(self->normal, lenbuf);
}

typedef struct _Vector
{
    gfloat xyz[3];
} Vector;

void moto_mesh_face_tesselate(MotoMeshFace *self, MotoMesh *mesh)
{}

gboolean
moto_mesh_face_intersect_ray(MotoMeshFace *self,
        MotoMesh *mesh, MotoRay *ray, gfloat *dist)
{
    if( ! self->triangles)
    {
        /* TODO: Intersection with convex polygon. */
    }

    MotoTriangle *t;
    guint i;
    for(i = 0; i < self->triangles_num; i++)
    {
        t = & self->triangles[i];
        if(moto_ray_intersect_triangle_dist(ray, dist,
                    mesh->verts[t->a].xyz,
                    mesh->verts[t->b].xyz,
                    mesh->verts[t->c].xyz))
            return TRUE;
    }

    return FALSE;
}

void moto_mesh_face_foreach_vertex(MotoMeshFace *face,
        MotoMeshFaceForeachVertexFunc func, MotoMesh *mesh)
{
    MotoMeshVert *vert;

    guint i;
    for(i = 0; i < face->verts_num; i++)
    {
        vert = & mesh->verts[face->indecies[i]];
        func(face, vert);
    }
}

/* MeshSelection */

MotoMeshSelection *moto_mesh_selection_new(guint verts_num, guint edges_num, guint faces_num)
{
    guint i, num;

    MotoMeshSelection *self = \
        (MotoMeshSelection *)g_try_malloc(sizeof(MotoMeshSelection));

    self->verts_num = verts_num;
    self->edges_num = edges_num;
    self->faces_num = faces_num;

    num = verts_num/32 + 1;
    self->verts = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->verts[i] = 0;

    num = edges_num/32 + 1;
    self->edges = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->edges[i] = 0;

    num = faces_num/32 + 1;
    self->faces = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->faces[i] = 0;

    return self;
}

MotoMeshSelection *moto_mesh_selection_copy(MotoMeshSelection *other)
{
    guint i, num;

    MotoMeshSelection *self = \
        (MotoMeshSelection *)g_try_malloc(sizeof(MotoMeshSelection));

    self->verts_num = other->verts_num;
    self->edges_num = other->edges_num;
    self->faces_num = other->faces_num;

    num = self->verts_num/32 + 1;
    self->verts = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->verts[i] = other->verts[i];

    num = self->edges_num/32 + 1;
    self->edges = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->edges[i] = other->edges[i];

    num = self->faces_num/32 + 1;
    self->faces = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->faces[i] = other->faces[i];

    return self;
}

MotoMeshSelection *moto_mesh_selection_for_mesh(MotoMesh *mesh)
{
    return moto_mesh_selection_new(mesh->verts_num, mesh->edges_num, mesh->faces_num);
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
    if(index > (self->verts_num - 1))
        return;

    *(self->verts + (index/32)) |= 1 << (index % 32);
}

void moto_mesh_selection_deselect_vertex(MotoMeshSelection *self, guint index)
{
    if(index > (self->verts_num - 1))
        return;

    *(self->verts + (index/32)) &= ~(1 << (index % 32));
}

void moto_mesh_selection_deselect_all_verts(MotoMeshSelection *self)
{
    guint i;
    guint num = self->verts_num/32 + 1;
    for(i = 0; i < num; i++)
        self->verts[i] = 0;
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
    if(index > (self->verts_num - 1))
        return FALSE;

    return *(self->verts + (index/32)) & (1 << (index % 32));
}

/* edge selection */

void moto_mesh_selection_select_edge(MotoMeshSelection *self, guint index)
{
    if(index > (self->edges_num - 1))
        return;

    *(self->edges + (index/32)) |= 1 << (index % 32);
}

void moto_mesh_selection_deselect_edge(MotoMeshSelection *self, guint index)
{
    if(index > (self->edges_num - 1))
        return;

    *(self->edges + (index/32)) &= ~(1 << (index % 32));
}

void moto_mesh_selection_deselect_all_edges(MotoMeshSelection *self)
{
    guint i;
    guint num = self->edges_num/32 + 1;
    for(i = 0; i < num; i++)
        self->edges[i] = 0;
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
    if(index > (self->edges_num - 1))
        return FALSE;

    return *(self->edges + (index/32)) & (1 << (index % 32));
}

/* face selection */

void moto_mesh_selection_select_face(MotoMeshSelection *self, guint index)
{
    if(index > (self->faces_num - 1))
        return;

    *(self->faces + (index/32)) |= 1 << (index % 32);
}

void moto_mesh_selection_deselect_face(MotoMeshSelection *self, guint index)
{
    if(index > (self->faces_num - 1))
        return;

    *(self->faces + (index/32)) &= ~(1 << (index % 32));
}

void moto_mesh_selection_deselect_all_faces(MotoMeshSelection *self)
{
    guint i;
    guint num = self->faces_num/32 + 1;
    for(i = 0; i < num; i++)
        self->faces[i] = 0;
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
    if(index > (self->faces_num - 1))
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
    if(self->verts_num != mesh->verts_num || self->edges_num != mesh->edges_num || self->faces_num != mesh->faces_num)
        return FALSE;

    return TRUE;
}

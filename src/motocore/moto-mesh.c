#include "moto-mesh.h"
#include "moto-messager.h"
#include "common/matrix.h"

/* class Mesh */

static GObjectClass *mesh_parent_class = NULL;

static void
free_attr(gpointer data, gpointer user_data)
{
    MotoMeshVertexAttr *attr = (MotoMeshVertexAttr *)data;

    g_string_free(attr->name, TRUE);
    g_free(attr->data);
    g_slice_free(MotoMeshVertexAttr, attr);
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
    self->verts = (MotoMeshVertex *)g_try_malloc(sizeof(MotoMeshVertex) * verts_num);

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
        MotoMeshVertexAttr *va  = (MotoMeshVertexAttr *)attr->data;
        MotoMeshVertexAttr *va2 = moto_mesh_add_attr(self, va->name->str, va->chnum);

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

MotoMeshVertexAttr *moto_mesh_add_attr(MotoMesh *self, const gchar *attr_name, guint chnum)
{
    MotoMeshVertexAttr *attr = moto_mesh_get_attr(self, attr_name);

    if(attr)
    {
        GString *msg = g_string_new("Mesh already has attribute with name \"");
        g_string_append(msg, attr_name);
        g_string_append(msg, "\". I won't create it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return attr;
    }

    attr = g_slice_new(MotoMeshVertexAttr);

    attr->name = g_string_new(attr_name);
    attr->chnum = chnum;
    attr->data = (gfloat *)g_try_malloc(sizeof(gfloat)*self->verts_num);

    self->verts_attrs = g_slist_append(self->verts_attrs, attr);

    return attr;
}

MotoMeshVertexAttr *moto_mesh_get_attr(MotoMesh *self, const gchar *attr_name)
{
    GSList *attr = self->verts_attrs;
    for(; attr; attr = g_slist_next(attr))
    {
        if(g_utf8_collate(attr_name, ((MotoMeshVertexAttr *)attr->data)->name->str) == 0)
        {
            return (MotoMeshVertexAttr *)attr->data;
        }
    }
    return NULL;
}

void moto_mesh_foreach_vertex(MotoMesh *self,
        MotoMeshForeachVertexFunc func)
{
    MotoMeshVertex *vert;

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

void moto_mesh_face_alloc(MotoMeshFace *self)
{
    self->indecies = (guint *)g_try_malloc(sizeof(guint) * self->verts_num);
}

void moto_mesh_face_free(MotoMeshFace *self)
{
    g_free(self->indecies);
}

/* Newell's method */
void moto_mesh_face_calc_normal(MotoMeshFace *self, MotoMesh *mesh)
{
    self->normal[0] = 0;
    self->normal[1] = 0;
    self->normal[2] = 0;

    MotoMeshVertex *vert, *nvert;

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

#define index(i, vnum) ((i) % (vnum))

static gboolean has_concaves(Vector *crosses, guint num, gfloat normal[3])
{
    if(num <= 3)
        return FALSE;

    guint i;
    for(i = 0; i < num; i++)
    {
        if(vector3_dot(normal, crosses[i].xyz) < 0)
            return TRUE;
    }

    return FALSE;
}

void moto_mesh_face_tesselate(MotoMeshFace *self, MotoMesh *mesh)
{
    Vector crosses[self->verts_num];
    Vector a, b;
    gfloat lenbuf;
    guint vnum = self->verts_num;

    guint i;
    for(i = 0; i < self->verts_num; i++)
    {
        vector3_dif(a.xyz,
                    mesh->verts[self->indecies[index(i-1, vnum)]].xyz,
                    mesh->verts[self->indecies[index(i, vnum)]].xyz);
        vector3_dif(b.xyz,
                    mesh->verts[self->indecies[index(i, vnum)]].xyz,
                    mesh->verts[self->indecies[index(i+1, vnum)]].xyz);

        vector3_cross(crosses[i].xyz, a.xyz, b.xyz);
        vector3_normalize(crosses[i].xyz, lenbuf);
    }

    if(has_concaves(crosses, self->verts_num, self->normal))
    {
        /* Face is convex and may be drawn as is. */
        self->convexes = NULL;
        return;
    }
}

void moto_mesh_face_foreach_vertex(MotoMeshFace *face,
        MotoMeshFaceForeachVertexFunc func, MotoMesh *mesh)
{
    MotoMeshVertex *vert;

    guint i;
    for(i = 0; i < face->verts_num; i++)
    {
        vert = & mesh->verts[face->indecies[i]];
        func(face, vert);
    }
}

/* MeshSelectionMask */

MotoMeshSelection *moto_mesh_selection_new(guint verts_num, guint edges_num, guint faces_num)
{
    guint i, num;

    MotoMeshSelection *self = \
        (MotoMeshSelection *)g_try_malloc(sizeof(MotoMeshSelection));

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
    guint verts_num = other->verts_num;
    guint edges_num = other->edges_num;
    guint faces_num = other->faces_num;

    guint i, num;

    MotoMeshSelection *self = \
        (MotoMeshSelection *)g_try_malloc(sizeof(MotoMeshSelection));

    num = verts_num/32 + 1;
    self->verts = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->verts[i] = other->verts[i];

    num = edges_num/32 + 1;
    self->edges = (guint32 *)g_try_malloc(num * sizeof(guint32));
    for(i = 0; i < num; i++)
        self->edges[i] = other->edges[i];

    num = faces_num/32 + 1;
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

    return *(self->verts + (index/8)) & (1 << (index % 8));
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

/* is selection valid */

gboolean moto_mesh_selection_is_valid(MotoMeshSelection *self, MotoMesh *mesh)
{
    if(self->verts_num != mesh->verts_num || self->edges_num != mesh->edges_num || self->faces_num != mesh->faces_num)
        return FALSE;

    return TRUE;
}

#include "moto-mesh.h"
#include "moto-messager.h"

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
    if(moto_mesh_get_attr(self, attr_name))
    {
        GString *msg = g_string_new("Mesh already has attribute with name \"");
        g_string_append(msg, attr_name);
        g_string_append(msg, "\". I won't create it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    MotoMeshVertexAttr *attr = g_slice_new(MotoMeshVertexAttr);

    attr->name = g_string_new(attr_name);
    attr->chnum = chnum;
    attr->data = (gfloat *)g_try_malloc(sizeof(gfloat)*self->verts_num);

    self->verts_attrs = g_slist_append(self->verts_attrs, attr);
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


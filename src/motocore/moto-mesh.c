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
    self->verts_attrs = NULL;
}

static void
moto_mesh_class_init(MotoMeshClass *klass)
{
    mesh_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    mesh_parent_class->dispose = moto_mesh_dispose;
    mesh_parent_class->finalize = moto_mesh_finalize;
}

G_DEFINE_TYPE(MotoMesh, moto_mesh, G_TYPE_OBJECT);

/* methods of class Mesh */

MotoMesh *moto_mesh_new(guint verts_num)
{
    MotoMesh *self = (MotoMesh *)g_object_new(MOTO_TYPE_MESH, NULL);

    self->verts_num = verts_num;
    self->verts = (MotoMeshVertex *)g_try_malloc(verts_num*sizeof(MotoMeshVertex));

    return self;
}

void moto_mesh_add_attr(MotoMesh *self, const gchar *attr_name, guint chnum)
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
        if(g_utf8_collate(attr_name, ((MotoMeshVertexAttr *)attr->data)->name->str))
        {
            return (MotoMeshVertexAttr *)attr->data;
        }
    }
    return NULL;
}


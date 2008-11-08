#include "moto-mesh-loader.h"

/* forwards */

static gboolean __moto_mesh_loader_can_virtual(MotoMeshLoader *self, const gchar *filename);

/* class MeshLoader */

static GObjectClass *mesh_loader_parent_class = NULL;

static void
moto_mesh_loader_dispose(GObject *obj)
{
    mesh_loader_parent_class->dispose(obj);
}

static void
moto_mesh_loader_finalize(GObject *obj)
{
    mesh_loader_parent_class->finalize(obj);
}

static void
moto_mesh_loader_init(MotoMeshLoader *self)
{}

static void
moto_mesh_loader_class_init(MotoMeshLoaderClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    mesh_loader_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_loader_dispose;
    goclass->finalize   = moto_mesh_loader_finalize;

    klass->get_extensions   = NULL;
    klass->can              = __moto_mesh_loader_can_virtual;
    klass->load             = NULL;
}

G_DEFINE_ABSTRACT_TYPE(MotoMeshLoader, moto_mesh_loader, G_TYPE_OBJECT);

/* methods of class MeshLoader */

GSList *moto_mesh_loader_get_extentions(MotoMeshLoader *self)
{
    MotoMeshLoaderClass *klass = MOTO_MESH_LOADER_GET_CLASS(self);

    if(klass->get_extensions)
        return klass->get_extensions(self);

    return NULL;
}

gboolean moto_mesh_loader_can(MotoMeshLoader *self, const gchar *filename)
{
    MotoMeshLoaderClass *klass = MOTO_MESH_LOADER_GET_CLASS(self);

    if(klass->can)
        return klass->can(self, filename);

    return FALSE;
}

MotoMesh *moto_mesh_loader_load(MotoMeshLoader *self, const gchar *filename)
{
    MotoMeshLoaderClass *klass = MOTO_MESH_LOADER_GET_CLASS(self);

    if(klass->load)
        return klass->load(self, filename);

    return NULL;
}

static gboolean __moto_mesh_loader_can_virtual(MotoMeshLoader *self, const gchar *filename)
{
    GString *ext;
    gchar *ext2;
    glong len = g_utf8_strlen(filename, -1);

    const GSList *ext_list = moto_mesh_loader_get_extentions(self);
    for(; ext_list; ext_list = g_slist_next(ext_list))
    {
        ext = (GString *)ext_list->data;

        if(len < ext->len)
            continue;

        ext2 = g_utf8_offset_to_pointer(filename, len - ext->len);

        if(g_utf8_collate(ext->str, ext2) == 0)
        {
            g_print("TRUE\n");
            return TRUE;
        }
    }

    return FALSE;
}

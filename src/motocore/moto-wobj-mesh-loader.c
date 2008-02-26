#include "moto-wobj-mesh-loader.h"

/* forwards */

GSList *moto_wobj_mesh_loader_get_extentions(MotoMeshLoader *self);
MotoMesh *moto_wobj_mesh_loader_load(MotoMeshLoader *self, const gchar *filename);

/* class WobjMeshLoader */

static GObjectClass *wobj_mesh_loader_parent_class = NULL;

struct _MotoWobjMeshLoaderPriv
{
    GSList *extensions;
};

static void
free_gstring(gpointer data, gpointer user_data)
{
    g_string_free((GString *)data, TRUE);
}

static void
moto_wobj_mesh_loader_dispose(GObject *obj)
{
    MotoWobjMeshLoader *wobj = (MotoWobjMeshLoader *)obj;

    g_slist_foreach(wobj->priv->extensions, free_gstring, NULL);
    g_slist_free(wobj->priv->extensions);

    wobj_mesh_loader_parent_class->dispose(obj);
}

static void
moto_wobj_mesh_loader_finalize(GObject *obj)
{
    wobj_mesh_loader_parent_class->finalize(obj);
}

static void
moto_wobj_mesh_loader_init(MotoWobjMeshLoader *self)
{
    self->priv = g_slice_new(MotoWobjMeshLoaderPriv);

    self->priv->extensions = NULL;
    self->priv->extensions = \
        g_slist_append(self->priv->extensions, g_string_new(".obj"));
}

static void
moto_wobj_mesh_loader_class_init(MotoWobjMeshLoaderClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoMeshLoaderClass *mlclass = (MotoMeshLoaderClass *)klass;

    wobj_mesh_loader_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_wobj_mesh_loader_dispose;
    goclass->finalize   = moto_wobj_mesh_loader_finalize;

    mlclass->get_extensions   = moto_wobj_mesh_loader_get_extentions;
    // klass->can              = moto_wobj_mesh_loader_can;
    mlclass->load             = moto_wobj_mesh_loader_load;
}

G_DEFINE_TYPE(MotoWobjMeshLoader, moto_wobj_mesh_loader, MOTO_TYPE_MESH_LOADER);

/* methods of class WobjMeshLoader */

MotoMeshLoader *moto_wobj_mesh_loader_new()
{
    MotoMeshLoader *self = (MotoMeshLoader *)g_object_new(MOTO_TYPE_WOBJ_MESH_LOADER, NULL);
    // MotoWobjMeshLoader *wobj = (MotoWobjMeshLoader *)self;

    return self;
}

GSList *moto_wobj_mesh_loader_get_extentions(MotoMeshLoader *self)
{

    return ((MotoWobjMeshLoader *)self)->priv->extensions;
}

/*
gboolean moto_wobj_mesh_loader_can(MotoWobjMeshLoader *self, const gchar *filename)
{


    return FALSE;
}
*/

MotoMesh *moto_wobj_mesh_loader_load(MotoMeshLoader *self, const gchar *filename)
{


    return NULL;
}

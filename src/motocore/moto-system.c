#include "moto-system.h"
#include "moto-object-node.h"
#include "moto-time-node.h"

/* class System */

static GObjectClass *system_parent_class = NULL;

struct _MotoSystemPriv
{
    MotoLibrary *library;
};

static void
moto_system_dispose(GObject *obj)
{
    MotoSystem *self = (MotoSystem *)obj;

    g_string_free(self->priv->filename, TRUE);
    g_slice_free(MotoSystemPriv, self->priv);

    G_OBJECT_CLASS(system_parent_class)->dispose(obj);
}

static void
moto_system_finalize(GObject *obj)
{
    system_parent_class->finalize(obj);
}

static void
moto_system_init(MotoSystem *self)
{
    self->priv = g_slice_new(MotoSystemPriv);

    self->priv->filename = g_string_new("");
    self->priv->changed = FALSE;
}

static void
moto_system_class_init(MotoSystemClass *klass)
{
    system_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    system_parent_class->dispose = moto_system_dispose;
    system_parent_class->finalize = moto_system_finalize;
}

G_DEFINE_TYPE(MotoSystem, moto_system, G_TYPE_OBJECT);

MotoSystem *moto_system_new(const gchar *name, MotoLibrary *lib)
{
    MotoSystem *self = (MotoSystem *)g_object_new(MOTO_TYPE_SYSTEM, NULL);

    MotoLibrary *lib = self->priv->library = moto_library_new();

    moto_library_new_slot(lib, "node", MOTO_TYPE_NODE_FACTORY);
    moto_library_new_slot(lib, "mesh-loader", MOTO_TYPE_MESH_LOADER);

    return NULL;
}

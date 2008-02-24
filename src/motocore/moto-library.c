#include "moto-library.h"

/* class Slot */

typedef struct _Slot Slot;

struct _Slot
{
    GType type;
    GData *dl;
};

static Slot *slot_new(GType type)
{
    Slot *self = (Slot *)g_try_malloc(sizeof(Slot));
    self->type = type;
    g_datalist_init(& self->dl);

    return self;
}

/*
static void slot_delete(Slot *self)
{
    g_datalist_clear(& self->dl);
    g_free(self);
}
*/

static gboolean slot_check_entry(Slot *self, gpointer entry)
{
    return G_TYPE_CHECK_INSTANCE_TYPE(entry, self->type);
}

static void slot_new_entry(Slot *self, const gchar *name, gpointer entry)
{
    g_assert(slot_check_entry(self, entry));
    g_datalist_set_data(& self->dl, name, entry);
}

static  gpointer slot_get_entry(Slot *self, const gchar *name)
{
    return g_datalist_get_data(& self->dl, name);
}

/* class Library */

static GObjectClass *library_parent_class = NULL;

struct _MotoLibraryPriv
{
    GData *slots;
};

static void
moto_library_dispose(GObject *obj)
{
    MotoLibrary *self = (MotoLibrary *)obj;

    g_datalist_clear(& self->priv->slots);
    g_slice_free(MotoLibraryPriv, self->priv);

    G_OBJECT_CLASS(library_parent_class)->dispose(obj);
}

static void
moto_library_finalize(GObject *obj)
{
    library_parent_class->finalize(obj);
}

static void
moto_library_init(MotoLibrary *self)
{
    self->priv = g_slice_new(MotoLibraryPriv);

    g_datalist_init(& self->priv->slots);

}

static void
moto_library_class_init(MotoLibraryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    library_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_library_dispose;
    goclass->finalize   = moto_library_finalize;
}

G_DEFINE_TYPE(MotoLibrary, moto_library, G_TYPE_OBJECT);

/* methods of class Library */

MotoLibrary *moto_library_new()
{
    MotoLibrary *self = (MotoLibrary *)g_object_new(MOTO_TYPE_LIBRARY, NULL);

    return self;
}

void moto_library_new_slot(MotoLibrary *self, const gchar *slot_name, GType type)
{
    if(g_datalist_get_data(& self->priv->slots, slot_name))
    {
        /* Warning! */
        return;
    }

    Slot *slot = slot_new(type);
    g_datalist_set_data(& self->priv->slots, slot_name, slot);
}

gboolean moto_library_new_entry(MotoLibrary *self,
        const gchar *slot_name, const gchar *entry_name, gpointer entry)
{
    Slot *slot = g_datalist_get_data(& self->priv->slots, slot_name);
    if( ! slot)
    {
        /* Warning! */
        return FALSE;
    }

    if( ! slot_check_entry(slot, entry))
    {
        /* Warning! */
        return FALSE;
    }

    if(slot_get_entry(slot, entry_name))
    {
        /* Warning! */
        return FALSE;
    }

    slot_new_entry(slot, entry_name, entry);

    return TRUE;
}

gpointer moto_library_get_entry(MotoLibrary *self,
        const gchar *slot_name, const gchar *entry_name)
{
    Slot *slot = g_datalist_get_data(& self->priv->slots, slot_name);
    if( ! slot)
    {
        /* Warning! */
        return NULL;
    }

    return slot_get_entry(slot, entry_name);
}

/* WARNING: This is NOT thread-safe!  */

static MotoLibraryForeachFunc lib_foreach_callback = NULL;

static void
lib_foreach(GQuark key_id, gpointer data, gpointer user_data)
{
    if(lib_foreach_callback)
        if(lib_foreach_callback(data, user_data))
            lib_foreach_callback = NULL;
}

void moto_library_foreach(MotoLibrary *self, const gchar *slot_name,
        MotoLibraryForeachFunc func, gpointer user_data)
{
    Slot *slot = g_datalist_get_data(& self->priv->slots, slot_name);
    if( ! slot)
    {
        /* Warning! */
        return;
    }

    lib_foreach_callback = func;
    g_datalist_foreach(& slot->dl, lib_foreach, user_data);
}

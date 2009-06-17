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

static void slot_free(Slot *self)
{
    g_datalist_clear(& self->dl);
    g_free(self);
}

static gboolean slot_check_entry(Slot *self, gpointer entry)
{
    return G_TYPE_CHECK_INSTANCE_TYPE(entry, self->type);
}

static void slot_new_entry(Slot *self, const gchar *name, gpointer entry)
{
    g_assert(slot_check_entry(self, entry));
    g_datalist_set_data_full(& self->dl, name, entry, (GDestroyNotify)g_object_unref);
}

static gpointer slot_get_entry(Slot *self, const gchar *name)
{
    return g_datalist_get_data(& self->dl, name);
}

/* class Library */

static GObjectClass *library_parent_class = NULL;

struct _MotoLibraryPriv
{
    GData *slots;
    GMutex *new_slot_mutex;
    GMutex *new_entry_mutex;
};

static void
moto_library_dispose(GObject *obj)
{
    MotoLibrary *self = (MotoLibrary *)obj;

    /* Slots are deleted by destroy_func whicj has been set while adding data ti datalist. */
    g_datalist_clear(& self->priv->slots);

    g_mutex_free(self->priv->new_slot_mutex);
    g_mutex_free(self->priv->new_entry_mutex);

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
    self->priv->new_slot_mutex = g_mutex_new();
    self->priv->new_entry_mutex = g_mutex_new();

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

/* Methods of class Library */

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

    g_mutex_lock(self->priv->new_slot_mutex);
    Slot *slot = slot_new(type);
    g_datalist_set_data_full(& self->priv->slots, slot_name, slot, (GDestroyNotify)slot_free);
    g_mutex_unlock(self->priv->new_slot_mutex);
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

    g_mutex_lock(self->priv->new_entry_mutex);
    slot_new_entry(slot, entry_name, entry);
    g_mutex_unlock(self->priv->new_entry_mutex);

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

typedef struct _LibForeachUserData
{
    MotoLibraryForeachFunc callback;
    gpointer user_data;
} LibForeachUserData;

static void
lib_foreach(GQuark key_id, gpointer data, gpointer user_data)
{
    LibForeachUserData *lfud = (LibForeachUserData *)user_data;
    if(lfud->callback)
        if(lfud->callback(data, lfud->user_data))
            lfud->callback = NULL;
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

    LibForeachUserData lfud = {func, user_data};
    g_datalist_foreach(& slot->dl, lib_foreach, & lfud);
}

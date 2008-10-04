#include "moto-mapped-list.h"

void moto_mapped_list_init(MotoMappedList *self)
{
    self->sl = NULL;
    g_datalist_init(& self->dl);
}

GSList *moto_mapped_list_find(MotoMappedList *self, gconstpointer data)
{
    return g_slist_find(self->sl, data);
}

void moto_mapped_list_set(MotoMappedList *self, const gchar *k, gpointer data)
{
    gpointer d = g_datalist_get_data(& self->dl, k);
    if(d)
    {
        GSList *l = g_slist_find(self->sl, d);
        if(l)
            l->data = data;
    }
    else
    {
        self->sl = g_slist_append(self->sl, data);
    }
    g_datalist_set_data(& self->dl, k, data);
}

gpointer moto_mapped_list_get(MotoMappedList *self, const gchar *k)
{
    return g_datalist_get_data(& self->dl, k);
}

void moto_mapped_list_remove_data(MotoMappedList *self, const gchar *k)
{
    GQuark q = g_quark_from_string(k);
    gpointer data = g_datalist_id_get_data(& self->dl, q);
    if( ! data)
        return;
    self->sl = g_slist_remove(self->sl, data);
    g_datalist_id_remove_data(& self->dl, q);
}

void moto_mapped_list_foreach(MotoMappedList *self, GFunc func, gpointer user_data)
{
    g_slist_foreach(self->sl, func, user_data);
}

void moto_mapped_list_free(MotoMappedList *self)
{
    g_slist_free(self->sl);
    g_datalist_clear(& self->dl);
}

void moto_mapped_list_free_all(MotoMappedList *self, GFunc data_delete_func)
{
    g_assert(data_delete_func);
    g_slist_foreach(self->sl, data_delete_func, NULL);
    moto_mapped_list_free(self);
}

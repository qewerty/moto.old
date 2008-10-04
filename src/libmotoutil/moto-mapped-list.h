#include <glib.h>

typedef struct _MotoMappedList MotoMappedList;

struct _MotoMappedList
{
    GSList *sl;
    GData  *dl;
};

void moto_mapped_list_init(MotoMappedList *self);
GSList *moto_mapped_list_find(MotoMappedList *self, gconstpointer data);
void moto_mapped_list_set(MotoMappedList *self, const gchar *k, gpointer data);
gpointer moto_mapped_list_get(MotoMappedList *self, const gchar *k);
void moto_mapped_list_remove_data(MotoMappedList *self, const gchar *k);
void moto_mapped_list_foreach(MotoMappedList *self, GFunc func, gpointer user_data);
void moto_mapped_list_free(MotoMappedList *self);
void moto_mapped_list_free_all(MotoMappedList *self, GFunc data_delete_func);

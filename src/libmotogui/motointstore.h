/* Adapted from The GIMP */

#ifndef __MOTO_INT_STORE_H__
#define __MOTO_INT_STORE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum
{
  MOTO_INT_STORE_VALUE,
  MOTO_INT_STORE_LABEL,
  MOTO_INT_STORE_STOCK_ID,
  MOTO_INT_STORE_PIXBUF,
  MOTO_INT_STORE_USER_DATA,
  MOTO_INT_STORE_NUM_COLUMNS
} MotoIntStoreColumns;

#define MOTO_TYPE_INT_STORE            (moto_int_store_get_type ())
#define MOTO_INT_STORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_INT_STORE, MotoIntStore))
#define MOTO_INT_STORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_INT_STORE, MotoIntStoreClass))
#define MOTO_IS_INT_STORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_INT_STORE))
#define MOTO_IS_INT_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_INT_STORE))
#define MOTO_INT_STORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_INT_STORE, MotoIntStoreClass))

typedef struct _MotoIntStore      MotoIntStore;
typedef struct _MotoIntStoreClass MotoIntStoreClass;

struct _MotoIntStore
{
  GtkListStore  parent;

  /*< private >*/
  GtkTreeIter  *empty_iter;
};

struct _MotoIntStoreClass
{
  GtkListStoreClass  parent;
};


GType          moto_int_store_get_type       (void) G_GNUC_CONST;
GtkListStore * moto_int_store_new            (void);
gboolean       moto_int_store_lookup_by_value(GtkTreeModel  *model,
                                              gint           value,
                                              GtkTreeIter   *iter);


G_END_DECLS

#endif  /* __MOTO_INT_STORE_H__ */

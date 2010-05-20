/* Adapted from The GIMP */

#ifndef __MOTO_INTSTORE_H__
#define __MOTO_INTSTORE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum
{
  MOTO_INTSTORE_VALUE,
  MOTO_INTSTORE_LABEL,
  MOTO_INTSTORE_STOCK_ID,
  MOTO_INTSTORE_PIXBUF,
  MOTO_INTSTORE_USER_DATA,
  MOTO_INTSTORE_NUM_COLUMNS
} MotoIntStoreColumns;

#define MOTO_TYPE_INTSTORE            (moto_intstore_get_type ())
#define MOTO_INTSTORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_INTSTORE, MotoIntStore))
#define MOTO_INTSTORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_INTSTORE, MotoIntStoreClass))
#define MOTO_IS_INTSTORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_INTSTORE))
#define MOTO_IS_INTSTORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_INTSTORE))
#define MOTO_INTSTORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_INTSTORE, MotoIntStoreClass))

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


GType          moto_intstore_get_type       (void) G_GNUC_CONST;
GtkListStore * moto_intstore_new            (void);
gboolean       moto_intstore_lookup_by_value(GtkTreeModel  *model,
                                              gint           value,
                                              GtkTreeIter   *iter);


G_END_DECLS

#endif  /* __MOTO_INTSTORE_H__ */

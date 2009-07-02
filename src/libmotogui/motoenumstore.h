#ifndef __MOTO_ENUM_STORE_H__
#define __MOTO_ENUM_STORE_H__

#include "motointstore.h"

G_BEGIN_DECLS

#define MOTO_TYPE_ENUM_STORE            (moto_enum_store_get_type ())
#define MOTO_ENUM_STORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_ENUM_STORE, MotoEnumStore))
#define MOTO_ENUM_STORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_ENUM_STORE, MotoEnumStoreClass))
#define MOTO_IS_ENUM_STORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_ENUM_STORE))
#define MOTO_IS_ENUM_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_ENUM_STORE))
#define MOTO_ENUM_STORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_ENUM_STORE, MotoEnumStoreClass))

typedef struct _MotoEnumStore       MotoEnumStore;
typedef struct _MotoEnumStoreClass  MotoEnumStoreClass;

struct _MotoEnumStore
{
  MotoIntStore       parent_instance;

  GEnumClass        *enum_class;
};

struct _MotoEnumStoreClass
{
  MotoIntStoreClass  parent_class;
};

GType          moto_enum_store_get_type               (void) G_GNUC_CONST;

GtkListStore * moto_enum_store_new                    (GType    enum_type);
GtkListStore * moto_enum_store_new_with_range         (GType    enum_type,
                                                       gint     minimum,
                                                       gint     maximum);
GtkListStore * moto_enum_store_new_with_values        (GType    enum_type,
                                                       gint     n_values,
                                                       ...);
GtkListStore * moto_enum_store_new_with_values_valist (GType    enum_type,
                                                       gint     n_values,
                                                       va_list  args);

void           moto_enum_store_set_stock_prefix (MotoEnumStore *store,
                                                 const gchar   *stock_prefix);


G_END_DECLS

#endif  /* __MOTO_ENUM_STORE_H__ */

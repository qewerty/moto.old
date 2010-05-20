#include <string.h>
#include "motointstore.h"

enum
{
    PROP_0,
    PROP_USER_DATA_TYPE
};

typedef struct
{
  GType  user_data_type;
} MotoIntStorePrivate;

static GObject * moto_intstore_constructor(GType                  type,
                                            guint                  n_params,
                                            GObjectConstructParam *params);

static void  moto_intstore_tree_model_init(GtkTreeModelIface *iface);

static void  moto_intstore_finalize       (GObject           *object);
static void  moto_intstore_set_property   (GObject           *object,
                                            guint              property_id,
                                            const GValue      *value,
                                            GParamSpec        *pspec);
static void  moto_intstore_get_property   (GObject           *object,
                                            guint              property_id,
                                            GValue            *value,
                                            GParamSpec        *pspec);

static void  moto_intstore_row_inserted   (GtkTreeModel      *model,
                                            GtkTreePath       *path,
                                            GtkTreeIter       *iter);
static void  moto_intstore_add_empty      (MotoIntStore      *store);


G_DEFINE_TYPE_WITH_CODE(MotoIntStore, moto_intstore, GTK_TYPE_LIST_STORE,
                        G_IMPLEMENT_INTERFACE(GTK_TYPE_TREE_MODEL,
                                              moto_intstore_tree_model_init))

#define MOTO_INTSTORE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_INTSTORE, MotoIntStorePrivate)

#define parent_class moto_intstore_parent_class

static GtkTreeModelIface *parent_iface = NULL;

static void
moto_intstore_class_init(MotoIntStoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructor  = moto_intstore_constructor;
  object_class->finalize     = moto_intstore_finalize;
  object_class->set_property = moto_intstore_set_property;
  object_class->get_property = moto_intstore_get_property;

  /**
   * MotoIntStore:user-data-type:
   *
   * Allows to set the #GType for the MOTO_INTSTORE_USER_DATA column.
   *
   * You need to set this property when constructing the store if you want
   * to use the MOTO_INTSTORE_USER_DATA column and want to have the store
   * handle ref-counting of your user data.
   *
   * Since: MOTO 2.4
   */
  g_object_class_install_property(object_class,
                                  PROP_USER_DATA_TYPE,
                                  g_param_spec_gtype("user-data-type",
                                                     NULL, NULL,
                                                     G_TYPE_NONE,
                                                     G_PARAM_CONSTRUCT_ONLY |
                                                     G_PARAM_READWRITE));

  g_type_class_add_private(object_class, sizeof (MotoIntStorePrivate));
}

static void
moto_intstore_init(MotoIntStore *store)
{
    store->empty_iter = NULL;
}

static GObject *
moto_intstore_constructor(GType                  type,
                           guint                  n_params,
                           GObjectConstructParam *params)
{
    GObject             *object;
    MotoIntStore        *store;
    MotoIntStorePrivate *priv;
    GType                types[MOTO_INTSTORE_NUM_COLUMNS];

    object = G_OBJECT_CLASS(parent_class)->constructor(type, n_params, params);

    store = MOTO_INTSTORE(object);
    priv  = MOTO_INTSTORE_GET_PRIVATE(store);

    types[MOTO_INTSTORE_VALUE]     = G_TYPE_INT;
    types[MOTO_INTSTORE_LABEL]     = G_TYPE_STRING;
    types[MOTO_INTSTORE_STOCK_ID]  = G_TYPE_STRING;
    types[MOTO_INTSTORE_PIXBUF]    = GDK_TYPE_PIXBUF;
    types[MOTO_INTSTORE_USER_DATA] = (priv->user_data_type != G_TYPE_NONE ?
                                       priv->user_data_type : G_TYPE_POINTER);

    gtk_list_store_set_column_types(GTK_LIST_STORE(store),
                                    MOTO_INTSTORE_NUM_COLUMNS, types);

    moto_intstore_add_empty(store);

    return object;
}

static void
moto_intstore_tree_model_init(GtkTreeModelIface *iface)
{
  parent_iface = g_type_interface_peek_parent(iface);

  iface->row_inserted = moto_intstore_row_inserted;
}

static void
moto_intstore_finalize(GObject *object)
{
    MotoIntStore *store = MOTO_INTSTORE (object);

    if (store->empty_iter)
    {
        gtk_tree_iter_free(store->empty_iter);
        store->empty_iter = NULL;
    }

    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void
moto_intstore_set_property(GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    MotoIntStorePrivate *priv = MOTO_INTSTORE_GET_PRIVATE(object);

    switch(property_id)
    {
        case PROP_USER_DATA_TYPE:
            priv->user_data_type = g_value_get_gtype(value);
        break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
moto_intstore_get_property(GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    MotoIntStorePrivate *priv = MOTO_INTSTORE_GET_PRIVATE(object);

    switch(property_id)
    {
        case PROP_USER_DATA_TYPE:
            g_value_set_gtype(value, priv->user_data_type);
        break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
moto_intstore_row_inserted(GtkTreeModel *model,
                            GtkTreePath  *path,
                            GtkTreeIter  *iter)
{
    MotoIntStore *store = MOTO_INTSTORE(model);

    if(parent_iface->row_inserted)
        parent_iface->row_inserted(model, path, iter);

    if(store->empty_iter &&
       memcmp(iter, store->empty_iter, sizeof (GtkTreeIter)))
    {
        gtk_list_store_remove(GTK_LIST_STORE (store), store->empty_iter);
        gtk_tree_iter_free(store->empty_iter);
        store->empty_iter = NULL;
    }
}

static void
moto_intstore_add_empty(MotoIntStore *store)
{
  GtkTreeIter iter = {0,};

  g_return_if_fail(store->empty_iter == NULL);

  gtk_list_store_prepend(GTK_LIST_STORE(store), &iter);
  gtk_list_store_set(GTK_LIST_STORE(store), &iter,
                     MOTO_INTSTORE_VALUE, -1,
                     MOTO_INTSTORE_LABEL, ("(Empty)"),
                     -1);

  store->empty_iter = gtk_tree_iter_copy(&iter);
}

/**
 * moto_intstore_new:
 *
 * Creates a #GtkListStore with a number of useful columns.
 * #MotoIntStore is especially useful if the items you want to store
 * are identified using an integer value.
 *
 * Return value: a new #MotoIntStore.
 *
 * Since: MOTO 2.2
 **/
GtkListStore *
moto_intstore_new(void)
{
  return g_object_new(MOTO_TYPE_INTSTORE, NULL);
}

/**
 * moto_intstore_lookup_by_value:
 * @model: a #MotoIntStore
 * @value: an integer value to lookup in the @model
 * @iter:  return location for the iter of the given @value
 *
 * Iterate over the @model looking for @value.
 *
 * Return value: %TRUE if the value has been located and @iter is
 *               valid, %FALSE otherwise.
 *
 * Since: MOTO 2.2
 **/
gboolean
moto_intstore_lookup_by_value(GtkTreeModel *model,
                               gint          value,
                               GtkTreeIter  *iter)
{
    gboolean iter_valid;

    g_return_val_if_fail(GTK_IS_TREE_MODEL (model), FALSE);
    g_return_val_if_fail(iter != NULL, FALSE);

    for(iter_valid = gtk_tree_model_get_iter_first(model, iter);
        iter_valid;
        iter_valid = gtk_tree_model_iter_next(model, iter))
    {
        gint this;

        gtk_tree_model_get(model, iter,
                           MOTO_INTSTORE_VALUE, & this,
                           -1);
        if(this == value)
            break;
    }

    return iter_valid;
}

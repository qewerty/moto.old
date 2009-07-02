#include "motoenumstore.h"

enum
{
    PROP_0,
    PROP_ENUM_TYPE
};

static void   moto_enum_store_finalize    (GObject      *object);
static void   moto_enum_store_set_property(GObject      *object,
                                           guint         property_id,
                                           const GValue *value,
                                           GParamSpec   *pspec);
static void   moto_enum_store_get_property(GObject      *object,
                                           guint         property_id,
                                           GValue       *value,
                                           GParamSpec   *pspec);

static void   moto_enum_store_add_value   (GtkListStore *store,
                                           GEnumValue   *value);


G_DEFINE_TYPE(MotoEnumStore, moto_enum_store, MOTO_TYPE_INT_STORE)

#define parent_class moto_enum_store_parent_class

static void
moto_enum_store_class_init(MotoEnumStoreClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize     = moto_enum_store_finalize;
    object_class->set_property = moto_enum_store_set_property;
    object_class->get_property = moto_enum_store_get_property;

    /**
     * MotoEnumStore:enum-type:
     *
     * Sets the #GType of the enum to be used in the store.
     *
     * Since: MOTO 2.4
     */
    g_object_class_install_property(object_class,
                                    PROP_ENUM_TYPE,
                                    g_param_spec_gtype("enum-type",
                                                       NULL, NULL,
                                                       G_TYPE_ENUM,
                                                       G_PARAM_CONSTRUCT_ONLY |
                                                       G_PARAM_READWRITE));
}

static void
moto_enum_store_init(MotoEnumStore *store)
{}

static void
moto_enum_store_finalize(GObject *object)
{
    MotoEnumStore *store = MOTO_ENUM_STORE (object);

    if (store->enum_class)
        g_type_class_unref(store->enum_class);

    G_OBJECT_CLASS(parent_class)->finalize (object);
}

static void
moto_enum_store_set_property(GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
    MotoEnumStore *store = MOTO_ENUM_STORE(object);

    switch (property_id)
    {
        case PROP_ENUM_TYPE:
            g_return_if_fail(store->enum_class == NULL);
            store->enum_class = g_type_class_ref(g_value_get_gtype(value));
        break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
moto_enum_store_get_property(GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
    MotoEnumStore *store = MOTO_ENUM_STORE(object);

    switch (property_id)
    {
        case PROP_ENUM_TYPE:
            g_value_set_gtype(value, (store->enum_class ?
                              G_TYPE_FROM_CLASS (store->enum_class) :
                              G_TYPE_NONE));
        break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
moto_enum_store_add_value(GtkListStore *store,
                          GEnumValue   *value)
{
    GtkTreeIter  iter;

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       MOTO_INT_STORE_VALUE, value->value,
                       MOTO_INT_STORE_LABEL, value->value_nick,
                       -1);
}


/**
 * moto_enum_store_new:
 * @enum_type: the #GType of an enum.
 *
 * Creates a new #MotoEnumStore, derived from #GtkListStore and fills
 * it with enum values. The enum needs to be registered to the type
 * system and should have translatable value names.
 *
 * Return value: a new #MotoEnumStore.
 *
 * Since: MOTO 2.4
 **/
GtkListStore *
moto_enum_store_new(GType enum_type)
{
  GtkListStore *store;
  GEnumClass   *enum_class;

  g_return_val_if_fail(G_TYPE_IS_ENUM(enum_type), NULL);

  enum_class = g_type_class_ref(enum_type);

  store = moto_enum_store_new_with_range(enum_type,
                                         enum_class->minimum,
                                         enum_class->maximum);

  g_type_class_unref(enum_class);

  return store;
}

/**
 * moto_enum_store_new_with_range:
 * @enum_type: the #GType of an enum.
 * @minimum: the minimum value to include
 * @maximum: the maximum value to include
 *
 * Creates a new #MotoEnumStore like moto_enum_store_new() but allows
 * to limit the enum values to a certain range. Values smaller than
 * @minimum or larger than @maximum are not added to the store.
 *
 * Return value: a new #MotoEnumStore.
 *
 * Since: MOTO 2.4
 **/
GtkListStore *
moto_enum_store_new_with_range(GType  enum_type,
                               gint   minimum,
                               gint   maximum)
{
    GtkListStore *store;
    GEnumValue   *value;

    g_return_val_if_fail(G_TYPE_IS_ENUM(enum_type), NULL);

    store = g_object_new(MOTO_TYPE_ENUM_STORE,
                         "enum-type", enum_type,
                         NULL);

    for(value = MOTO_ENUM_STORE (store)->enum_class->values;
        value->value_name;
        value++)
    {
        if (value->value < minimum || value->value > maximum)
            continue;

        moto_enum_store_add_value (store, value);
    }

    return store;
}

/**
 * moto_enum_store_new_with_values
 * @enum_type: the #GType of an enum.
 * @n_values:  the number of enum values to include
 * @...:       a list of enum values (exactly @n_values)
 *
 * Creates a new #MotoEnumStore like moto_enum_store_new() but allows
 * to expliticely list the enum values that should be added to the
 * store.
 *
 * Return value: a new #MotoEnumStore.
 *
 * Since: MOTO 2.4
 **/
GtkListStore *
moto_enum_store_new_with_values(GType enum_type,
                                gint  n_values,
                                ...)
{
    GtkListStore *store;
    va_list       args;

    va_start(args, n_values);

    store = moto_enum_store_new_with_values_valist(enum_type, n_values, args);

    va_end(args);

    return store;
}

/**
 * moto_enum_store_new_with_values_valist:
 * @enum_type: the #GType of an enum.
 * @n_values:  the number of enum values to include
 * @args:      a va_list of enum values (exactly @n_values)
 *
 * See moto_enum_store_new_with_values().
 *
 * Return value: a new #MotoEnumStore.
 *
 * Since: MOTO 2.4
 **/
GtkListStore *
moto_enum_store_new_with_values_valist(GType     enum_type,
                                       gint      n_values,
                                       va_list   args)
{
    GtkListStore *store;
    GEnumValue   *value;
    gint          i;

    g_return_val_if_fail(G_TYPE_IS_ENUM (enum_type), NULL);
    g_return_val_if_fail(n_values > 1, NULL);

    store = g_object_new(MOTO_TYPE_ENUM_STORE,
                         "enum-type", enum_type,
                         NULL);

    for(i = 0; i < n_values; i++)
    {
        value = g_enum_get_value(MOTO_ENUM_STORE (store)->enum_class,
                                 va_arg (args, gint));

        if(value)
            moto_enum_store_add_value(store, value);
    }

    return store;
}

/**
 * moto_enum_store_set_stock_prefix:
 * @store:        a #MotoEnumStore
 * @stock_prefix: a prefix to create icon stock ID from enum values
 *
 * Creates a stock ID for each enum value in the @store by appending
 * the value's nick to the given @stock_prefix, separated by a hyphen.
 *
 * See also: moto_enum_combo_box_set_stock_prefix().
 *
 * Since: MOTO 2.4
 **/
void
moto_enum_store_set_stock_prefix(MotoEnumStore *store,
                                 const gchar   *stock_prefix)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    gboolean      iter_valid;

    g_return_if_fail(MOTO_IS_ENUM_STORE (store));

    model = GTK_TREE_MODEL(store);

    for(iter_valid = gtk_tree_model_get_iter_first(model, &iter);
        iter_valid;
        iter_valid = gtk_tree_model_iter_next(model, &iter))
    {
        gchar *stock_id = NULL;

        if (stock_prefix)
        {
            GEnumValue *enum_value;
            gint        value;

            gtk_tree_model_get(model, &iter,
                               MOTO_INT_STORE_VALUE, &value,
                               -1);

            enum_value = g_enum_get_value(store->enum_class, value);

            stock_id = g_strconcat(stock_prefix, "-",
                                   enum_value->value_nick,
                                   NULL);
        }

        gtk_list_store_set(GTK_LIST_STORE (store), &iter,
                           MOTO_INT_STORE_STOCK_ID, stock_id,
                           -1);

        if(stock_id)
            g_free(stock_id);
    }
}

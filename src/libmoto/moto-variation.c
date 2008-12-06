#include "moto-variation.h"

/* class MotoVariation */

typedef struct _MotoVariationPriv MotoVariationPriv;

struct _MotoVariationPriv
{
    GString *name;
    MotoVariation *parent;

    GHashTable *params;
};

#define MOTO_VARIATION_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_VARIATION, MotoVariationPriv)

static GObjectClass *variation_parent_class = NULL;

static void
moto_variation_finalize(GObject *obj)
{
    MotoVariationPriv *priv = MOTO_VARIATION_GET_PRIVATE(obj);

    g_string_free(priv->name, TRUE);
    g_hash_table_unref(priv->params);

    variation_parent_class->finalize(obj);
}

static void destroy_key(gpointer data)
{
    g_slice_free(guint, data);
}

static void destroy_value(gpointer data)
{
    g_slice_free(GValue, data);
}

static void
moto_variation_init(MotoVariation *self)
{
    MotoVariationPriv *priv = MOTO_VARIATION_GET_PRIVATE(self);

    priv->name = g_string_new("");
    priv->parent = NULL;

    priv->params = \
        g_hash_table_new_full(g_int_hash, g_int_equal,
                              destroy_key, destroy_value);
}

static void
moto_variation_class_init(MotoVariationClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    variation_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->finalize   = moto_variation_finalize;

    g_type_class_add_private(klass, sizeof(MotoVariationPriv));
}

G_DEFINE_TYPE(MotoVariation, moto_variation, G_TYPE_OBJECT);

/* methods of class MotoVariation */

MotoVariation *moto_variation_new(const gchar *name)
{
    MotoVariation *self = (MotoVariation *)g_object_new(MOTO_TYPE_VARIATION, NULL);
    MotoVariationPriv *priv = MOTO_VARIATION_GET_PRIVATE(self);

    g_string_assign(priv->name, name);

    return self;
}

MotoVariation *moto_variation_get_parent(MotoVariation *self)
{
    MotoVariationPriv *priv = MOTO_VARIATION_GET_PRIVATE(self);
    return priv->parent;
}

void moto_variation_set_parent(MotoVariation *self, MotoVariation *parent)
{
    MotoVariationPriv *priv = MOTO_VARIATION_GET_PRIVATE(self);
    priv->parent = parent;
}

static void null_value(GValue *value, GObject *where_the_object_was)
{
    g_value_set_object(value, NULL);
}

void moto_variation_save_param(MotoVariation *self, MotoParam *p)
{
    MotoVariationPriv *priv = MOTO_VARIATION_GET_PRIVATE(self);

    GValue none = {0,};

    guint id = moto_param_get_id(p);
    GValue *pv = moto_param_get_value(p);
    GType pt = G_VALUE_TYPE(pv);
    GHashTable *params = priv->params;

    GValue *v = (GValue *)g_hash_table_lookup(params, &id);
    if( ! v)
    {
        v = g_slice_new(GValue);
        *v = none;
        g_value_init(v, pt);

        guint *key = g_slice_new(guint);
        *key = id;
        g_hash_table_insert(params, key, v);
    }
    else if(g_type_is_a(pt, G_TYPE_OBJECT))
    {
        GObject *o = g_value_get_object(v);
        if(o)
            g_object_weak_unref(o, (GWeakNotify)null_value, v);
    }

    if(g_type_is_a(pt, G_TYPE_OBJECT))
    {
        GObject *o = moto_param_get_object(p);
        if(o)
            g_object_weak_ref(o, (GWeakNotify)null_value, v);
    }
    g_value_copy(pv, v);
}

void moto_variation_restore_param(MotoVariation *self, MotoParam *p)
{
    MotoVariationPriv *priv = MOTO_VARIATION_GET_PRIVATE(self);

    guint id = moto_param_get_id(p);
    GValue *pv = moto_param_get_value(p);
    GHashTable *params = priv->params;

    GValue *v = (GValue *)g_hash_table_lookup(params, &id);
    if( ! v)
    {
        if(priv->parent)
            moto_variation_restore_param(priv->parent, p);
        return;
    }

    g_value_copy(v, pv);
}


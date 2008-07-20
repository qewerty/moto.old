#include "common/moto-mapped-list.h"

#include "moto-node.h"
#include "moto-world.h"
#include "moto-messager.h"

/* utils */

typedef struct _Domain
{
    GSList *params;
} Domain;

static Domain *domain_new()
{
    Domain *self = g_slice_new(Domain);
    self->params = NULL;
    return self;
}

static void domain_free(Domain *self)
{
    g_slist_free(self->params);
    g_slice_free(Domain, self);
}

static void domain_add_param(Domain *self, MotoParam *param)
{
    self->params = g_slist_append(self->params, param);
}

static void domain_foreach_param(Domain *self, GFunc func, gpointer user_data)
{
    g_slist_foreach(self->params, func, user_data);
}

static void
unref_gobject(gpointer data, gpointer user_data)
{
    g_object_unref(data);
}

static void
free_domain(Domain *domain, gpointer user_data)
{
    domain_free(domain);
}

/* forwards */

static GType moto_node_factory_get_node_type_virtual(MotoNodeFactory *self);

/* privates */

struct _MotoNodePriv
{
    gboolean disposed;

    guint id;

    GString *name;
    MotoWorld *world;

    MotoMappedList params;
    MotoMappedList pdomains;

    gboolean hidden;

    GSList *tags;
    GSList *notes;

    /* For exporting optimization. */
    GTimeVal last_modified;
};

struct _MotoNodeFactoryPriv
{
    gboolean disposed;

    GType node_type;
};

struct _MotoParamPriv
{
    gboolean disposed;

    guint id;

    GValue value;
    GParamSpec *pspec;
    MotoParam *source;
    GSList *dests;

    GString *name;
    GString *title;
    MotoNode *node;
    MotoParamMode mode;

    gboolean hidden;
    GTimeVal last_modified; /* For exporting optimization. */

    /* GList *notes; // ??? */
};

/* class MotoNode */

static GObjectClass *node_parent_class = NULL;

static void
moto_node_dispose(GObject *obj)
{
    MotoNode *self = (MotoNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_string_free(self->priv->name, TRUE);
    moto_mapped_list_free_all(& self->priv->params, unref_gobject);
    moto_mapped_list_free_all(& self->priv->pdomains, (GFunc)free_domain);

    node_parent_class->dispose(obj);
}

static void
moto_node_finalize(GObject *obj)
{
    MotoNode *self = (MotoNode *)obj;

    g_slice_free(MotoNodePriv, self->priv);

    node_parent_class->finalize(obj);
}

static void
moto_node_init(MotoNode *self)
{
    self->priv = g_slice_new(MotoNodePriv);
    self->priv->disposed = FALSE;

    static guint id = 0;
    self->priv->id = ++id;

    self->priv->name = g_string_new("");
    self->priv->world = NULL;

    moto_mapped_list_init(& self->priv->params);
    moto_mapped_list_init(& self->priv->pdomains);

    self->priv->hidden = FALSE;
    g_get_current_time(& self->priv->last_modified);

    self->priv->tags = NULL;
}

static void
moto_node_class_init(MotoNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_node_dispose;
    goclass->finalize   = moto_node_finalize;

    klass->update = NULL;
}

G_DEFINE_ABSTRACT_TYPE(MotoNode, moto_node, G_TYPE_OBJECT);

/* methods of class MotoNode */

const gchar *moto_node_get_name(MotoNode *self)
{
    return self->priv->name->str;
}

void moto_node_set_name(MotoNode *self, const gchar *name)
{
    g_string_assign(self->priv->name, name);
}

guint moto_node_get_id(MotoNode *self)
{
    return self->priv->id;
}

void moto_node_add_param(MotoNode *self, MotoParam *param,
        const gchar *domain, const gchar *group)
{
    Domain *d = (Domain *)moto_mapped_list_get(& self->priv->pdomains, domain);
    if( ! d)
    {
        d = domain_new();
        moto_mapped_list_set(& self->priv->pdomains, domain, d);
    }
    domain_add_param(d, param);
    moto_mapped_list_set(& self->priv->params, moto_param_get_name(param), param);
}

void moto_node_add_params(MotoNode *self, ...)
{
    va_list ap;
    va_start(ap, self);

    GValue none = {0,};

    while(1)
    {
        gchar *pname    = va_arg(ap, gchar*);
        if( ! pname)
            return;
        gchar *ptitle   = va_arg(ap, gchar*);
        GType ptype     = va_arg(ap, GType);
        MotoParamMode pmode = va_arg(ap, MotoParamMode);

        GValue v = none;
        g_value_init(&v, ptype);

        switch(ptype)
        {
            case G_TYPE_BOOLEAN:
                g_value_set_boolean(&v, va_arg(ap, gboolean));
            break;
            case G_TYPE_INT:
                g_value_set_int(&v, va_arg(ap, gint));
            break;
            case G_TYPE_UINT:
                g_value_set_uint(&v, va_arg(ap, guint));
            break;
            case G_TYPE_LONG:
                g_value_set_long(&v, va_arg(ap, glong));
            break;
            case G_TYPE_ULONG:
                g_value_set_ulong(&v, va_arg(ap, gulong));
            break;
            case G_TYPE_INT64:
                g_value_set_int64(&v, va_arg(ap, gint64));
            break;
            case G_TYPE_UINT64:
                g_value_set_uint64(&v, va_arg(ap, guint64));
            break;
            case G_TYPE_FLOAT:
                g_value_set_float(&v, (gfloat)va_arg(ap, gdouble));
            break;
            case G_TYPE_DOUBLE:
                g_value_set_double(&v, va_arg(ap, gdouble));
            break;
            case G_TYPE_POINTER:
                g_value_set_pointer(&v, va_arg(ap, gpointer));
            break;
            default:
                if(g_type_is_a(ptype, G_TYPE_ENUM))
                {
                    g_value_set_enum(&v, va_arg(ap, gint));
                }
                else
                    g_value_set_object(&v, va_arg(ap, gpointer));
        }

        GParamSpec *pspec = va_arg(ap, GParamSpec*);
        gchar *domain   = va_arg(ap, gchar*);
        gchar *group    = va_arg(ap, gchar*);

        MotoParam *p = moto_param_new(pname, ptitle, pmode, &v, pspec, self);

        moto_node_add_param(self, p, domain, group);
    }
}

MotoParam *moto_node_get_param(MotoNode *self, const gchar *name)
{
    return moto_mapped_list_get(& self->priv->params, name);
}

GValue *moto_node_get_param_value(MotoNode *self, const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
        return NULL;
    return moto_param_get_value(p);
}

void moto_node_set_param_boolean(MotoNode *self, const gchar *name, gboolean value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_boolean(p, value);
}

void moto_node_set_param_int(MotoNode *self, const gchar *name, gint value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_int(p, value);
}

void moto_node_set_param_uint(MotoNode *self, const gchar *name, guint value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_uint(p, value);
}

void moto_node_set_param_long(MotoNode *self, const gchar *name, glong value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_long(p, value);
}

void moto_node_set_param_ulong(MotoNode *self, const gchar *name, gulong value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_ulong(p, value);
}

void moto_node_set_param_int64(MotoNode *self, const gchar *name, gint64 value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_int64(p, value);
}

void moto_node_set_param_uint64(MotoNode *self, const gchar *name, guint64 value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_uint64(p, value);
}

void moto_node_set_param_float(MotoNode *self, const gchar *name, gfloat value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_float(p, value);
}

void moto_node_set_param_double(MotoNode *self, const gchar *name, gdouble value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_double(p, value);
}

void moto_node_set_param_pointer(MotoNode *self, const gchar *name, gpointer value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_pointer(p, value);
}

void moto_node_set_param_enum(MotoNode *self, const gchar *name, gint value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_enum(p, value);
}

void moto_node_set_param_object(MotoNode *self, const gchar *name, GObject *value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        return;
    }
    moto_param_set_object(p, value);
}

void moto_node_set_params(MotoNode *self, ...)
{
    va_list ap;
    va_start(ap, self);

    MotoParam *p;
    while(1)
    {
        gchar *pname = va_arg(ap, gchar*);
        p = moto_node_get_param(self, pname);
        if( ! p)
        {
            // TODO: print error
            return;
        }
        GValue *v = moto_param_get_value(p);
        GType ptype = G_VALUE_TYPE(v);

        switch(ptype)
        {
            case G_TYPE_BOOLEAN:
                g_value_set_boolean(v, va_arg(ap, gboolean));
            break;
            case G_TYPE_INT:
                g_value_set_int(v, va_arg(ap, gint));
            break;
            case G_TYPE_UINT:
                g_value_set_uint(v, va_arg(ap, guint));
            break;
            case G_TYPE_LONG:
                g_value_set_long(v, va_arg(ap, glong));
            break;
            case G_TYPE_ULONG:
                g_value_set_ulong(v, va_arg(ap, gulong));
            break;
            case G_TYPE_INT64:
                g_value_set_int64(v, va_arg(ap, gint64));
            break;
            case G_TYPE_UINT64:
                g_value_set_uint64(v, va_arg(ap, guint64));
            break;
            case G_TYPE_FLOAT:
                g_value_set_float(v, (gfloat)va_arg(ap, gdouble));
            break;
            case G_TYPE_DOUBLE:
                g_value_set_double(v, va_arg(ap, gdouble));
            break;
            case G_TYPE_POINTER:
                g_value_set_pointer(v, va_arg(ap, gpointer));
            break;
            default:
                if(g_type_is_a(ptype, G_TYPE_ENUM))
                {
                    g_value_set_enum(v, va_arg(ap, gint));
                }
                else
                    g_value_set_object(v, va_arg(ap, gpointer));
        }
    }
}

void moto_node_set_source(MotoNode *self, const gchar *in_name,
                          MotoNode *other, const gchar *out_name)
{
    MotoParam *in  = moto_node_get_param(self, in_name);
    MotoParam *out = moto_node_get_param(other, out_name);
    moto_param_set_source(in, out);
}


static save_param(MotoParam *p, MotoVariation *v)
{
    moto_variation_save_param(v, p);
}

static restore_param(MotoParam *p, MotoVariation *v)
{
    moto_variation_restore_param(v, p);
}

void moto_node_save_to_variation(MotoNode *self, MotoVariation *variation)
{
    moto_mapped_list_foreach(& self->priv->params, (GFunc)save_param, variation);
}

void moto_node_restore_from_variation(MotoNode *self, MotoVariation *variation)
{
    moto_mapped_list_foreach(& self->priv->params, (GFunc)restore_param, variation);
}

gboolean moto_node_is_hidden(MotoNode *self)
{
    return self->priv->hidden;
}

void moto_node_set_hidden(MotoNode *self, gboolean hidden)
{
    self->priv->hidden = hidden;
}

void moto_node_hide(MotoNode *self)
{
    moto_node_set_hidden(self, FALSE);
}

void moto_node_show(MotoNode *self)
{
    moto_node_set_hidden(self, TRUE);
}

void moto_node_update(MotoNode *self)
{
    MotoNodeClass *klass = MOTO_NODE_GET_CLASS(self);

    if(klass->update)
        klass->update(self);

    moto_node_update_last_modified(self);
}

const GTimeVal *moto_node_get_last_modified(MotoNode *self)
{
    return & self->priv->last_modified;
}

void moto_node_update_last_modified(MotoNode *self)
{
    g_get_current_time(& self->priv->last_modified);
}

void moto_node_set_tag(MotoNode *self, const gchar *tag)
{
    GSList *curtag = self->priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate(((GString *)curtag->data)->str, tag) == 0)
            return;
    }

    GString *str = g_string_new(tag);
    self->priv->tags = g_slist_append(self->priv->tags, str);
}

void moto_node_del_tag(MotoNode *self, const gchar *tag)
{
    GSList *curtag = self->priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate((gchar *)(curtag->data), tag) == 0)
        {
            self->priv->tags = g_slist_delete_link(self->priv->tags, curtag);
            g_string_free((GString *)curtag->data, TRUE);
        }
    }
}

gboolean moto_node_has_tag(MotoNode *self, const gchar *tag)
{
    GSList *curtag = self->priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate(((GString *)curtag->data)->str, tag) == 0)
            return TRUE;
    }
    return FALSE;
}

MotoWorld *moto_node_get_world(MotoNode *self)
{
    return self->priv->world;
}

void moto_node_set_world(MotoNode *self, MotoWorld *world)
{
    self->priv->world = world;
}

MotoLibrary *moto_node_get_library(MotoNode *self)
{
    MotoWorld *w = moto_node_get_world(self);
    if( ! w)
        return NULL;
    return moto_world_get_library(w);
}

/* class MotoNodeFactory */

static GObjectClass *node_factory_parent_class = NULL;

static void
moto_node_factory_dispose(GObject *obj)
{
    /* MotoNodeFactory *self = (MotoNodeFactory *)obj; */
    node_factory_parent_class->dispose(obj);
}

static void
moto_node_factory_finalize(GObject *obj)
{
    node_factory_parent_class->finalize(obj);
}

static void
moto_node_factory_init(MotoNodeFactory *self)
{
    self->priv = (MotoNodeFactoryPriv *)g_slice_new(MotoNodeFactoryPriv);

    self->priv->node_type = G_TYPE_INVALID;
}

static void
moto_node_factory_class_init(MotoNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    node_factory_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_node_factory_dispose;
    goclass->finalize   = moto_node_factory_finalize;

    klass->get_node_type    = moto_node_factory_get_node_type_virtual;
    klass->create_node      = NULL;

    klass->create_node_signal_id = g_signal_newv ("create-node",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->node_created_signal_id = g_signal_newv ("node-created",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

}

G_DEFINE_ABSTRACT_TYPE(MotoNodeFactory, moto_node_factory, G_TYPE_OBJECT);

/* methods of class MotoNodeFactory */

MotoNode *
moto_node_factory_create_node(MotoNodeFactory *self, const gchar *name)
{
    MotoNodeFactoryClass *klass = MOTO_NODE_FACTORY_GET_CLASS(self);

    g_signal_emit(self, klass->create_node_signal_id, 0, NULL);

    if(G_TYPE_IS_ABSTRACT(self->priv->node_type))
    {
        GString *msg = g_string_new("You are trying to create instance of abstract node type (\"");
        g_string_append(msg, g_type_name(self->priv->node_type));
        g_string_append(msg, "\"). I won't create it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return NULL;
    }

    MotoNode *node = NULL;

    if(klass->create_node)
        node = klass->create_node(self, name);

    g_signal_emit(self, klass->node_created_signal_id, 0, NULL);

    return node;
}

GType moto_node_factory_get_node_type(MotoNodeFactory *self)
{
    MotoNodeFactoryClass *klass = MOTO_NODE_FACTORY_GET_CLASS(self);

    if(klass->get_node_type)
        return klass->get_node_type(self);
    return G_TYPE_INVALID;
}

static GType moto_node_factory_get_node_type_virtual(MotoNodeFactory *self)
{
    return MOTO_TYPE_NODE;
}

/* class MotoParam */

static GObjectClass *param_parent_class = NULL;

static void
moto_param_dispose(GObject *obj)
{
    MotoParam *self = (MotoParam *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_string_free(self->priv->name, TRUE);
    g_string_free(self->priv->title, TRUE);

    param_parent_class->dispose(obj);
}

static void
moto_param_finalize(GObject *obj)
{
    MotoParam *self = (MotoParam *)obj;

    g_slice_free(MotoParamPriv, self->priv);

    G_OBJECT_CLASS(param_parent_class)->finalize(obj);
}

static void
moto_param_init(MotoParam *self)
{
    self->priv = (MotoParamPriv *)g_slice_new(MotoParamPriv);
    self->priv->disposed = FALSE;

    static guint id = 0;
    self->priv->id = ++id;

    self->priv->source = NULL;
    self->priv->dests = NULL;

    self->priv->name = g_string_new("");
    self->priv->title = g_string_new("");

    self->priv->mode = MOTO_PARAM_MODE_IN;

    self->priv->hidden = FALSE;
    g_get_current_time(& self->priv->last_modified);
}

static void
moto_param_class_init(MotoParamClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    param_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_param_dispose;
    goclass->finalize   = moto_param_finalize;
}

G_DEFINE_TYPE(MotoParam, moto_param, G_TYPE_OBJECT);

/* methods of class MotoParam */

MotoParam *moto_param_new(const gchar *name, const gchar *title,
        MotoParamMode mode, GValue *value, GParamSpec *pspec,
        MotoNode *node)
{
    if( ! node)
        return NULL;
    if( ! value)
        return NULL;
    /*
    if( ! pspec)
        return NULL;
    */

    GValue none = {0, };

    MotoParam *self = (MotoParam *)g_object_new(MOTO_TYPE_PARAM, NULL);

    g_string_assign(self->priv->name, name);
    g_string_assign(self->priv->title, title);
    self->priv->mode = mode;
    self->priv->node = node;

    self->priv->value = none;
    g_value_init(& self->priv->value, G_VALUE_TYPE(value));
    g_value_copy(value, & self->priv->value);

    return self;
}

void moto_param_set_from_string(MotoParam *self,
        const gchar *string)
{

}

const gchar *moto_param_get_name(MotoParam *self)
{
    return self->priv->name->str;
}

const gchar *moto_param_get_title(MotoParam *self)
{
    return self->priv->title->str;
}

MotoParamMode moto_param_get_mode(MotoParam *self)
{
    return self->priv->mode;
}

guint moto_param_get_id(MotoParam *self)
{
    return self->priv->id;
}

GValue * moto_param_get_value(MotoParam *self)
{
    return & self->priv->value;
}

gpointer moto_param_get_value_pointer(MotoParam *self)
{
    return self->priv->value.data;
}

void moto_param_set_boolean(MotoParam *self, gboolean value)
{
    g_value_set_boolean(& self->priv->value, value);
}

void moto_param_set_int(MotoParam *self, gint value)
{
    g_value_set_int(& self->priv->value, value);
}

void moto_param_set_uint(MotoParam *self, guint value)
{
    g_value_set_uint(& self->priv->value, value);
}

void moto_param_set_long(MotoParam *self, glong value)
{
    g_value_set_long(& self->priv->value, value);
}

void moto_param_set_ulong(MotoParam *self, gulong value)
{
    g_value_set_ulong(& self->priv->value, value);
}

void moto_param_set_int64(MotoParam *self, gint64 value)
{
    g_value_set_int64(& self->priv->value, value);
}

void moto_param_set_uint64(MotoParam *self, guint64 value)
{
    g_value_set_uint64(& self->priv->value, value);
}

void moto_param_set_float(MotoParam *self, gfloat value)
{
    g_value_set_float(& self->priv->value, value);
}

void moto_param_set_double(MotoParam *self, gdouble value)
{
    g_value_set_double(& self->priv->value, value);
}

void moto_param_set_pointer(MotoParam *self, gpointer value)
{
    g_value_set_pointer(& self->priv->value, value);
}

void moto_param_set_enum(MotoParam *self, gint value)
{
    g_value_set_enum(& self->priv->value, value);
}

void moto_param_set_object(MotoParam *self, GObject *value)
{
    g_value_set_object(& self->priv->value, value);
}

MotoParam *moto_param_get_source(MotoParam *self)
{
    return self->priv->source;
}

static void
disconnect_on_source_deleted(gpointer data, GObject *where_the_object_was)
{
    MotoParam *param = (MotoParam *)data;

    param->priv->source = NULL;
}

static void
exclude_from_dests_on_dest_deleted(gpointer data, GObject *where_the_object_was)
{
    MotoParam *param = (MotoParam *)data;

    param->priv->dests = g_slist_remove(param->priv->dests, where_the_object_was);
}

void moto_param_set_source(MotoParam *self, MotoParam *src)
{
    if( ! src)
    {
        GString *msg = g_string_new("You are trying to connect nothing (None|NULL). I won't connect it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    if( ! (src->priv->mode & MOTO_PARAM_MODE_OUT))
    {
        GString *msg = g_string_new("You are trying to connect source that has no output (\"");
        g_string_append(msg, moto_param_get_name(src));
        g_string_append(msg, "\"). I won't connect it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    if( ! (self->priv->mode & MOTO_PARAM_MODE_IN))
    {
        GString *msg = g_string_new("You are trying to connect source to output parameter (\"");
        g_string_append(msg, moto_param_get_name(self));
        g_string_append(msg, "\"). I won't connect it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    /* TODO: Type checking! */

    if(src == self->priv->source)
        return;

    g_object_weak_ref(G_OBJECT(src), disconnect_on_source_deleted, self);
    g_object_weak_ref(G_OBJECT(self), exclude_from_dests_on_dest_deleted, src);

    self->priv->source = src;
    src->priv->dests = g_slist_append(src->priv->dests, self);

    moto_param_update(self);
}

void moto_param_clear_source(MotoParam *self)
{

    if(self->priv->source)
    {
        g_object_weak_unref(G_OBJECT(self->priv->source), disconnect_on_source_deleted, self);
        g_object_weak_unref(G_OBJECT(self), exclude_from_dests_on_dest_deleted, self->priv->source);

        self->priv->source->priv->dests = g_slist_remove(self->priv->source->priv->dests, self);
    }
    self->priv->source = NULL;
}

static void
null_source(gpointer data, gpointer user_data)
{
    ((MotoParam *)data)->priv->source = NULL;
}

void moto_param_clear_dests(MotoParam *self)
{
    if(self->priv->mode == MOTO_PARAM_MODE_IN)
    {
        GString *msg = g_string_new("You are trying to clear destinations of input parameter (\"");
        g_string_append(msg, moto_param_get_name(self));
        g_string_append(msg, "\"). Inputs may not have destinations.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    g_slist_foreach(self->priv->dests, null_source, NULL);
    g_slist_free(self->priv->dests);
    self->priv->dests = NULL;
}

gboolean moto_param_has_dests(MotoParam *self)
{
    return (g_slist_length(self->priv->dests) > 0);
}

gboolean moto_param_is_valid(MotoParam *self)
{
    return MOTO_IS_NODE(moto_param_get_node(self));
}

MotoNode *moto_param_get_node(MotoParam *self)
{
    return self->priv->node;
}

void moto_param_update(MotoParam *self)
{
    if( ! self->priv->source)
        return;

    g_value_transform(& self->priv->source->priv->value, & self->priv->value);

    moto_node_update(moto_param_get_node(self));
}

void moto_param_update_dests(MotoParam *self)
{
    if( ! (self->priv->mode & MOTO_PARAM_MODE_OUT))
        return;

    GSList *dest = self->priv->dests;
    for(; dest; dest = g_slist_next(dest))
    {
        MotoParam *param = (MotoParam *)dest->data;
        moto_param_update(param);
    }

}

/* class MotoVariation */

struct _MotoVariationPriv
{
    gboolean disposed;

    GString *name;
    MotoVariation *parent;

    GHashTable *params;

};

static GObjectClass *variation_parent_class = NULL;

static void
moto_variation_dispose(GObject *obj)
{
    MotoVariation *self = (MotoVariation *)obj;
    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_string_free(self->priv->name, TRUE);
    g_hash_table_unref(self->priv->params);
}

static void
moto_variation_finalize(GObject *obj)
{
    MotoVariation *self = (MotoVariation *)obj;

    g_slice_free(MotoVariationPriv, self->priv);

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
    self->priv = g_slice_new(MotoVariationPriv);

    self->priv->name = g_string_new("");
    self->priv->parent = NULL;

    self->priv->params = \
        g_hash_table_new_full(g_int_hash, g_int_equal,
                              destroy_key, destroy_value);
}

static void
moto_variation_class_init(MotoVariationClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    variation_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_variation_dispose;
    goclass->finalize   = moto_variation_finalize;
}

G_DEFINE_TYPE(MotoVariation, moto_variation, G_TYPE_OBJECT);

/* methods of class MotoVariation */

MotoVariation *moto_variation_new(const gchar *name)
{
    MotoVariation *self = (MotoVariation *)g_object_new(MOTO_TYPE_VARIATION, NULL);

    g_string_assign(self->priv->name, name);

    return self;
}

MotoVariation *moto_variation_get_parent(MotoVariation *self)
{
    return self->priv->parent;
}

void moto_variation_set_parent(MotoVariation *self, MotoVariation *parent)
{
    self->priv->parent = parent;
}

void moto_variation_save_param(MotoVariation *self, MotoParam *p)
{
    GValue none = {0,};

    guint id = moto_param_get_id(p);
    GValue *pv = moto_param_get_value(p);
    GHashTable *params = self->priv->params;

    GValue *v = (GValue *)g_hash_table_lookup(params, &id);
    if( ! v)
    {
        v = g_slice_new(GValue);
        *v = none;
        g_value_init(v, G_VALUE_TYPE(pv));

        guint *key = g_slice_new(guint);
        *key = id;
        g_hash_table_insert(params, key, v);
    }
    g_value_copy(pv, v);
}

void moto_variation_restore_param(MotoVariation *self, MotoParam *p)
{
    guint id = moto_param_get_id(p);
    GValue *pv = moto_param_get_value(p);
    GHashTable *params = self->priv->params;

    GValue *v = (GValue *)g_hash_table_lookup(params, &id);
    if( ! v)
    {
        if(self->priv->parent)
            moto_variation_restore_param(self->priv->parent, p);
        return;
    }

    g_value_copy(v, pv);
}


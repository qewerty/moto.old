#include "common/moto-mapped-list.h"

#include "moto-filename.h"
#include "moto-node.h"
#include "moto-world.h"
#include "moto-messager.h"

/* enums */

GType moto_param_mode_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_PARAM_MODE_IN ,   "PARAM_MODE_IN",    "IN"},
            {MOTO_PARAM_MODE_OUT,   "PARAM_MODE_OUT",   "OUT"},
            {MOTO_PARAM_MODE_INOUT, "PARAM_MODE_INPUT", "INOUT"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoParamMode", values);
    }
    return type;
}

/* utils */

typedef struct _Domain
{
    GArray *types; // Param types allowed in the domain. If NULL any type allowed.
    GSList *params;
} Domain;

static Domain *domain_new()
{
    Domain *self = g_slice_new(Domain);
    self->types = NULL;
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

typedef struct _Group
{
    GString *name;
    GArray *types; // Param types allowed in the group. If NULL any type allowed.
    GSList *params;
} Group;

static Group *group_new(const gchar *name)
{
    Group *self = g_slice_new(Group);
    self->name = g_string_new(name);
    self->types = NULL;
    self->params = NULL;
    return self;
}

static void group_free(Group *self)
{
    g_string_free(self->name, TRUE);
    g_slist_free(self->params);
    g_slice_free(Group, self);
}

static void group_add_param(Group *self, MotoParam *param)
{
    self->params = g_slist_append(self->params, param);
}

static void group_foreach_param(Group *self, GFunc func, gpointer user_data)
{
    g_slist_foreach(self->params, func, user_data);
}

static void
free_group(Group *group, gpointer user_data)
{
    group_free(group);
}


/* privates */

typedef struct _MotoNodePriv MotoNodePriv;
typedef struct _MotoParamPriv MotoParamPriv;

struct _MotoNodePriv
{
    gboolean disposed;

    guint id;

    GString *name;
    MotoWorld *world;

    MotoMappedList params;
    MotoMappedList pgroups;
    MotoMappedList pdomains;

    gboolean hidden;

    GSList *tags;
    GSList *notes;

    /* For exporting optimization. */
    GTimeVal last_modified;
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

#define MOTO_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_NODE, MotoNodePriv)
#define MOTO_PARAM_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_PARAM, MotoParamPriv)

static GObjectClass *node_parent_class = NULL;

static void
moto_node_dispose(GObject *obj)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(obj);
    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    g_string_free(priv->name, TRUE);
    moto_mapped_list_free_all(& priv->params, unref_gobject);
    moto_mapped_list_free_all(& priv->pgroups, (GFunc)free_group);
    moto_mapped_list_free_all(& priv->pdomains, (GFunc)free_domain);

    node_parent_class->dispose(obj);
}

static void
moto_node_finalize(GObject *obj)
{
    node_parent_class->finalize(obj);
}

static void
moto_node_init(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    priv->disposed = FALSE;

    static guint id = 0;
    priv->id = ++id;

    priv->name = g_string_new("");
    priv->world = NULL;

    moto_mapped_list_init(& priv->params);
    moto_mapped_list_init(& priv->pgroups);
    moto_mapped_list_init(& priv->pdomains);

    priv->hidden = FALSE;
    g_get_current_time(& priv->last_modified);

    priv->tags = NULL;
}

static void
moto_node_class_init(MotoNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_node_dispose;
    goclass->finalize   = moto_node_finalize;

    g_datalist_init(& klass->actions);

    klass->update = NULL;

    g_type_class_add_private(goclass, sizeof(MotoNodePriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoNode, moto_node, G_TYPE_OBJECT);

/* methods of class MotoNode */

MotoNode *moto_create_node(GType type, const gchar *name)
{
    if( ! g_type_is_a(type, MOTO_TYPE_NODE))
    {
        // TODO: Error
        return NULL;
    }

    if(G_TYPE_IS_ABSTRACT(type))
    {
        // TODO: Error
        return NULL;
    }

    MotoNode *node = g_object_new(type, NULL);
    moto_node_set_name(node, name);
    moto_node_update(node);

    return node;
}

MotoNode *moto_create_node_by_name(const gchar *type_name, const gchar *name)
{
    GType type = g_type_from_name(type_name);
    if( ! type)
    {
        // TODO: Error
        return NULL;
    }

    return moto_create_node(type, name);
}

void moto_node_do_action(MotoNode *self, const gchar *action_name)
{
    MotoNodeActionFunc func = \
        (MotoNodeActionFunc)g_datalist_get_data(& MOTO_NODE_GET_CLASS(self)->actions, action_name);
    if( ! func)
    {
        // FIXME: Warning?
        return;
    }

    func(self);
}

void moto_node_set_action(MotoNode *self, const gchar *action_name, MotoNodeActionFunc func)
{
    g_datalist_set_data(& MOTO_NODE_GET_CLASS(self)->actions, action_name, func);
}

const gchar *moto_node_get_name(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return priv->name->str;
}

void moto_node_set_name(MotoNode *self, const gchar *name)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    g_string_assign(priv->name, name);
}

guint moto_node_get_id(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return priv->id;
}

void moto_node_add_dynamic_param(MotoNode *self, MotoParam *param,
        const gchar *domain, const gchar *group)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    Domain *d = (Domain *)moto_mapped_list_get(& priv->pdomains, domain);
    if( ! d)
    {
        d = domain_new();
        moto_mapped_list_set(& priv->pdomains, domain, d);
    }
    domain_add_param(d, param);

    Group *g = (Group *)moto_mapped_list_get(& priv->pgroups, group);
    if( ! g)
    {
        g = group_new(group);
        moto_mapped_list_set(& priv->pgroups, group, g);
    }
    group_add_param(g, param);

    moto_mapped_list_set(& priv->params, moto_param_get_name(param), param);
}

void moto_node_add_params(MotoNode *self, ...)
{
    va_list ap;
    va_start(ap, self);

    while(1)
    {
        gchar *pname    = va_arg(ap, gchar*);
        if( ! pname)
            return;
        gchar *ptitle   = va_arg(ap, gchar*);
        GType ptype     = va_arg(ap, GType);
        MotoParamMode pmode = va_arg(ap, MotoParamMode);

        GValue v = {0,};
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
            case G_TYPE_STRING:
                g_value_set_string(&v, va_arg(ap, gchar*));
            break;
            case G_TYPE_POINTER:
                g_value_set_pointer(&v, va_arg(ap, gpointer));
            break;
            default:
                if(g_type_is_a(ptype, G_TYPE_ENUM))
                {
                    g_value_set_enum(&v, va_arg(ap, gint));
                }
                else if(ptype == MOTO_TYPE_FILENAME)
                {
                    g_value_set_string(&v, va_arg(ap, gchar*));
                }
                else if(g_type_is_a(ptype, G_TYPE_OBJECT))
                {
                    g_value_set_object(&v, va_arg(ap, gpointer));
                }
                else
                {
                    moto_error("moto_node_add_params: type (%s) is not supported for parameters of moto nodes.", g_type_name(ptype));

                    // pop all data of param with invalid type
                    va_arg(ap, GParamSpec*);
                    va_arg(ap, gchar*);
                    va_arg(ap, gchar*);

                    continue;
                }
        }

        GParamSpec *pspec = va_arg(ap, GParamSpec*);
        gchar *domain   = va_arg(ap, gchar*);
        gchar *group    = va_arg(ap, gchar*);

        MotoParam *p = moto_param_new(pname, ptitle, pmode, &v, pspec, self);

        moto_node_add_param(self, p, domain, group);
    }

    va_end(ap);
}

MotoParam *moto_node_get_param(MotoNode *self, const gchar *name)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return moto_mapped_list_get(& priv->params, name);
}

GValue *moto_node_get_param_value(MotoNode *self, const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
        return NULL;
    return moto_param_get_value(p);
}

gboolean moto_node_get_param_boolean(MotoNode *self,    const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_boolean(p);
}

gint moto_node_get_param_int(MotoNode *self,        const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_int(p);
}

guint moto_node_get_param_uint(MotoNode *self,       const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_uint(p);
}

glong moto_node_get_param_long(MotoNode *self,       const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_long(p);
}

gulong moto_node_get_param_ulong(MotoNode *self,      const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_ulong(p);
}

gint64  moto_node_get_param_int64(MotoNode *self,      const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_int64(p);
}

guint64 moto_node_get_param_uint64(MotoNode *self,     const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_uint64(p);
}

gfloat moto_node_get_param_float(MotoNode *self,      const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_float(p);
}

gdouble moto_node_get_param_double(MotoNode *self,     const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_double(p);
}

gpointer moto_node_get_param_pointer(MotoNode *self,    const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_pointer(p);
}

gint moto_node_get_param_enum(MotoNode *self,       const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_enum(p);
}

GObject *moto_node_get_param_object(MotoNode *self,     const gchar *name)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        // TODO: print error
        // return;
    }
    return moto_param_get_object(p);
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

void moto_node_get_params(MotoNode *self, ...)
{
    va_list ap;
    va_start(ap, self);

    MotoParam *p;
    while(1)
    {
        gchar *pname = va_arg(ap, gchar*);
        if( ! pname)
            break;

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
                (*va_arg(ap, gboolean*)) = g_value_get_boolean(v);
            break;
            case G_TYPE_INT:
                (*va_arg(ap, gint*)) = g_value_get_int(v);
            break;
            case G_TYPE_UINT:
                (*va_arg(ap, guint*)) = g_value_get_uint(v);
            break;
            case G_TYPE_LONG:
                (*va_arg(ap, glong*)) = g_value_get_long(v);
            break;
            case G_TYPE_ULONG:
                (*va_arg(ap, gulong*)) = g_value_get_ulong(v);
            break;
            case G_TYPE_INT64:
                (*va_arg(ap, gint64*)) = g_value_get_int64(v);
            break;
            case G_TYPE_UINT64:
                (*va_arg(ap, guint64*)) = g_value_get_uint64(v);
            break;
            case G_TYPE_FLOAT:
                (*va_arg(ap, gfloat*)) = g_value_get_float(v);
            break;
            case G_TYPE_DOUBLE:
                (*va_arg(ap, gdouble*)) = g_value_get_double(v);
            break;
            case G_TYPE_POINTER:
                (*va_arg(ap, gpointer*)) = g_value_get_pointer(v);
            break;
            default:
                if(g_type_is_a(ptype, G_TYPE_ENUM))
                {
                    (*va_arg(ap, gint*)) = g_value_get_enum(v);
                }
                else
                    (*va_arg(ap, GObject**)) = g_value_get_object(v);
        }

    }

    va_end(ap);
}

void moto_node_set_params(MotoNode *self, ...)
{
    va_list ap;
    va_start(ap, self);

    MotoParam *p;
    while(1)
    {
        gchar *pname = va_arg(ap, gchar*);
        if( ! pname)
            break;

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

    va_end(ap);
}

typedef struct _NodeParamData
{
    MotoNodeForeachParamFunc func;
    MotoNode *node;
    gpointer user_data;
} NodeParamData;

static void call_param_func(MotoParam *param, NodeParamData *data)
{
    data->func(data->node, param, data->user_data);
}

void moto_node_foreach_param(MotoNode *self,
        MotoNodeForeachParamFunc func, gpointer user_data)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    NodeParamData data = {func, self, user_data};
    moto_mapped_list_foreach( & priv->params, (GFunc)call_param_func, & data);
}

typedef struct _NodeGroupData
{
    MotoNodeForeachGroupFunc func;
    MotoNode *node;
    gpointer user_data;
} NodeGroupData;

static void call_group_func(Group *group, NodeGroupData *data)
{
    data->func(data->node, group->name->str, data->user_data);
}

void moto_node_foreach_group(MotoNode *self,
        MotoNodeForeachGroupFunc func, gpointer user_data)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    NodeGroupData data = {func, self, user_data};
    moto_mapped_list_foreach( & priv->pgroups, (GFunc)call_group_func, & data);
}

void moto_node_foreach_param_in_group(MotoNode *self, const gchar *group,
        MotoNodeForeachParamInGroupFunc func, gpointer user_data)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    Group *g = moto_mapped_list_get(& priv->pgroups, group);
    if( ! g)
        return;

    GSList *p = g->params;
    for(; p; p = g_slist_next(p))
    {
        func(self, group, (MotoParam *)p->data, user_data);
    }
}

void moto_node_link(MotoNode *self, const gchar *in_name,
                    MotoNode *other, const gchar *out_name)
{
    g_return_if_fail(self);
    g_return_if_fail(other);

    MotoParam *in  = moto_node_get_param(self, in_name);
    if( ! in)
    {
        moto_error("Node '%s' hasn't input with name '%s'", moto_node_get_name(self), in_name);
        return;
    }

    MotoParam *out = moto_node_get_param(other, out_name);
    if( ! out)
    {
        moto_error("Node '%s' hasn't output with name '%s'", moto_node_get_name(other), out_name);
        return;
    }

    moto_param_link(in, out);
}


static void save_param(MotoParam *p, MotoVariation *v)
{
    moto_variation_save_param(v, p);
}

static void restore_param(MotoParam *p, MotoVariation *v)
{
    moto_variation_restore_param(v, p);
}

void moto_node_save_to_variation(MotoNode *self, MotoVariation *variation)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    moto_mapped_list_foreach(& priv->params, (GFunc)save_param, variation);
}

void moto_node_restore_from_variation(MotoNode *self, MotoVariation *variation)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    moto_mapped_list_foreach(& priv->params, (GFunc)restore_param, variation);
}

gboolean moto_node_is_hidden(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return priv->hidden;
}

void moto_node_set_hidden(MotoNode *self, gboolean hidden)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    priv->hidden = hidden;
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
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return & priv->last_modified;
}

void moto_node_update_last_modified(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    g_get_current_time(& priv->last_modified);
}

void moto_node_set_tag(MotoNode *self, const gchar *tag)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    GSList *curtag = priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate(((GString *)curtag->data)->str, tag) == 0)
            return;
    }

    GString *str = g_string_new(tag);
    priv->tags = g_slist_append(priv->tags, str);
}

void moto_node_del_tag(MotoNode *self, const gchar *tag)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    GSList *curtag = priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate((gchar *)(curtag->data), tag) == 0)
        {
            priv->tags = g_slist_delete_link(priv->tags, curtag);
            g_string_free((GString *)curtag->data, TRUE);
        }
    }
}

gboolean moto_node_has_tag(MotoNode *self, const gchar *tag)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    GSList *curtag = priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate(((GString *)curtag->data)->str, tag) == 0)
            return TRUE;
    }
    return FALSE;
}

MotoWorld *moto_node_get_world(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return priv->world;
}

void moto_node_set_world(MotoNode *self, MotoWorld *world)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    priv->world = world;
}

MotoLibrary *moto_node_get_library(MotoNode *self)
{
    MotoWorld *w = moto_node_get_world(self);
    if( ! w)
        return NULL;
    return moto_world_get_library(w);
}

/* class MotoParam */

static GObjectClass *param_parent_class = NULL;

static void
moto_param_dispose(GObject *obj)
{
    MotoParam *self = (MotoParam *)obj;
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    g_string_free(priv->name, TRUE);
    g_string_free(priv->title, TRUE);

    param_parent_class->dispose(obj);
}

static void
moto_param_finalize(GObject *obj)
{
    G_OBJECT_CLASS(param_parent_class)->finalize(obj);
}

static void
moto_param_init(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    priv->disposed = FALSE;

    static guint id = 0;
    priv->id = ++id;

    priv->source = NULL;
    priv->dests = NULL;

    priv->name = g_string_new("");
    priv->title = g_string_new("");

    priv->mode = MOTO_PARAM_MODE_IN;

    priv->pspec = NULL;

    priv->hidden = FALSE;
    g_get_current_time(& priv->last_modified);
}

static void
moto_param_class_init(MotoParamClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    param_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_param_dispose;
    goclass->finalize   = moto_param_finalize;

    klass->value_changed_signal_id = g_signal_newv ("value-changed",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->source_changed_signal_id = g_signal_newv ("source-changed",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->dest_added_signal_id = g_signal_newv ("dest-added",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->dest_removed_signal_id = g_signal_newv ("dest-removed",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    g_type_class_add_private(klass, sizeof(MotoParamPriv));
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
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    priv->pspec = pspec;

    g_string_assign(priv->name, name);
    g_string_assign(priv->title, title);
    priv->mode = mode;
    priv->node = node;

    priv->value = none;
    g_value_init(& priv->value, G_VALUE_TYPE(value));
    g_value_copy(value, & priv->value);

    return self;
}

void moto_param_set_from_string(MotoParam *self,
        const gchar *string)
{

}

const gchar *moto_param_get_name(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->name->str;
}

const gchar *moto_param_get_title(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->title->str;
}

MotoParamMode moto_param_get_mode(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->mode;
}

GParamSpec *moto_param_get_spec(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->pspec;
}

guint moto_param_get_id(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->id;
}

GValue * moto_param_get_value(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return & priv->value;
}

gpointer moto_param_get_value_pointer(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->value.data;
}

GType moto_param_get_value_type(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return G_VALUE_TYPE( & priv->value);
}

gboolean moto_param_get_boolean(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_boolean(& priv->value);
}

gint moto_param_get_int(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_int(& priv->value);
}

guint moto_param_get_uint(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_uint(& priv->value);
}

glong moto_param_get_long(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_long(& priv->value);
}

gulong moto_param_get_ulong(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_ulong(& priv->value);
}

gint64 moto_param_get_int64(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_int64(& priv->value);
}

guint64 moto_param_get_uint64(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_uint64(& priv->value);
}

gfloat moto_param_get_float(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_float(& priv->value);
}

gdouble moto_param_get_double(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_double(& priv->value);
}

const gchar *moto_param_get_string(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_string(& priv->value);
}

gpointer moto_param_get_pointer(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_pointer(& priv->value);
}

gint moto_param_get_enum(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
     return g_value_get_enum(& priv->value);
}

GObject *moto_param_get_object(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_object(& priv->value);
}

void moto_param_set_boolean(MotoParam *self, gboolean value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_boolean(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_int(MotoParam *self, gint value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_int(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_uint(MotoParam *self, guint value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_uint(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_long(MotoParam *self, glong value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_long(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_ulong(MotoParam *self, gulong value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_ulong(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_int64(MotoParam *self, gint64 value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_int64(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_uint64(MotoParam *self, guint64 value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_uint64(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_float(MotoParam *self, gfloat value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_float(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_double(MotoParam *self, gdouble value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_double(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_string(MotoParam *self, const gchar *value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_string(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_pointer(MotoParam *self, gpointer value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_pointer(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_enum(MotoParam *self, gint value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_enum(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

void moto_param_set_object(MotoParam *self, GObject *value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_object(& priv->value, value);
    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->value_changed_signal_id, 0);
}

MotoParam *moto_param_get_source(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->source;
}

static void
disconnect_on_source_deleted(MotoParam *param, GObject *where_the_object_was)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(param);
    priv->source = NULL;
    g_signal_emit(param, MOTO_PARAM_GET_CLASS(param)->source_changed_signal_id, 0);
}

static void
exclude_from_dests_on_dest_deleted(gpointer data, GObject *where_the_object_was)
{
    MotoParam *param = (MotoParam *)data;
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(param);

    priv->dests = g_slist_remove(priv->dests, where_the_object_was);
    g_signal_emit(param, MOTO_PARAM_GET_CLASS(param)->dest_removed_signal_id, 0);
}

void moto_param_link(MotoParam *self, MotoParam *src)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    if(src == priv->source)
        return;

    if( ! src)
    {
        moto_param_unlink_source(self);
        return;
    }

    MotoParamPriv *src_priv = MOTO_PARAM_GET_PRIVATE(src);

    if( ! (src_priv->mode & MOTO_PARAM_MODE_OUT))
    {
        moto_error("You are trying to connect source that has no output '%s'. I won't connect it", moto_param_get_name(src));
        return;
    }

    if( ! (priv->mode & MOTO_PARAM_MODE_IN))
    {
        moto_error("You are trying to connect source to output parameter '%s'. I won't connect it", moto_param_get_name(self));
        return;
    }

    /* TODO: Type checking! */

    g_object_weak_ref(G_OBJECT(src), (GWeakNotify)disconnect_on_source_deleted, self);
    g_object_weak_ref(G_OBJECT(self), (GWeakNotify)exclude_from_dests_on_dest_deleted, src);

    priv->source = src;
    src_priv->dests = g_slist_append(src_priv->dests, self);
    g_signal_emit(src, MOTO_PARAM_GET_CLASS(src)->dest_added_signal_id, 0);

    moto_node_update(moto_param_get_node(src));
    moto_param_update(self);

    g_signal_emit(self, MOTO_PARAM_GET_CLASS(self)->source_changed_signal_id, 0);
}

void moto_param_unlink_source(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    MotoParamPriv *src_priv = MOTO_PARAM_GET_PRIVATE(priv->source);

    if(src_priv)
    {
        g_object_weak_unref(G_OBJECT(priv->source), (GWeakNotify)disconnect_on_source_deleted, self);
        g_object_weak_unref(G_OBJECT(self), (GWeakNotify)exclude_from_dests_on_dest_deleted, priv->source);

        src_priv->dests = g_slist_remove(src_priv->dests, self);
        g_signal_emit(priv->source, MOTO_PARAM_GET_CLASS(priv->source)->dest_removed_signal_id, 0);
    }
    priv->source = NULL;
}

static void
null_source(gpointer data, gpointer user_data)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(data);
    priv->source = NULL;
}

void moto_param_unlink_dests(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    if(priv->mode == MOTO_PARAM_MODE_IN)
    {
        GString *msg = g_string_new("You are trying to clear destinations of input parameter (\"");
        g_string_append(msg, moto_param_get_name(self));
        g_string_append(msg, "\"). Inputs may not have destinations.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    g_slist_foreach(priv->dests, null_source, NULL);
    g_slist_free(priv->dests);
    priv->dests = NULL;
}

void moto_param_unlink(MotoParam *self)
{
    moto_param_unlink_source(self);
    moto_param_unlink_dests(self);
}

gboolean moto_param_has_dests(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return (g_slist_length(priv->dests) > 0);
}

gboolean moto_param_is_valid(MotoParam *self)
{
    return MOTO_IS_NODE(moto_param_get_node(self));
}

MotoNode *moto_param_get_node(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->node;
}

void moto_param_update(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    if( ! priv->source)
        return;

    MotoParamPriv *src_priv = MOTO_PARAM_GET_PRIVATE(priv->source);

    g_value_transform(& src_priv->value, & priv->value);

    moto_node_update(moto_param_get_node(self));
}

void moto_param_update_dests(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    if( ! (priv->mode & MOTO_PARAM_MODE_OUT))
        return;

    GSList *dest = priv->dests;
    for(; dest; dest = g_slist_next(dest))
        moto_param_update((MotoParam *)dest->data);
}


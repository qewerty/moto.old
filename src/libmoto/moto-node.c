#include <string.h>

#include <moto-python.h>

#include "libmotoutil/moto-mapped-list.h"

#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-filename.h"

#include "moto-node.h"
#include "moto-scene-node.h"
#include "moto-messager.h"
#include "moto-variation.h"

/* forwards */

static void moto_param_update(MotoParam *self);
static void moto_param_mark_for_update(MotoParam *self);

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

static void
unref_gobject(gpointer data, gpointer user_data)
{
    g_object_unref(data);
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

    MotoNode *parent;
    GList *children;

    gboolean ready;

    guint id;

    GString *name;
    MotoSceneNode *scene_node;

    MotoMappedList params;
    MotoMappedList pgroups;

    gboolean hidden;

    GSList *tags;
    GSList *notes;

    /* For exporting optimization. */
    GTimeVal last_modified;
};

struct _MotoParamPriv
{
    gboolean disposed;

    gboolean is_static;

    guint id;

    GValue value;
    MotoParamSpec *pspec;
    MotoParam *source;
    GSList *dests;

    gboolean ready;

    /* Used for determing which IN params this OUT depends on.
     * Only for params with MOTO_PARAM_MODE_OUT flag. */
    GPtrArray *depends_on_params;

    gboolean scriptable; // Only hint for param editor
    gboolean use_expression;
    GString *expression;
    PyObject *expression_function;

    GString *name;
    GString *title;
    MotoNode *node;
    MotoParamMode mode;

    gboolean hidden;
    GTimeVal last_modified; /* For exporting optimization. */

    /* GList *notes; // ??? */
};

/* class MotoNode */

#define MOTO_NODE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_NODE, MotoNodePriv)
#define MOTO_PARAM_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_PARAM, MotoParamPriv)

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

    priv->parent   = NULL;
    priv->children = NULL;

    priv->ready = FALSE;

    static guint id = 0;

    priv->name = g_string_new("");
    priv->scene_node = NULL;

    moto_mapped_list_init(& priv->params);
    moto_mapped_list_init(& priv->pgroups);

    priv->hidden = FALSE;
    g_get_current_time(& priv->last_modified);

    priv->tags = NULL;

    moto_node_define_param_group(self, "Status");

    moto_node_add_params(self,
            "self", "Self", MOTO_TYPE_NODE, MOTO_PARAM_MODE_OUT, self, NULL, "Node",
            NULL);
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

G_DEFINE_ABSTRACT_TYPE(MotoNode, moto_node, G_TYPE_INITIALLY_UNOWNED);

/* Methods of class MotoNode */

MotoNode *moto_create_node(GType type, const gchar *name)
{
    if( ! g_type_is_a(type, MOTO_TYPE_NODE))
    {
        moto_error("Can't create node for type '%s'", g_type_name(type));
        return NULL;
    }

    if(G_TYPE_IS_ABSTRACT(type))
    {
        moto_error("Can't create node of abstract type '%s'", g_type_name(type));
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
        moto_error("Can't create node of type '%s'. Type does not exist.", type_name);
        return NULL;
    }

    return moto_create_node(type, name);
}

MotoNode *moto_node_create_child(MotoNode *self, GType type, const gchar *name)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    MotoNode *node = moto_create_node(type, name);
    if(!node)
        return node;

    GString *new_name = g_string_new(name);
    MotoNode *child;
    gint count = 0;
    while((child = moto_node_get_child(self, new_name->str)))
    {
        g_string_printf(new_name, "%s%i", name, count);
        ++count;
    }
    moto_node_set_name(node, new_name->str);
    g_string_free(new_name, TRUE);

    MOTO_NODE_GET_PRIVATE(node)->parent = self;
    priv->children = g_list_append(priv->children, node);

    moto_node_set_scene_node(node, moto_node_get_scene_node(self));
    return node;
}

MotoNode *moto_node_create_child_by_name(MotoNode *self,
    const gchar *type_name, const gchar *name)
{
    GType type = g_type_from_name(type_name);
    if( ! type)
    {
        moto_error("Can't create node of type '%s'. Type does not exist.", type_name);
        return NULL;
    }

    return moto_node_create_child(self, type, name);
}

MotoNode *moto_node_get_child(MotoNode *self, const gchar *name)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    GList *child = g_list_first(priv->children);
    for(; child; child = g_list_next(child))
    {
        MotoNode *node = (MotoNode*)child->data;
        if(g_utf8_collate(name, moto_node_get_name(node)) == 0)
        {
            return node;
        }
    }
    return NULL;
}

guint moto_node_get_n_children(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return g_list_length(priv->children);
}

GList* moto_node_get_children(MotoNode *self)
{
    return MOTO_NODE_GET_PRIVATE(self)->children;
}

void moto_node_foreach_children(MotoNode *self, GFunc func, gpointer user_data)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    g_list_foreach(priv->children, func, user_data);
}

MotoNode *moto_node_get_parent(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return priv->parent;
}

void moto_node_set_parent(MotoNode *self, MotoNode *parent)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    if(parent)
    {
        // Auto-renaming if farent already has child with this name.
        const gchar *name = moto_node_get_name(self);
        GString *new_name = g_string_new(name);
        gint count = 0;
        MotoNode *child;
        while((child = moto_node_get_child(parent, new_name->str)))
        {
            g_string_printf(new_name, "%s%i", name, count);
            ++count;
        }
        moto_node_set_name(self, new_name->str);
        g_string_free(new_name, TRUE);
    }

    priv->parent = parent;
}

void moto_node_do_action(MotoNode *self, const gchar *action_name)
{
    MotoNodeActionFunc func = \
        (MotoNodeActionFunc)g_datalist_get_data(& MOTO_NODE_GET_CLASS(self)->actions, action_name);
    if( ! func)
    {
        moto_error("Action '%s' of node '%s' is not implemented",
            action_name, moto_node_get_name(self));
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

const gchar *moto_node_get_full_name(MotoNode *self)
{
    return moto_node_get_name(self); // TODO: Implement properly ;)
}

const gchar *moto_node_get_type_name(MotoNode *self)
{
    return g_type_name(G_TYPE_FROM_INSTANCE(self));
}

const gchar *moto_node_get_type_alias(MotoNode *self)
{
    // return moto_get_type_alias(G_TYPE_FROM_INSTANCE(self));
    return NULL;
}

gboolean moto_node_is_valid(MotoNode *self)
{
    return TRUE; // TODO: Implement
}

static void check_param_source(MotoParam *param, gboolean *has)
{
    if(*has)
        return;

    *has = moto_param_get_source(param) != NULL;
}

gboolean moto_node_is_independent(MotoNode *self)
{
    // TODO: Rewrite. Params should notify node on dis/connecting.
    gboolean has_sources = FALSE;
    moto_mapped_list_foreach(& MOTO_NODE_GET_PRIVATE(self)->params,
        (GFunc)check_param_source, & has_sources);
    return has_sources;
}

static void check_param_source_ready(MotoParam *param, gboolean *ready)
{
    if( ! *ready)
        return;

    MotoParam *source = moto_param_get_source(param);
    if(source)
    {
        MotoNode *src_node = moto_param_get_node(source);
        if(src_node)
        {
            *ready = MOTO_NODE_GET_PRIVATE(src_node)->ready;
        }
    }
}

gboolean moto_node_is_ready_to_update(MotoNode *self)
{
    gboolean ready = TRUE;
    moto_mapped_list_foreach(& MOTO_NODE_GET_PRIVATE(self)->params,
        (GFunc)check_param_source_ready, & ready);
    return ready;
}

gboolean moto_node_needs_update(MotoNode *self)
{
    return ! MOTO_NODE_GET_PRIVATE(self)->ready;
}

guint moto_node_get_id(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return priv->id;
}

void moto_node_define_param_group(MotoNode* self, const char* name)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    Group *g = (Group *)moto_mapped_list_get(& priv->pgroups, name);
    if(!g)
    {
        g = group_new(name);
        moto_mapped_list_set(& priv->pgroups, name, g);
    }
}

void moto_node_add_dynamic_param(MotoNode *self, MotoParam *param, const gchar *group)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    Group *g = (Group *)moto_mapped_list_get(& priv->pgroups, group);
    if(!g)
    {
        g = group_new(group);
        moto_mapped_list_set(& priv->pgroups, group, g);
    }
    group_add_param(g, param);

    moto_mapped_list_set(& priv->params, moto_param_get_name(param), param);
}

static void moto_node_add_params_va(MotoNode *self, gboolean is_static, va_list ap)
{
    while(1)
    {
        gchar *pname    = va_arg(ap, gchar*);
        if( ! pname)
            return;
        gchar *ptitle   = va_arg(ap, gchar*);
        GType ptype     = va_arg(ap, GType);
        MotoParamMode pmode = va_arg(ap, MotoParamMode);

        GValue v = {0,};
        if( ! g_type_is_a(ptype, G_TYPE_INTERFACE))
            g_value_init(&v, ptype);
        else
            g_value_init(&v, G_TYPE_OBJECT);

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
                if(g_type_is_a(ptype, MOTO_TYPE_BOOL2))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_bool[2|3|4] when them will be implemented!
                    gboolean *ptr = (gboolean *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gboolean)*2);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_BOOL3))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_bool[2|3|4] when them will be implemented!
                    gfloat *ptr = (gfloat *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gboolean)*3);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_BOOL4))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_bool[2|3|4] when them will be implemented!
                    gboolean *ptr = (gboolean *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gboolean)*4);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_INT2))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_int[2|3|4] when them will be implemented!
                    gint *ptr = (gint *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gint)*2);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_INT3))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_int[2|3|4] when them will be implemented!
                    gfloat *ptr = (gfloat *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gint)*3);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_INT4))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_int[2|3|4] when them will be implemented!
                    gint *ptr = (gint *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gint)*4);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_FLOAT2))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_float[2|3|4] when them will be implemented!
                    gfloat *ptr = (gfloat *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gfloat)*2);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_FLOAT3))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_float[2|3|4] when them will be implemented!
                    gfloat *ptr = (gfloat *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gfloat)*3);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_FLOAT4))
                {
                    // FIXME: Rewrite with moto_value_[g|s]et_float[2|3|4] when them will be implemented!
                    gfloat *ptr = (gfloat *)g_value_peek_pointer(&v);
                    memcpy(ptr, va_arg(ap, gpointer), sizeof(gfloat)*4);
                }
                /*
                else if(g_type_is_a(ptype, MOTO_TYPE_BOOL_ARRAY))
                {
                    moto_value_set_bool_array(&v, NULL, 0);
                }
                else if(g_type_is_a(ptype, MOTO_TYPE_INT_ARRAY))
                {
                    moto_value_set_int_array(&v, NULL, 0);
                }
                */
                else if(g_type_is_a(ptype, MOTO_TYPE_FLOAT_ARRAY))
                {
                    va_arg(ap, gpointer);
                    moto_value_set_float_array(&v, NULL, 0);
                }
                else if(g_type_is_a(ptype, G_TYPE_ENUM))
                {
                    g_value_set_enum(&v, va_arg(ap, gint));
                }
                else if(ptype == MOTO_TYPE_FILENAME)
                {
                    g_value_set_string(&v, va_arg(ap, gchar*));
                }
                else if(g_type_is_a(ptype, G_TYPE_OBJECT) || g_type_is_a(ptype, G_TYPE_INTERFACE))
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
                    // FIXME: Why SegFault?

                    continue;
                }
            break;
        }

        MotoParamSpec *pspec = va_arg(ap, MotoParamSpec*);
        gchar         *group = va_arg(ap, gchar*);

        MotoParam *p = moto_param_new(pname, ptitle, pmode, &v, pspec, self);
        MOTO_PARAM_GET_PRIVATE(p)->is_static = is_static;

        moto_node_add_param(self, p, group);
    }

    va_end(ap);
}

void moto_node_add_params(MotoNode *self, ...)
{
    va_list ap;
    va_start(ap, self);
    moto_node_add_params_va(self, FALSE, ap);
    va_end(ap);
}

void moto_node_add_static_param(MotoNode *self, MotoParam *param, const gchar *group)
{
    MOTO_PARAM_GET_PRIVATE(param)->is_static = TRUE;
    moto_node_add_param(self, param, group);
}

void moto_node_add_static_params(MotoNode *self, ...)
{
    va_list ap;
    va_start(ap, self);
    moto_node_add_params_va(self, TRUE, ap);
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

gboolean moto_node_get_param_boolean(MotoNode *self, const gchar *name, gboolean *v)
{

    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    *v = moto_param_get_boolean(p);
    return TRUE;
}

gboolean moto_node_get_param_int(MotoNode *self, const gchar *name, gint *v)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    *v = moto_param_get_int(p);
    return TRUE;
}

gboolean moto_node_get_param_float(MotoNode *self, const gchar *name, gfloat *v)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    *v = moto_param_get_float(p);
    return TRUE;
}

gboolean moto_node_get_param_string(MotoNode *self, const gchar *name, const gchar **v)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        *v = NULL;
        return FALSE;
    }
    *v = moto_param_get_string(p);
    return TRUE;
}

gboolean moto_node_get_param_pointer(MotoNode *self, const gchar *name, gpointer *v)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        *v = NULL;
        return FALSE;
    }
    *v = moto_param_get_pointer(p);
    return TRUE;
}

gboolean moto_node_get_param_enum(MotoNode *self, const gchar *name, gint *v)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    *v = moto_param_get_enum(p);
    return TRUE;
}

gboolean moto_node_get_param_object(MotoNode *self, const gchar *name, GObject **v)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        *v = NULL;
        return FALSE;
    }
    *v = moto_param_get_object(p);
    return TRUE;
}

gboolean moto_node_set_param_boolean(MotoNode *self, const gchar *name, gboolean value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    moto_param_set_boolean(p, value);
    return TRUE;
}

gboolean moto_node_set_param_int(MotoNode *self, const gchar *name, gint value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    moto_param_set_int(p, value);
    return TRUE;
}

gboolean moto_node_set_param_float(MotoNode *self, const gchar *name, gfloat value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    moto_param_set_float(p, value);
    return TRUE;
}

gboolean moto_node_set_param_pointer(MotoNode *self, const gchar *name, gpointer value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    moto_param_set_pointer(p, value);
    return TRUE;
}

gboolean moto_node_set_param_enum(MotoNode *self, const gchar *name, gint value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    moto_param_set_enum(p, value);
    return TRUE;
}

gboolean moto_node_set_param_object(MotoNode *self, const gchar *name, GObject *value)
{
    MotoParam *p = moto_node_get_param(self, name);
    if( ! p)
    {
        return FALSE;
    }
    moto_param_set_object(p, value);
    return TRUE;
}

// boolean

gboolean moto_node_set_param_2b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_2b(param, v0, v1);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_2bv(MotoNode *self, const gchar *name, const gboolean *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_2bv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_3b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1, gboolean v2)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_3b(param, v0, v1, v2);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_3bv(MotoNode *self, const gchar *name, const gboolean *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_3bv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_4b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1, gboolean v2, gboolean v3)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_4b(param, v0, v1, v2, v3);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_4bv(MotoNode *self, const gchar *name, const gboolean *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_4bv(param, v);
        return TRUE;
    }
    return FALSE;
}

// int

gboolean moto_node_set_param_2i(MotoNode *self,  const gchar *name, gint v0, gint v1)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_2i(param, v0, v1);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_2iv(MotoNode *self, const gchar *name, const gint *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_2iv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_3i(MotoNode *self,  const gchar *name, gint v0, gint v1, gint v2)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_3i(param, v0, v1, v2);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_3iv(MotoNode *self, const gchar *name, const gint *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_3iv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_4i(MotoNode *self,  const gchar *name, gint v0, gint v1, gint v2, gint v3)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_4i(param, v0, v1, v2, v3);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_4iv(MotoNode *self, const gchar *name, const gint *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_4iv(param, v);
        return TRUE;
    }
    return FALSE;
}

// float

gboolean moto_node_set_param_2f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_2f(param, v0, v1);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_2fv(MotoNode *self, const gchar *name, const gfloat *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_2fv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_3f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1, gfloat v2)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_3f(param, v0, v1, v2);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_3fv(MotoNode *self, const gchar *name, const gfloat *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_3fv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_4f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1, gfloat v2, gfloat v3)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_4f(param, v0, v1, v2, v3);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_set_param_4fv(MotoNode *self, const gchar *name, const gfloat *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_set_4fv(param, v);
        return TRUE;
    }
    return FALSE;
}

// boolean

gboolean moto_node_get_param_2b(MotoNode *self, const gchar *name, gboolean *v0, gboolean *v1)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_2b(param, v0, v1);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_2bv(MotoNode *self, const gchar *name, gboolean *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_2bv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_3b(MotoNode *self,  const gchar *name, gboolean *v0, gboolean *v1, gboolean *v2)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_3b(param, v0, v1, v2);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_3bv(MotoNode *self, const gchar *name, gboolean *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_3bv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_4b(MotoNode *self,  const gchar *name, gboolean *v0, gboolean *v1, gboolean *v2, gboolean *v3)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_4b(param, v0, v1, v2, v3);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_4bv(MotoNode *self, const gchar *name, gboolean *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_4bv(param, v);
        return TRUE;
    }
    return FALSE;
}

// int

gboolean moto_node_get_param_2i(MotoNode *self, const gchar *name, gint *v0, gint *v1)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_2i(param, v0, v1);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_2iv(MotoNode *self, const gchar *name, gint *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_2iv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_3i(MotoNode *self,  const gchar *name, gint *v0, gint *v1, gint *v2)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_3i(param, v0, v1, v2);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_3iv(MotoNode *self, const gchar *name, gint *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_3iv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_4i(MotoNode *self,  const gchar *name, gint *v0, gint *v1, gint *v2, gint *v3)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_4i(param, v0, v1, v2, v3);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_4iv(MotoNode *self, const gchar *name, gint *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_4iv(param, v);
        return TRUE;
    }
    return FALSE;
}

// float

gboolean moto_node_get_param_2f(MotoNode *self, const gchar *name, gfloat *v0, gfloat *v1)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_2f(param, v0, v1);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_2fv(MotoNode *self, const gchar *name, gfloat *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_2fv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_3f(MotoNode *self,  const gchar *name, gfloat *v0, gfloat *v1, gfloat *v2)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_3f(param, v0, v1, v2);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_3fv(MotoNode *self, const gchar *name, gfloat *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_3fv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_4f(MotoNode *self,  const gchar *name, gfloat *v0, gfloat *v1, gfloat *v2, gfloat *v3)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_4f(param, v0, v1, v2, v3);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_param_4fv(MotoNode *self, const gchar *name, gfloat *v)
{
    MotoParam *param = moto_node_get_param(self, name);
    if(param)
    {
        moto_param_get_4fv(param, v);
        return TRUE;
    }
    return FALSE;
}

gboolean moto_node_get_params(MotoNode *self, ...)
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
            va_end(ap);
            return FALSE;
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
    return TRUE;
}

gboolean moto_node_set_params(MotoNode *self, ...)
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
            va_end(ap);
            return FALSE;
        }

        GValue *v = moto_param_get_value(p);
        GType ptype = G_VALUE_TYPE(v);

        switch(ptype)
        {
            case G_TYPE_BOOLEAN:
            {
                g_value_set_boolean(v, va_arg(ap, gboolean));
            }
            break;
            case G_TYPE_INT:
            {
                g_value_set_int(v, va_arg(ap, gint));
            }
            break;
            case G_TYPE_UINT:
            {
                g_value_set_uint(v, va_arg(ap, guint));
            }
            break;
            case G_TYPE_LONG:
            {
                g_value_set_long(v, va_arg(ap, glong));
            }
            break;
            case G_TYPE_ULONG:
            {
                g_value_set_ulong(v, va_arg(ap, gulong));
            }
            break;
            case G_TYPE_INT64:
            {
                g_value_set_int64(v, va_arg(ap, gint64));
            }
            break;
            case G_TYPE_UINT64:
            {
                g_value_set_uint64(v, va_arg(ap, guint64));
            }
            break;
            case G_TYPE_FLOAT:
            {
                g_value_set_float(v, (gfloat)va_arg(ap, gdouble));
            }
            break;
            case G_TYPE_DOUBLE:
            {
                g_value_set_double(v, va_arg(ap, gdouble));
            }
            break;
            case G_TYPE_POINTER:
            {
                g_value_set_pointer(v, va_arg(ap, gpointer));
            }
            break;
            default:
            {
                if(g_type_is_a(ptype, G_TYPE_ENUM))
                {
                    g_value_set_enum(v, va_arg(ap, gint));
                }
                else
                {
                    g_value_set_object(v, va_arg(ap, gpointer));
                }
            }
        }
    }

    va_end(ap);
    return TRUE;
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

guint moto_node_get_n_params_in_group(MotoNode* self, const char* name)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    Group* g = (Group*)moto_mapped_list_get(&priv->pgroups, name);
    if(!g)
        return 0;
    return g_slist_length(g->params);
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

static void update_param(MotoParam *param, gpointer user_data)
{
    if( ! MOTO_PARAM_GET_PRIVATE(param)->ready)
        moto_param_update(param);
}

void moto_node_update(MotoNode *self)
{
    MotoNodePriv  *priv  = MOTO_NODE_GET_PRIVATE(self);
    MotoNodeClass *klass = MOTO_NODE_GET_CLASS(self);

    moto_mapped_list_foreach( & priv->params, (GFunc)update_param, NULL);

    if(klass->update)
        klass->update(self);

    moto_node_update_last_modified(self);
    priv->ready = TRUE;
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

MotoSceneNode *moto_node_get_scene_node(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    return priv->scene_node;
}

void moto_node_set_scene_node(MotoNode *self, MotoSceneNode *scene_node)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);
    priv->scene_node = scene_node;
}

MotoLibrary *moto_node_get_library(MotoNode *self)
{
    MotoSceneNode *w = moto_node_get_scene_node(self);
    if( ! w)
        return NULL;
    return moto_scene_node_get_library(w);
}

gboolean moto_node_depends_on(MotoNode *self, MotoNode *other)
{
    if(self == other)
        return TRUE;

    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    GSList *p = priv->params.sl;
    for(; p; p = g_slist_next(p))
    {
        MotoParam *param = (MotoParam *)p->data;
        if( ! (moto_param_get_mode(param) & MOTO_PARAM_MODE_IN))
            continue;
        MotoParam *src = moto_param_get_source(param);
        if( ! src)
            continue;

        MotoNode *node = moto_param_get_node(src);
        if(moto_node_depends_on(node, other))
            return TRUE;
    }

    return FALSE;
}

gboolean moto_node_is_animated(MotoNode *self)
{
    MotoNodePriv *priv = MOTO_NODE_GET_PRIVATE(self);

    GSList *p = priv->params.sl;
    for(; p; p = g_slist_next(p))
    {
        MotoParam *param = (MotoParam *)p->data;
        if(moto_param_is_animated(param))
            return TRUE;
    }

    return FALSE;
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

    if(priv->pspec)
        g_object_unref(priv->pspec);

    g_string_free(priv->name, TRUE);
    g_string_free(priv->title, TRUE);

    if(priv->depends_on_params) // FIXME: Remove depends_on_params?
        g_ptr_array_free(priv->depends_on_params, TRUE);
    g_string_free(priv->expression, TRUE);

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

    priv->is_static = FALSE;

    static guint id = 0;
    // FIXME: Implement generating unique ids correcly even when
    // scene loaded from file and params are saved in variations.
    priv->id = ++id;

    priv->source = NULL;
    priv->dests = NULL;

    priv->ready = FALSE;

    priv->depends_on_params = NULL;

    priv->scriptable     = TRUE;
    priv->use_expression = FALSE;
    priv->expression = g_string_new("");
    priv->expression_function = NULL;

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

/* Methods of class MotoParam */

MotoParam *moto_param_new(const gchar *name, const gchar *title,
        MotoParamMode mode, GValue *value, MotoParamSpec *pspec,
        MotoNode *node)
{
    return moto_param_new_full(name, title, mode, value, pspec, node, TRUE, FALSE);
}

MotoParam *moto_param_new_full(const gchar *name, const gchar *title,
        MotoParamMode mode, GValue *value, MotoParamSpec *pspec,
        MotoNode *node, gboolean scriptable, gboolean is_static)
{
    if( ! node)
        return NULL;
    if( ! value)
        return NULL;

    GValue none = {0, };

    MotoParam *self = (MotoParam *)g_object_new(MOTO_TYPE_PARAM, NULL);
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    g_string_assign(priv->name, name);
    g_string_assign(priv->title, title);
    priv->mode = mode;
    priv->node = node;

    priv->value = none;
    g_value_init( & priv->value, G_VALUE_TYPE(value));
    g_value_copy(value, & priv->value);

    GType ptype = G_VALUE_TYPE( & priv->value);

    if(pspec)
    {
        GType spec_type = moto_param_spec_get_value_type(pspec);
        if(g_type_is_a(ptype, spec_type))
        {
            priv->pspec = moto_param_spec_copy(pspec);
        }
        else
        {
            moto_error("ParamSpec %s/%s is not compatibale with parameter %s.%s of %s type. I ignore it.",
                       g_type_name(G_TYPE_FROM_INSTANCE(pspec)), g_type_name(spec_type),
                       g_type_name(G_TYPE_FROM_INSTANCE(node)), moto_param_get_name(self),
                       g_type_name(G_VALUE_TYPE(value)));

            priv->pspec = NULL;
        }
    }
    else
    {
        priv->pspec = NULL;
    }

    priv->scriptable = scriptable;
    priv->is_static  = is_static;

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

MotoParamSpec *moto_param_get_spec(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->pspec;
}

guint moto_param_get_id(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->id;
}

GValue *moto_param_get_value(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return & priv->value;
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

gfloat moto_param_get_float(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return g_value_get_float(& priv->value);
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

// boolean

void moto_param_get_2b(MotoParam *self,  gboolean *v0, gboolean *v1)
{
    moto_value_get_bool2(moto_param_get_value(self), v0, v1);
}

void moto_param_get_2bv(MotoParam *self, gboolean *v)
{
    moto_value_get_bool2_v(moto_param_get_value(self), v);
}

void moto_param_get_3b(MotoParam *self,  gboolean *v0, gboolean *v1, gboolean *v2)
{
    moto_value_get_bool3(moto_param_get_value(self), v0, v1, v2);
}

void moto_param_get_3bv(MotoParam *self, gboolean *v)
{
    moto_value_get_bool3_v(moto_param_get_value(self), v);
}

void moto_param_get_4b(MotoParam *self,  gboolean *v0, gboolean *v1, gboolean *v2, gboolean *v3)
{
    moto_value_get_bool4(moto_param_get_value(self), v0, v1, v2, v3);
}

void moto_param_get_4bv(MotoParam *self, gboolean *v)
{
    moto_value_get_bool4_v(moto_param_get_value(self), v);
}

// int

void moto_param_get_2i(MotoParam *self, gint *v0, gint *v1)
{
    moto_value_get_int2(moto_param_get_value(self), v0, v1);
}

void moto_param_get_2iv(MotoParam *self, gint *v)
{
    moto_value_get_int2_v(moto_param_get_value(self), v);
}

void moto_param_get_3i(MotoParam *self, gint *v0, gint *v1, gint *v2)
{
    moto_value_get_int3(moto_param_get_value(self), v0, v1, v2);
}

void moto_param_get_3iv(MotoParam *self, gint *v)
{
    moto_value_get_int3_v(moto_param_get_value(self), v);
}

void moto_param_get_4i(MotoParam *self, gint *v0, gint *v1, gint *v2, gint *v3)
{
    moto_value_get_int4(moto_param_get_value(self), v0, v1, v2, v3);
}

void moto_param_get_4iv(MotoParam *self, gint *v)
{
    moto_value_get_int4_v(moto_param_get_value(self), v);
}

// float

void moto_param_get_2f(MotoParam *self,  gfloat *v0, gfloat *v1)
{
    moto_value_get_float2(moto_param_get_value(self), v0, v1);
}

void moto_param_get_2fv(MotoParam *self, gfloat *v)
{
    moto_value_get_float2_v(moto_param_get_value(self), v);
}

void moto_param_get_3f(MotoParam *self,  gfloat *v0, gfloat *v1, gfloat *v2)
{
    moto_value_get_float3(moto_param_get_value(self), v0, v1, v2);
}

void moto_param_get_3fv(MotoParam *self, gfloat *v)
{
    moto_value_get_float3_v(moto_param_get_value(self), v);
}

void moto_param_get_4f(MotoParam *self,  gfloat *v0, gfloat *v1, gfloat *v2, gfloat *v3)
{
    moto_value_get_float4(moto_param_get_value(self), v0, v1, v2, v3);
}

void moto_param_get_4fv(MotoParam *self, gfloat *v)
{
    moto_value_get_float4_v(moto_param_get_value(self), v);
}

void moto_param_set_boolean(MotoParam *self, gboolean value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_boolean( & priv->value, value);
    moto_param_notify_dests(self);
}

void moto_param_set_int(MotoParam *self, gint value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_int(& priv->value, value);
    moto_param_notify_dests(self);
}

void moto_param_set_float(MotoParam *self, gfloat value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_float(& priv->value, value);
    moto_param_notify_dests(self);
}

void moto_param_set_string(MotoParam *self, const gchar *value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_string(& priv->value, value);
    moto_param_notify_dests(self);
}

void moto_param_set_pointer(MotoParam *self, gpointer value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_pointer(& priv->value, value);
    moto_param_notify_dests(self);
}

void moto_param_set_enum(MotoParam *self, gint value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_enum(& priv->value, value);
    moto_param_notify_dests(self);
}

void moto_param_set_object(MotoParam *self, GObject *value)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_value_set_object(& priv->value, value);
    moto_param_notify_dests(self);
}

// boolean

void moto_param_set_2b(MotoParam *self, gboolean v0, gboolean v1)
{
    moto_value_set_bool2(moto_param_get_value(self), v0, v1);
    moto_param_notify_dests(self);
}

void moto_param_set_2bv(MotoParam *self, const gboolean *v)
{
    moto_value_set_bool2_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
}

void moto_param_set_3b(MotoParam *self, gboolean v0, gboolean v1, gboolean v2)
{
    moto_value_set_bool3(moto_param_get_value(self), v0, v1, v2);
    moto_param_notify_dests(self);
}

void moto_param_set_3bv(MotoParam *self, const gboolean *v)
{
    moto_value_set_bool3_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
}

void moto_param_set_4b(MotoParam *self, gboolean v0, gboolean v1, gboolean v2, gboolean v3)
{
    moto_value_set_bool4(moto_param_get_value(self), v0, v1, v2, v3);
    moto_param_notify_dests(self);
}

void moto_param_set_4bv(MotoParam *self, const gboolean *v)
{
    moto_value_set_bool4_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
}

// int

void moto_param_set_2i(MotoParam *self, gint v0, gint v1)
{
    moto_value_set_int2(moto_param_get_value(self), v0, v1);
    moto_param_notify_dests(self);
}

void moto_param_set_2iv(MotoParam *self, const gint *v)
{
    moto_value_set_int2_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
}

void moto_param_set_3i(MotoParam *self, gint v0, gint v1, gint v2)
{
    moto_value_set_int3(moto_param_get_value(self), v0, v1, v2);
    moto_param_notify_dests(self);
}

void moto_param_set_3iv(MotoParam *self, const gint *v)
{
    moto_value_set_int3_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
}

void moto_param_set_4i(MotoParam *self, gint v0, gint v1, gint v2, gint v3)
{
    moto_value_set_int4(moto_param_get_value(self), v0, v1, v2, v3);
    moto_param_notify_dests(self);
}

void moto_param_set_4iv(MotoParam *self, const gint *v)
{
    moto_value_set_int4_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
}

// float

void moto_param_set_2f(MotoParam *self, gfloat v0, gfloat v1)
{
    moto_value_set_float2(moto_param_get_value(self), v0, v1);
    moto_param_notify_dests(self);
}

void moto_param_set_2fv(MotoParam *self, const gfloat *v)
{
    moto_value_set_float2_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
}

void moto_param_set_3f(MotoParam *self, gfloat v0, gfloat v1, gfloat v2)
{
    moto_value_set_float3(moto_param_get_value(self), v0, v1, v2);
    moto_param_notify_dests(self);
}

void moto_param_set_3fv(MotoParam *self, const gfloat *v)
{
    moto_value_set_float3_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
}

void moto_param_set_4f(MotoParam *self, gfloat v0, gfloat v1, gfloat v2, gfloat v3)
{
    moto_value_set_float4(moto_param_get_value(self), v0, v1, v2, v3);
    moto_param_notify_dests(self);
}

void moto_param_set_4fv(MotoParam *self, const gfloat *v)
{
    moto_value_set_float4_v(moto_param_get_value(self), v);
    moto_param_notify_dests(self);
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
}

static void
exclude_from_dests_on_dest_deleted(gpointer data, GObject *where_the_object_was)
{
    MotoParam *param = (MotoParam *)data;
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(param);

    priv->dests = g_slist_remove(priv->dests, where_the_object_was);
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

    MotoNode *self_node = moto_param_get_node(self);
    MotoNode *src_node  = moto_param_get_node(src);

    if(moto_node_depends_on(src_node, self_node))
    {
        moto_error("Connection of '%s.%s'->'%s.%s' makes a cycle. I won't connect it",
                moto_node_get_name(src_node), moto_param_get_name(src),
                moto_node_get_name(self_node), moto_param_get_name(self));
        return;
    }

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

    /* Type checking. */
    GType self_type = moto_param_get_value_type(self);
    GType src_type = moto_param_get_value_type(src);
    if(g_type_is_a(self_type, G_TYPE_OBJECT) || g_type_is_a(self_type, G_TYPE_INTERFACE))
    {
        if(!g_type_is_a(src_type, G_TYPE_OBJECT))
        {
            moto_error("Can't link param of not derived type (src: %s; dst: %s)",
                g_type_name(src_type), g_type_name(self_type));
            return;
        }
    }
    else
    {
        if(g_type_is_a(src_type, G_TYPE_OBJECT) || g_type_is_a(src_type, G_TYPE_INTERFACE))
        {

            return;
        }
        if( ! g_value_type_transformable(src_type, self_type))
            return;
    }

    g_object_weak_ref(G_OBJECT(src),  (GWeakNotify)disconnect_on_source_deleted,       self);
    g_object_weak_ref(G_OBJECT(self), (GWeakNotify)exclude_from_dests_on_dest_deleted, src);

    priv->source = src;
    src_priv->dests = g_slist_append(src_priv->dests, self);

    moto_param_mark_for_update(self);
}

void moto_param_unlink_source(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    if( ! priv->source)
        return;

    MotoParamPriv *src_priv = MOTO_PARAM_GET_PRIVATE(priv->source);

    g_object_weak_unref(G_OBJECT(priv->source), (GWeakNotify)disconnect_on_source_deleted,       self);
    g_object_weak_unref(G_OBJECT(self),         (GWeakNotify)exclude_from_dests_on_dest_deleted, priv->source);

    src_priv->dests = g_slist_remove(src_priv->dests, self);
    priv->source = NULL;
}

static void null_source(gpointer data, gpointer user_data)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(data);
    priv->source = NULL;
}

void moto_param_unlink_dests(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    if(priv->mode == MOTO_PARAM_MODE_IN)
    {
        moto_warning("You are trying to clear destinations of input parameter '%s'. Inputs may not have destinations.",
            moto_param_get_name(self));
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

const GSList* moto_param_get_dests(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->dests;
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

gboolean moto_param_is_animated(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    if(priv->mode & MOTO_PARAM_MODE_IN)
    {
        /* Evaluation order: value, source, expression (can use value and source).
         * If expresion is invalid source is used if it's not NULL else value of param. */
        if(priv->use_expression)
        {
            return TRUE;
        }
        else if(priv->source)
        {
            return moto_param_is_animated(priv->source);
        }
    }
    if(priv->mode & MOTO_PARAM_MODE_OUT)
    {
        if(priv->depends_on_params)
        {
            guint i;
            for(i = 0; i < priv->depends_on_params->len; i++)
            {
                MotoParam *dp = (MotoParam *)g_ptr_array_index(priv->depends_on_params, i);
                if(moto_param_is_animated(dp))
                    return TRUE;
            }
        }
    }

    return FALSE;
}

MotoNode *moto_param_get_node(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->node;
}

gboolean moto_param_is_static(MotoParam *self)
{
    return MOTO_PARAM_GET_PRIVATE(self)->is_static;
}

// Dealing with expressions

void moto_param_set_scriptable(MotoParam *self, gboolean scriptable)
{
    MOTO_PARAM_GET_PRIVATE(self)->scriptable = scriptable;
}

gboolean moto_param_get_scriptable(MotoParam *self)
{
    return MOTO_PARAM_GET_PRIVATE(self)->scriptable;
}

void moto_param_set_use_expression(MotoParam *self, gboolean use)
{
    MOTO_PARAM_GET_PRIVATE(self)->use_expression = use;
    if(use)
        moto_param_eval(self);
}

gboolean moto_param_get_use_expression(MotoParam *self)
{
    return MOTO_PARAM_GET_PRIVATE(self)->use_expression;
}

void moto_param_set_expression(MotoParam *self, const gchar *body)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    g_string_assign(priv->expression, body);

    if(priv->expression_function)
    {
        Py_DECREF(priv->expression_function);
    }

    priv->expression_function = \
        moto_PyFunction_from_args_and_body("(p=None, v=None, s=None)", body);
}

const gchar *moto_param_get_expression(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    return priv->expression->str;
}

gboolean moto_param_eval(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    PyObject *result = NULL;
    if(priv->expression_function)
    {
        PyObject *args = PyTuple_New(3);
        PyTuple_SetItem(args, 0, PyString_FromString(moto_param_get_name(self)));

        PyTuple_SetItem(args, 1, moto_PyObject_from_GValue( & priv->value));

        if(priv->source)
        {
            PyTuple_SetItem(args, 2, moto_PyObject_from_GValue(moto_param_get_value(priv->source)));
        }
        else
        {
            Py_INCREF(Py_None);
            PyTuple_SetItem(args, 2, Py_None);
        }

        result = PyObject_CallObject(priv->expression_function, args);
    }

    gboolean status = FALSE;
    if(result)
    {
        status = moto_GValue_from_PyObject(moto_param_get_value(self), result);
        Py_DECREF(result);
    }

    return status;
}

static void moto_param_update(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    gboolean use_source = TRUE;

    if(priv->use_expression && moto_param_eval(self)) // Always update if expression is used
        use_source = FALSE;

    if(use_source && priv->source)
    {
        MotoParamPriv *src_priv = MOTO_PARAM_GET_PRIVATE(priv->source);


        GType dst_type = G_VALUE_TYPE(&priv->value);
        if(g_type_is_a(dst_type, G_TYPE_OBJECT))
        {
            if(!g_type_is_a(dst_type, G_VALUE_TYPE(&src_priv->value)))
            {
                moto_error("Can't update parameter '%s'.\n", moto_param_get_name(self));
                return;
            }
        }

        g_value_transform(&src_priv->value, &priv->value);
    }

    MotoNode *node = moto_param_get_node(self);
    if(node)
        MOTO_NODE_GET_PRIVATE(node)->ready = FALSE;
}

static void moto_param_mark_for_update(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);
    priv->ready = FALSE;

    MotoNode *node = moto_param_get_node(self);
    if(node)
        MOTO_NODE_GET_PRIVATE(node)->ready = FALSE;
}

void moto_param_notify_dests(MotoParam *self)
{
    MotoParamPriv *priv = MOTO_PARAM_GET_PRIVATE(self);

    if( ! (priv->mode & MOTO_PARAM_MODE_OUT))
        return;

    GSList *dest = priv->dests;
    for(; dest; dest = g_slist_next(dest))
    {
        moto_param_mark_for_update((MotoParam *)dest->data);
    }
}


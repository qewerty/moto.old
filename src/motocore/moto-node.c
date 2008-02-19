#include "moto-node.h"
#include "moto-param-types.h"

/* utils */

static void
unref_gobject(gpointer data, gpointer user_data)
{
    g_object_unref(data);
}

/* class Node */

static GObjectClass *node_parent_class = NULL;

struct _MotoNodePriv
{
    GString *name;
    GList *param_blocks;

    gboolean hidden;

    /* For exporting optimization. */
    GTimeVal last_modified;

    GSList *tags;
    GSList *notes;
};

static void
moto_node_dispose(GObject *obj)
{
    MotoNode *self = (MotoNode *)obj;

    g_string_free(self->priv->name, TRUE);
    g_list_foreach(self->priv->param_blocks, unref_gobject, NULL);
    g_list_free(self->priv->param_blocks);
    g_slice_free(MotoNodePriv, self->priv);

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
    self->priv = (MotoNodePriv *)g_slice_new(MotoNodePriv);

    self->priv->name = g_string_new("");
    self->priv->param_blocks = NULL;
    self->priv->hidden = FALSE;
    g_get_current_time(& self->priv->last_modified);

    self->priv->tags = NULL;
}

static void
moto_node_class_init(MotoNodeClass *klass)
{
    node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    node_parent_class->dispose = moto_node_dispose;
    node_parent_class->finalize = moto_node_finalize;
}

G_DEFINE_ABSTRACT_TYPE(MotoNode, moto_node, G_TYPE_OBJECT);

/* methods of class Node */

const gchar *moto_node_get_name(MotoNode *self)
{
    return self->priv->name->str;
}

void moto_node_set_name(MotoNode *self, const gchar *name)
{
    g_string_assign(self->priv->name, name);
}

MotoParamBlock *moto_node_get_param_block(MotoNode *self, const gchar *name)
{
    GList *pb = g_list_first(self->priv->param_blocks);
    for(; pb; pb = g_list_next(pb))
    {
        if(g_utf8_collate(moto_param_block_get_name(MOTO_PARAM_BLOCK(pb->data)), name))
        {
            return (MotoParamBlock *)(pb->data);
        }
    }
    return NULL;
}

void moto_node_add_param_block(MotoNode *self, MotoParamBlock *pb)
{

}

void moto_node_del_param_block(MotoNode *self, MotoParamBlock *pb)
{

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
    for(; curtag; curtag=g_slist_next(tag))
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
    for(; curtag; curtag=g_slist_next(tag))
    {
        if(g_utf8_collate(((GString *)curtag->data)->str, tag) == 0)
            return TRUE;
    }
    return FALSE;
}

/* class Param */

static GObjectClass *param_parent_class = NULL;

struct _MotoParamPriv
{
    GValue value;
    MotoParamMode mode;

    MotoParam *source;
    GSList *dests;

    GString *name;
    GString *title;

    gboolean hidden;
    GTimeVal last_modified; /* For exporting optimization. */

    MotoNode *node;
    MotoParamBlock *pb;

    /* GList *notes; // ??? */
};

static void
moto_param_dispose(GObject *obj)
{
    MotoParam *self = (MotoParam *)obj;

    param_parent_class->dispose(obj);

    g_string_free(self->priv->name, TRUE);
    g_string_free(self->priv->title, TRUE);
    g_slice_free(MotoParamPriv, self->priv);
}

static void
moto_param_finalize(GObject *obj)
{
    G_OBJECT_CLASS(param_parent_class)->finalize(obj);
}

static void
moto_param_init(MotoParam *self)
{
    self->priv = (MotoParamPriv *)g_slice_new(MotoParamPriv);

    self->priv->source = NULL;
    self->priv->dests = NULL;

    self->priv->name = g_string_new("");
    self->priv->title = g_string_new("");

    self->priv->mode = MOTO_PARAM_MODE_IN;

    self->priv->hidden = FALSE;
    g_get_current_time(& self->priv->last_modified);

    self->priv->pb = NULL;
}

static void
moto_param_class_init(MotoParamClass *klass)
{
    param_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    param_parent_class->dispose = moto_param_dispose;
    param_parent_class->finalize = moto_param_finalize;
}

G_DEFINE_TYPE(MotoParam, moto_param, G_TYPE_OBJECT);

/* methods of class Param */

MotoParam *moto_param_new(const gchar *name, MotoParamMode mode,
        const gchar *title, MotoParamBlock *pb)
{
    MotoParam *self = (MotoParam *)g_object_new(MOTO_TYPE_PARAM, NULL);

    g_string_assign(self->priv->name, name);
    g_string_assign(self->priv->title, title);

    self->priv->mode = mode;

    g_assert(pb);
    self->priv->pb = pb;

    return self;
}

void moto_param_set_from_string(MotoParam *self,
        const gchar *string)
{

}

void moto_param_set_source(MotoParam *self, MotoParam *src)
{
    if(self->priv->mode == MOTO_PARAM_MODE_OUT)
    {
        /* Warning! */
        return;
    }

    if(src == self->priv->source)
        return;

    self->priv->source = src;
    src->priv->dests = g_slist_append(src->priv->dests, self);
}

void moto_param_clear_source(MotoParam *self)
{

    if(self->priv->source)
        self->priv->dests = g_slist_remove(self->priv->dests, self);
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
        /* Warning! */
        return;
    }

    g_slist_foreach(self->priv->dests, null_source, NULL);
    g_slist_free(self->priv->dests);
    self->priv->dests = NULL;
}

gboolean moto_param_is_valid(MotoParam *self)
{
    return (MOTO_IS_PARAM_BLOCK(self->priv->pb) && \
            MOTO_IS_NODE(moto_param_block_get_node(self->priv->pb)));
}

/* class ParamBlock */

static GObjectClass *param_block_parent_class = NULL;

struct _MotoParamBlockPriv
{
    GString *name;
    GString *title;
    gboolean hidden;

    GList *params;

    MotoNode *node;
};

static void
moto_param_block_dispose(GObject *obj)
{
    MotoParamBlock *self = (MotoParamBlock *)obj;

    G_OBJECT_CLASS(param_block_parent_class)->dispose(obj);

    g_string_free(self->priv->name, TRUE);
    g_list_foreach(self->priv->params, unref_gobject, NULL);
    g_list_free(self->priv->params);
    g_slice_free(MotoParamBlockPriv, self->priv);
}

static void
moto_param_block_finalize(GObject *obj)
{
    G_OBJECT_CLASS(param_block_parent_class)->finalize(obj);
}

static void
moto_param_block_init(MotoParamBlock *self)
{
    self->priv = (MotoParamBlockPriv *)g_slice_new(MotoParamBlockPriv);

    self->priv->name = g_string_new("");
    self->priv->title = g_string_new("");
    self->priv->hidden = FALSE;

    self->priv->params = NULL;

    self->priv->node = NULL;
}

static void
moto_param_block_class_init(MotoParamBlockClass *klass)
{
    param_block_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    param_block_parent_class->dispose = moto_param_block_dispose;
    param_block_parent_class->finalize = moto_param_block_finalize;
}

G_DEFINE_TYPE(MotoParamBlock, moto_param_block, G_TYPE_OBJECT);

/* methods of class ParamBlock */

MotoParamBlock *moto_param_block_new(const gchar *name, const gchar *title,
        MotoNode *node)
{
    MotoParamBlock *self = (MotoParamBlock *)g_object_new(MOTO_TYPE_PARAM_BLOCK, NULL);

    g_string_assign(self->priv->name, name);
    g_string_assign(self->priv->title, title);

    g_assert(node);
    self->priv->node = node;

    return self;
}

const gchar *moto_param_block_get_name(MotoParamBlock *self)
{
    return self->priv->name->str;
}

void moto_param_block_set_name(MotoParamBlock *self, const gchar *name)
{
    g_string_assign(self->priv->name, name);
}

const gchar *moto_param_block_get_title(MotoParamBlock *self)
{
    return self->priv->title->str;
}

void moto_param_block_set_title(MotoParamBlock *self, const gchar *title)
{
    g_string_assign(self->priv->title, title);
}

const MotoNode *moto_param_block_get_node(MotoParamBlock *self)
{
    return self->priv->node;
}

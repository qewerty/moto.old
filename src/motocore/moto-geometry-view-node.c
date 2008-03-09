#include <GL/gl.h>

#include "moto-messager.h"
#include "moto-geometry-view-node.h"

/* class GeometryViewNode */

static GObjectClass *geometry_view_node_parent_class = NULL;

struct _MotoGeometryViewNodePriv
{
    gboolean prepared;
    MotoGeometryViewState *state;
};

static void
moto_geometry_view_node_dispose(GObject *obj)
{
    MotoGeometryViewNode *self = (MotoGeometryViewNode *)obj;

    g_slice_free(MotoGeometryViewNodePriv, self->priv);

    geometry_view_node_parent_class->dispose(obj);
}

static void
moto_geometry_view_node_finalize(GObject *obj)
{
    geometry_view_node_parent_class->finalize(obj);
}

static void
moto_geometry_view_node_init(MotoGeometryViewNode *self)
{
    self->priv = g_slice_new(MotoGeometryViewNodePriv);

    self->priv->prepared = FALSE;
    self->priv->state = NULL;
}

static void
moto_geometry_view_node_class_init(MotoGeometryViewNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    geometry_view_node_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    goclass->dispose    = moto_geometry_view_node_dispose;
    goclass->finalize   = moto_geometry_view_node_finalize;

    klass->draw = NULL;
    klass->prepare_for_draw = NULL;

    klass->states = NULL;

    klass->before_draw_signal_id = g_signal_newv ("before-draw",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->after_draw_signal_id = g_signal_newv ("after-draw",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->before_prepare_for_draw_signal_id = g_signal_newv ("before-prepare-for-draw",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->after_prepare_for_draw_signal_id = g_signal_newv ("after-prepare-for-draw",
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

G_DEFINE_ABSTRACT_TYPE(MotoGeometryViewNode, moto_geometry_view_node, MOTO_TYPE_NODE);

/* methods of class GeometryViewNode */

void moto_geometry_view_node_draw(MotoGeometryViewNode *self)
{
    MotoGeometryViewNodeClass *klass = MOTO_GEOMETRY_VIEW_NODE_GET_CLASS(self);

    g_signal_emit(self, klass->before_draw_signal_id, 0, NULL);

    if(klass->draw)
        klass->draw(self);

    g_signal_emit(self, klass->after_draw_signal_id, 0, NULL);
}

void moto_geometry_view_node_prepare_for_draw(MotoGeometryViewNode *self)
{
    MotoGeometryViewNodeClass *klass = MOTO_GEOMETRY_VIEW_NODE_GET_CLASS(self);

    g_signal_emit(self, klass->before_prepare_for_draw_signal_id, 0, NULL);

    if(klass->prepare_for_draw)
        klass->prepare_for_draw(self);

    g_signal_emit(self, klass->after_prepare_for_draw_signal_id, 0, NULL);
}

gboolean moto_geometry_view_node_get_prepared(MotoGeometryViewNode *self)
{
    return self->priv->prepared;
}

void moto_geometry_view_node_set_prepared(MotoGeometryViewNode *self, gboolean status)
{
    self->priv->prepared = status;
}

MotoGeometryViewState *moto_geometry_view_node_get_state(MotoGeometryViewNode *self)
{
    return self->priv->state;
}

void moto_geometry_view_node_set_state(MotoGeometryViewNode *self, const gchar *state_name)
{
    GSList *state = MOTO_GEOMETRY_VIEW_NODE_GET_CLASS(self)->states;
    for(; state; state = g_slist_next(state))
    {
        if(g_utf8_collate(moto_geometry_view_state_get_name((MotoGeometryViewState *)state->data), state_name) == 0)
        {
            MotoGeometryViewState *s = (MotoGeometryViewState *)state->data;
            if(s != self->priv->state)
            {
                 self->priv->state = s;
                 moto_geometry_view_node_set_prepared(self, FALSE);
            }
            return;
        }
    }

    GString *msg = g_string_new("instance \"");
    g_string_append(msg, moto_node_get_name((MotoNode *)self));
    g_string_append(msg, "\" of GeometryViewNode has no state with name \"");
    g_string_append(msg, state_name);
    g_string_append(msg, "\"");
    moto_warning(msg->str);
    g_string_free(msg, TRUE);
}

GSList *moto_geometry_view_node_get_state_list(MotoGeometryViewNode *self)
{
    return MOTO_GEOMETRY_VIEW_NODE_GET_CLASS(self)->states;
}

/* class GeometryViewState */

static GObjectClass *geometry_view_state_parent_class = NULL;

struct _MotoGeometryViewStatePriv
{
    GString *name;
    GString *title;
    MotoGeometryViewStateDrawFunc draw;
};

static void
moto_geometry_view_state_dispose(GObject *obj)
{
    MotoGeometryViewState *self = (MotoGeometryViewState *)obj;

    g_string_free(self->priv->name, TRUE);
    g_string_free(self->priv->title, TRUE);

    geometry_view_state_parent_class->dispose(obj);
}

static void
moto_geometry_view_state_finalize(GObject *obj)
{
    MotoGeometryViewState *self = (MotoGeometryViewState *)obj;

    g_slice_free(MotoGeometryViewStatePriv, self->priv);

    geometry_view_state_parent_class->finalize(obj);
}

static void
moto_geometry_view_state_init(MotoGeometryViewState *self)
{
    self->priv = g_slice_new(MotoGeometryViewStatePriv);

    self->priv->name    = g_string_new("");
    self->priv->title   = g_string_new("");

    self->priv->draw = NULL;
}

static void
moto_geometry_view_state_class_init(MotoGeometryViewStateClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    geometry_view_state_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    goclass->dispose    = moto_geometry_view_state_dispose;
    goclass->finalize   = moto_geometry_view_state_finalize;
}

G_DEFINE_TYPE(MotoGeometryViewState, moto_geometry_view_state, G_TYPE_OBJECT);

/* methods of class GeometryViewState */

MotoGeometryViewState *
moto_geometry_view_state_new(const gchar *name, const gchar *title,
        MotoGeometryViewStateDrawFunc draw)
{
    MotoGeometryViewState *self = (MotoGeometryViewState *)g_object_new(MOTO_TYPE_GEOMETRY_VIEW_STATE, NULL);

    g_string_assign(self->priv->name, name);
    g_string_assign(self->priv->title, title);

    self->priv->draw = draw;

    return self;
}

const gchar *moto_geometry_view_state_get_name(MotoGeometryViewState *self)
{
    return self->priv->name->str;
}

const gchar *moto_geometry_view_state_get_title(MotoGeometryViewState *self)
{
    return self->priv->title->str;
}

void moto_geometry_view_state_draw(MotoGeometryViewState *self, MotoGeometryViewNode *geom)
{
    if(self->priv->draw)
        self->priv->draw(self, geom);
}


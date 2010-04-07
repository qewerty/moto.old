#include "libmotoutil/moto-gl.h"
#include "libmotoutil/numdef.h"
#include "libmotoutil/xform.h"

#include "moto-messager.h"
#include "moto-geom-view-node.h"
#include "moto-mesh-view-node.h"

/* forwards */

static void moto_shape_view_node_grow_selection_virtual(MotoShapeViewNode *self);
static void moto_shape_view_node_select_less_virtual(MotoShapeViewNode *self);
static void moto_shape_view_node_invert_selection_virtual(MotoShapeViewNode *self);

/* class ShapeViewNode */

static GObjectClass *shape_view_node_parent_class = NULL;

struct _MotoShapeViewNodePriv
{
    gboolean disposed;

    gboolean prepared;
    MotoShapeViewState *state;
};

static void
moto_shape_view_node_dispose(GObject *obj)
{
    MotoShapeViewNode *self = (MotoShapeViewNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    shape_view_node_parent_class->dispose(obj);
}

static void
moto_shape_view_node_finalize(GObject *obj)
{
    MotoShapeViewNode *self = (MotoShapeViewNode *)obj;

    g_slice_free(MotoShapeViewNodePriv, self->priv);

    shape_view_node_parent_class->finalize(obj);
}

static void
moto_shape_view_node_init(MotoShapeViewNode *self)
{
    self->priv = g_slice_new(MotoShapeViewNodePriv);
    self->priv->disposed = FALSE;

    self->priv->prepared = FALSE;
    self->priv->state = NULL;
}

static void
moto_shape_view_node_class_init(MotoShapeViewNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    shape_view_node_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_shape_view_node_dispose;
    goclass->finalize   = moto_shape_view_node_finalize;

    klass->draw = NULL;
    klass->select = NULL;
    klass->prepare_for_draw = NULL;
    klass->get_shape = NULL;
    klass->grow_selection = moto_shape_view_node_grow_selection_virtual;
    klass->select_less    = moto_shape_view_node_select_less_virtual;
    klass->invert_selection = moto_shape_view_node_invert_selection_virtual;

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

G_DEFINE_ABSTRACT_TYPE(MotoShapeViewNode, moto_shape_view_node, MOTO_TYPE_NODE);

/* Methods of class ShapeViewNode */

void moto_shape_view_node_draw(MotoShapeViewNode *self)
{
    MotoShapeViewNodeClass *klass = MOTO_SHAPE_VIEW_NODE_GET_CLASS(self);

    g_signal_emit(self, klass->before_draw_signal_id, 0, NULL);

    if(klass->draw)
        klass->draw(self);

    g_signal_emit(self, klass->after_draw_signal_id, 0, NULL);
}

void moto_shape_view_node_prepare_for_draw(MotoShapeViewNode *self)
{
    MotoShapeViewNodeClass *klass = MOTO_SHAPE_VIEW_NODE_GET_CLASS(self);

    g_signal_emit(self, klass->before_prepare_for_draw_signal_id, 0, NULL);

    if(klass->prepare_for_draw)
        klass->prepare_for_draw(self);

    g_signal_emit(self, klass->after_prepare_for_draw_signal_id, 0, NULL);
}

gboolean moto_shape_view_node_select(MotoShapeViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoShapeViewNodeClass *klass = MOTO_SHAPE_VIEW_NODE_GET_CLASS(self);

    if(klass->select)
        return klass->select(self, x, y, width, height, ray, tinfo);
    return TRUE;
}

gboolean moto_shape_view_node_get_prepared(MotoShapeViewNode *self)
{
    return self->priv->prepared;
}

void moto_shape_view_node_set_prepared(MotoShapeViewNode *self, gboolean status)
{
    self->priv->prepared = status;
}

MotoShapeViewState *moto_shape_view_node_get_state(MotoShapeViewNode *self)
{
    return self->priv->state;
}

void moto_shape_view_node_set_state(MotoShapeViewNode *self, const gchar *state_name)
{
    GSList *state = MOTO_SHAPE_VIEW_NODE_GET_CLASS(self)->states;
    for(; state; state = g_slist_next(state))
    {
        if(g_utf8_collate(moto_shape_view_state_get_name((MotoShapeViewState *)state->data), state_name) == 0)
        {
            MotoShapeViewState *s = (MotoShapeViewState *)state->data;
            if(s != self->priv->state)
            {
                if(self->priv->state)
                    moto_shape_view_state_leave(self->priv->state, self);
                self->priv->state = s;
                moto_shape_view_node_set_prepared(self, FALSE);
            }
            return;
        }
    }

    moto_warning("Instance \"%s\" of ShapeViewNode has no state with name \"%s\"",
        moto_node_get_name((MotoNode *)self), state_name);
}

GSList *moto_shape_view_node_get_state_list(MotoShapeViewNode *self)
{
    return MOTO_SHAPE_VIEW_NODE_GET_CLASS(self)->states;
}

void moto_shape_view_node_goto_next_state(MotoShapeViewNode *self)
{
    GSList *li = moto_shape_view_node_get_state_list(self);
    if( ! li)
        return;

    GSList *s = g_slist_find(li, moto_shape_view_node_get_state(self));
    if(s)
    {
        s = g_slist_next(s);
        moto_shape_view_node_set_state(self,
            moto_shape_view_state_get_name((MotoShapeViewState *)((s) ? s->data : li->data)));
    }
}

MotoShapeNode *moto_shape_view_node_get_shape(MotoShapeViewNode *self)
{
    MotoShapeViewNodeClass *klass = MOTO_SHAPE_VIEW_NODE_GET_CLASS(self);

    if(klass->get_shape)
        return klass->get_shape(self);
    return NULL;
}

static void moto_shape_view_node_grow_selection_virtual(MotoShapeViewNode *self)
{
    MotoShapeViewState *state = moto_shape_view_node_get_state((MotoShapeViewNode *)self);
    if(state)
        moto_shape_view_state_grow_selection(state, self);
}

static void moto_shape_view_node_select_less_virtual(MotoShapeViewNode *self)
{
    MotoShapeViewState *state = moto_shape_view_node_get_state((MotoShapeViewNode *)self);
    if(state)
        moto_shape_view_state_select_less(state, self);
}

static void moto_shape_view_node_invert_selection_virtual(MotoShapeViewNode *self)
{
    MotoShapeViewState *state = moto_shape_view_node_get_state((MotoShapeViewNode *)self);
    if(state)
        moto_shape_view_state_invert_selection(state, self);
}

void moto_shape_view_node_grow_selection(MotoShapeViewNode *self)
{
    MotoShapeViewNodeClass *klass = MOTO_SHAPE_VIEW_NODE_GET_CLASS(self);

    if(klass->grow_selection)
        klass->grow_selection(self);

    moto_shape_view_node_set_prepared(self, FALSE);
}

void moto_shape_view_node_select_less(MotoShapeViewNode *self)
{
    MotoShapeViewNodeClass *klass = MOTO_SHAPE_VIEW_NODE_GET_CLASS(self);

    if(klass->select_less)
        klass->select_less(self);

    moto_shape_view_node_set_prepared(self, FALSE);
}

void moto_shape_view_node_invert_selection(MotoShapeViewNode *self)
{
    MotoShapeViewNodeClass *klass = MOTO_SHAPE_VIEW_NODE_GET_CLASS(self);

    if(klass->invert_selection)
        klass->invert_selection(self);

    moto_shape_view_node_set_prepared(self, FALSE);
}

gboolean moto_shape_view_node_process_button_press(MotoShapeViewNode *self,
    gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    /* ray is in object space.
     * tinfo contains all transformation info needed for projecting smth into window. */

    return moto_shape_view_node_select(self, x, y, width, height, ray, tinfo);
}

gboolean moto_shape_view_node_process_button_release(MotoShapeViewNode *self,
    gint x, gint y, gint width, gint height)
{
    return TRUE;
}

gboolean moto_shape_view_node_process_motion(MotoShapeViewNode *self,
    gint x, gint y, gint width, gint height)
{
    return TRUE;
}

/* class ShapeViewState */

static GObjectClass *shape_view_state_parent_class = NULL;

struct _MotoShapeViewStatePriv
{
    GString *name;
    GString *title;

    MotoShapeViewStateDrawFunc draw;
    MotoShapeViewStateSelectFunc select;
    MotoShapeViewStateGrowSelectionFunc grow_selection;
    MotoShapeViewStateSelectLessFunc select_less;
    MotoShapeViewStateInvertSelectionFunc invert_selection;
    MotoShapeViewStateLeaveFunc leave;
};

static void
moto_shape_view_state_dispose(GObject *obj)
{
    MotoShapeViewState *self = (MotoShapeViewState *)obj;

    g_string_free(self->priv->name, TRUE);
    g_string_free(self->priv->title, TRUE);

    shape_view_state_parent_class->dispose(obj);
}

static void
moto_shape_view_state_finalize(GObject *obj)
{
    MotoShapeViewState *self = (MotoShapeViewState *)obj;

    g_slice_free(MotoShapeViewStatePriv, self->priv);

    shape_view_state_parent_class->finalize(obj);
}

static void
moto_shape_view_state_init(MotoShapeViewState *self)
{
    self->priv = g_slice_new(MotoShapeViewStatePriv);

    self->priv->name    = g_string_new("");
    self->priv->title   = g_string_new("");

    self->priv->draw = NULL;
}

static void
moto_shape_view_state_class_init(MotoShapeViewStateClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    shape_view_state_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    goclass->dispose    = moto_shape_view_state_dispose;
    goclass->finalize   = moto_shape_view_state_finalize;
}

G_DEFINE_TYPE(MotoShapeViewState, moto_shape_view_state, G_TYPE_OBJECT);

/* Methods of class ShapeViewState */

MotoShapeViewState *
moto_shape_view_state_new(const gchar *name, const gchar *title,
        MotoShapeViewStateDrawFunc draw, MotoShapeViewStateSelectFunc select,
        MotoShapeViewStateGrowSelectionFunc grow_selection,
        MotoShapeViewStateSelectLessFunc select_less,
        MotoShapeViewStateInvertSelectionFunc invert_selection,
        MotoShapeViewStateLeaveFunc leave)
{
    MotoShapeViewState *self = (MotoShapeViewState *)g_object_new(MOTO_TYPE_SHAPE_VIEW_STATE, NULL);

    g_string_assign(self->priv->name, name);
    g_string_assign(self->priv->title, title);

    self->priv->draw             = draw;
    self->priv->select           = select;
    self->priv->grow_selection   = grow_selection;
    self->priv->select_less      = select_less;
    self->priv->invert_selection = invert_selection;
    self->priv->leave            = leave;

    return self;
}

const gchar *moto_shape_view_state_get_name(MotoShapeViewState *self)
{
    return self->priv->name->str;
}

const gchar *moto_shape_view_state_get_title(MotoShapeViewState *self)
{
    return self->priv->title->str;
}

void moto_shape_view_state_draw(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    if(self->priv->draw)
        self->priv->draw(self, geom);
}

gboolean moto_shape_view_state_select(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    if(self->priv->select)
        return self->priv->select(self, geom, x, y, width, height, ray, tinfo);
    return TRUE;
}

void moto_shape_view_state_grow_selection(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    if(self->priv->grow_selection)
        return self->priv->grow_selection(self, geom);
}

void moto_shape_view_state_select_less(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    if(self->priv->select_less)
        return self->priv->select_less(self, geom);
}

void moto_shape_view_state_invert_selection(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    if(self->priv->invert_selection)
        return self->priv->invert_selection(self, geom);
}

void moto_shape_view_state_leave(MotoShapeViewState *self, MotoShapeViewNode *geom)
{
    if(self->priv->leave)
        return self->priv->leave(self, geom);
}

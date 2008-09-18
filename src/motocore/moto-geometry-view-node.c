#include <GL/glew.h>
#include <GL/glu.h>

#include "moto-mesh-view-node.h"
#include "common/numdef.h"
#include "common/matrix.h"

#include "moto-messager.h"
#include "moto-geometry-view-node.h"

/* forwards */

static void moto_geom_view_node_grow_selection_virtual(MotoGeomViewNode *self);
static void moto_geom_view_node_select_less_virtual(MotoGeomViewNode *self);
static void moto_geom_view_node_invert_selection_virtual(MotoGeomViewNode *self);

/* enums */

GType moto_geom_view_draw_mode_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        GEnumValue values[] = {
            {MOTO_GEOM_VIEW_DRAW_MODE_BBOX,
                "GEOM_VIEW_DRAW_MODE_BBOX", "GEOM_VIEW_DRAW_MODE_BBOX"},
            {MOTO_GEOM_VIEW_DRAW_MODE_WIREFRAME,
                "GEOM_VIEW_DRAW_MODE_WIREFRAME", "GEOM_VIEW_DRAW_MODE_WIREFRAME"},
            {MOTO_GEOM_VIEW_DRAW_MODE_WIREFRAME_TEX,
                "GEOM_VIEW_DRAW_MODE_WIREFRAME_TEX", "GEOM_VIEW_DRAW_MODE_WIREFRAME_TEX"},
            {MOTO_GEOM_VIEW_DRAW_MODE_SOLID,
                "GEOM_VIEW_DRAW_MODE_SOLID", "GEOM_VIEW_DRAW_MODE_SOLID"},
            {MOTO_GEOM_VIEW_DRAW_MODE_SOLID_TEX,
                "GEOM_VIEW_DRAW_MODE_SOLID_TEX", "GEOM_VIEW_DRAW_MODE_SOLID_TEX"},
            {MOTO_GEOM_VIEW_DRAW_MODE_SMOOTH,
                "GEOM_VIEW_DRAW_MODE_SMOOTH", "GEOM_VIEW_DRAW_MODE_SMOOTH"},
            {MOTO_GEOM_VIEW_DRAW_MODE_SMOOTH_TEX,
                "GEOM_VIEW_DRAW_MODE_SMOOTH_TEX", "GEOM_VIEW_DRAW_MODE_SMOOTH_TEX"},
            {MOTO_GEOM_VIEW_DRAW_MODE_SHADED,
                "GEOM_VIEW_DRAW_MODE_SHADED", "GEOM_VIEW_DRAW_MODE_SHADED"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoGeomViewDrawMode", values);
    }
    return type;
}

/* class GeometryViewNode */

static GObjectClass *geometry_view_node_parent_class = NULL;

struct _MotoGeomViewNodePriv
{
    gboolean disposed;

    MotoGeomViewDrawMode draw_mode;

    gboolean prepared;
    MotoGeomViewState *state;
};

static void
moto_geom_view_node_dispose(GObject *obj)
{
    MotoGeomViewNode *self = (MotoGeomViewNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    geometry_view_node_parent_class->dispose(obj);
}

static void
moto_geom_view_node_finalize(GObject *obj)
{
    MotoGeomViewNode *self = (MotoGeomViewNode *)obj;

    g_slice_free(MotoGeomViewNodePriv, self->priv);

    geometry_view_node_parent_class->finalize(obj);
}

static void
moto_geom_view_node_init(MotoGeomViewNode *self)
{
    self->priv = g_slice_new(MotoGeomViewNodePriv);
    self->priv->disposed = FALSE;

    self->priv->draw_mode = MOTO_GEOM_VIEW_DRAW_MODE_SMOOTH;

    self->priv->prepared = FALSE;
    self->priv->state = NULL;
}

static void
moto_geom_view_node_class_init(MotoGeomViewNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    geometry_view_node_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_geom_view_node_dispose;
    goclass->finalize   = moto_geom_view_node_finalize;

    klass->draw = NULL;
    klass->select = NULL;
    klass->prepare_for_draw = NULL;
    klass->get_geometry = NULL;
    klass->grow_selection = moto_geom_view_node_grow_selection_virtual;
    klass->select_less    = moto_geom_view_node_select_less_virtual;
    klass->invert_selection = moto_geom_view_node_invert_selection_virtual;

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

G_DEFINE_ABSTRACT_TYPE(MotoGeomViewNode, moto_geom_view_node, MOTO_TYPE_NODE);

/* methods of class GeometryViewNode */

void moto_geom_view_node_draw(MotoGeomViewNode *self)
{
    MotoGeomViewNodeClass *klass = MOTO_GEOM_VIEW_NODE_GET_CLASS(self);

    g_signal_emit(self, klass->before_draw_signal_id, 0, NULL);

    if(klass->draw)
        klass->draw(self);

    g_signal_emit(self, klass->after_draw_signal_id, 0, NULL);
}

void moto_geom_view_node_prepare_for_draw(MotoGeomViewNode *self)
{
    MotoGeomViewNodeClass *klass = MOTO_GEOM_VIEW_NODE_GET_CLASS(self);

    g_signal_emit(self, klass->before_prepare_for_draw_signal_id, 0, NULL);

    if(klass->prepare_for_draw)
        klass->prepare_for_draw(self);

    g_signal_emit(self, klass->after_prepare_for_draw_signal_id, 0, NULL);
}

gboolean moto_geom_view_node_select(MotoGeomViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    MotoGeomViewNodeClass *klass = MOTO_GEOM_VIEW_NODE_GET_CLASS(self);

    if(klass->select)
        return klass->select(self, x, y, width, height, ray, tinfo);
    return TRUE;
}

gboolean moto_geom_view_node_get_prepared(MotoGeomViewNode *self)
{
    return self->priv->prepared;
}

void moto_geom_view_node_set_prepared(MotoGeomViewNode *self, gboolean status)
{
    self->priv->prepared = status;
}

MotoGeomViewState *moto_geom_view_node_get_state(MotoGeomViewNode *self)
{
    return self->priv->state;
}

void moto_geom_view_node_set_state(MotoGeomViewNode *self, const gchar *state_name)
{
    GSList *state = MOTO_GEOM_VIEW_NODE_GET_CLASS(self)->states;
    for(; state; state = g_slist_next(state))
    {
        if(g_utf8_collate(moto_geom_view_state_get_name((MotoGeomViewState *)state->data), state_name) == 0)
        {
            MotoGeomViewState *s = (MotoGeomViewState *)state->data;
            if(s != self->priv->state)
            {
                 self->priv->state = s;
                 moto_geom_view_node_set_prepared(self, FALSE);
            }
            return;
        }
    }

    GString *msg = g_string_new("instance \"");
    g_string_printf(msg, "instance \"%s\" of GeometryViewNode has no state with name \"%s\"",
            moto_node_get_name((MotoNode *)self), state_name);
    moto_warning(msg->str);
    g_string_free(msg, TRUE);
}

GSList *moto_geom_view_node_get_state_list(MotoGeomViewNode *self)
{
    return MOTO_GEOM_VIEW_NODE_GET_CLASS(self)->states;
}

void moto_geom_view_node_goto_next_state(MotoGeomViewNode *self)
{
    GSList *li = moto_geom_view_node_get_state_list(self);
    if( ! li)
        return;

    GSList *s = g_slist_find(li, moto_geom_view_node_get_state(self));
    if(s)
    {
        s = g_slist_next(s);
        moto_geom_view_node_set_state(self,
            moto_geom_view_state_get_name((MotoGeomViewState *)((s) ? s->data : li->data)));
    }
}

MotoGeometryNode *moto_geom_view_node_get_geometry(MotoGeomViewNode *self)
{
    MotoGeomViewNodeClass *klass = MOTO_GEOM_VIEW_NODE_GET_CLASS(self);

    if(klass->get_geometry)
        return klass->get_geometry(self);
    return NULL;
}

static void moto_geom_view_node_grow_selection_virtual(MotoGeomViewNode *self)
{
    MotoGeomViewState *state = moto_geom_view_node_get_state((MotoGeomViewNode *)self);
    if(state)
        moto_geom_view_state_grow_selection(state, self);
}

static void moto_geom_view_node_select_less_virtual(MotoGeomViewNode *self)
{
    MotoGeomViewState *state = moto_geom_view_node_get_state((MotoGeomViewNode *)self);
    if(state)
        moto_geom_view_state_select_less(state, self);
}

static void moto_geom_view_node_invert_selection_virtual(MotoGeomViewNode *self)
{
    MotoGeomViewState *state = moto_geom_view_node_get_state((MotoGeomViewNode *)self);
    if(state)
        moto_geom_view_state_invert_selection(state, self);
}

void moto_geom_view_node_grow_selection(MotoGeomViewNode *self)
{
    MotoGeomViewNodeClass *klass = MOTO_GEOM_VIEW_NODE_GET_CLASS(self);

    if(klass->grow_selection)
        klass->grow_selection(self);

    moto_geom_view_node_set_prepared(self, FALSE);
}

void moto_geom_view_node_select_less(MotoGeomViewNode *self)
{
    MotoGeomViewNodeClass *klass = MOTO_GEOM_VIEW_NODE_GET_CLASS(self);

    if(klass->select_less)
        klass->select_less(self);

    moto_geom_view_node_set_prepared(self, FALSE);
}

void moto_geom_view_node_invert_selection(MotoGeomViewNode *self)
{
    MotoGeomViewNodeClass *klass = MOTO_GEOM_VIEW_NODE_GET_CLASS(self);

    if(klass->invert_selection)
        klass->invert_selection(self);

    moto_geom_view_node_set_prepared(self, FALSE);
}

gboolean moto_geom_view_node_process_button_press(MotoGeomViewNode *self,
    gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    /* ray is in object space.
     * tinfo contains all transformation info needed for projecting smth into window. */

    return moto_geom_view_node_select(self, x, y, width, height, ray, tinfo);
}

gboolean moto_geom_view_node_process_button_release(MotoGeomViewNode *self,
    gint x, gint y, gint width, gint height)
{
    return TRUE;
}

gboolean moto_geom_view_node_process_motion(MotoGeomViewNode *self,
    gint x, gint y, gint width, gint height)
{
    return TRUE;
}

/* class GeometryViewState */

static GObjectClass *geometry_view_state_parent_class = NULL;

struct _MotoGeomViewStatePriv
{
    GString *name;
    GString *title;
    MotoGeomViewStateDrawFunc draw;
    MotoGeomViewStateSelectFunc select;
    MotoGeomViewStateGrowSelectionFunc grow_selection;
    MotoGeomViewStateSelectLessFunc select_less;
    MotoGeomViewStateInvertSelectionFunc invert_selection;
};

static void
moto_geom_view_state_dispose(GObject *obj)
{
    MotoGeomViewState *self = (MotoGeomViewState *)obj;

    g_string_free(self->priv->name, TRUE);
    g_string_free(self->priv->title, TRUE);

    geometry_view_state_parent_class->dispose(obj);
}

static void
moto_geom_view_state_finalize(GObject *obj)
{
    MotoGeomViewState *self = (MotoGeomViewState *)obj;

    g_slice_free(MotoGeomViewStatePriv, self->priv);

    geometry_view_state_parent_class->finalize(obj);
}

static void
moto_geom_view_state_init(MotoGeomViewState *self)
{
    self->priv = g_slice_new(MotoGeomViewStatePriv);

    self->priv->name    = g_string_new("");
    self->priv->title   = g_string_new("");

    self->priv->draw = NULL;
}

static void
moto_geom_view_state_class_init(MotoGeomViewStateClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    geometry_view_state_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    goclass->dispose    = moto_geom_view_state_dispose;
    goclass->finalize   = moto_geom_view_state_finalize;
}

G_DEFINE_TYPE(MotoGeomViewState, moto_geom_view_state, G_TYPE_OBJECT);

/* methods of class GeometryViewState */

MotoGeomViewState *
moto_geom_view_state_new(const gchar *name, const gchar *title,
        MotoGeomViewStateDrawFunc draw, MotoGeomViewStateSelectFunc select,
        MotoGeomViewStateGrowSelectionFunc grow_selection,
        MotoGeomViewStateSelectLessFunc select_less,
        MotoGeomViewStateInvertSelectionFunc invert_selection)
{
    MotoGeomViewState *self = (MotoGeomViewState *)g_object_new(MOTO_TYPE_GEOMETRY_VIEW_STATE, NULL);

    g_string_assign(self->priv->name, name);
    g_string_assign(self->priv->title, title);

    self->priv->draw = draw;
    self->priv->select = select;
    self->priv->grow_selection = grow_selection;
    self->priv->select_less = select_less;
    self->priv->invert_selection = invert_selection;

    return self;
}

const gchar *moto_geom_view_state_get_name(MotoGeomViewState *self)
{
    return self->priv->name->str;
}

const gchar *moto_geom_view_state_get_title(MotoGeomViewState *self)
{
    return self->priv->title->str;
}

void moto_geom_view_state_draw(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    if(self->priv->draw)
        self->priv->draw(self, geom);
}

gboolean moto_geom_view_state_select(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo)
{
    if(self->priv->select)
        return self->priv->select(self, geom, x, y, width, height, ray, tinfo);
    return TRUE;
}

void moto_geom_view_state_grow_selection(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    if(self->priv->grow_selection)
        return self->priv->grow_selection(self, geom);
}

void moto_geom_view_state_select_less(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    if(self->priv->select_less)
        return self->priv->select_less(self, geom);
}

void moto_geom_view_state_invert_selection(MotoGeomViewState *self, MotoGeomViewNode *geom)
{
    if(self->priv->invert_selection)
        return self->priv->invert_selection(self, geom);
}

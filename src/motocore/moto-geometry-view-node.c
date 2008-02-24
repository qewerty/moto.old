#include <GL/gl.h>

#include "moto-geometry-view-node.h"

/* class GeometryViewNode */

static GObjectClass *geometry_view_node_parent_class = NULL;

struct _MotoGeometryViewNodePriv
{
    gint dummy; /* TEMP */
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


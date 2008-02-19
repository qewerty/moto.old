
#include <GL/gl.h>

#include "moto-geometry-view-node.h"

/* class GeometryViewNode */

static GObjectClass *geometry_view_node_parent_class = NULL;

struct _MotoGeometryViewNodePriv
{
    gboolean prepared;
};

static void
moto_geometry_view_node_dispose(MotoGeometryViewNode *self)
{
    G_OBJECT_CLASS(geometry_view_node_parent_class)->dispose(G_OBJECT(self));
    g_slice_free(MotoGeometryViewNodePriv, self->priv);
}

static void
moto_geometry_view_node_finalize(MotoGeometryViewNode *self)
{
    G_OBJECT_CLASS(geometry_view_node_parent_class)->finalize(G_OBJECT(self));
}

static void
moto_geometry_view_node_init(MotoGeometryViewNode *self)
{
    self->priv = g_slice_new(MotoGeometryViewNodePriv);

    self->priv->prepared = FALSE;
}

static void
moto_geometry_view_node_class_init(MotoGeometryViewNodeClass *klass)
{
    geometry_view_node_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

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

G_DEFINE_ABSTRACT_TYPE(MotoGeometryViewNode, moto_geometry_view_node, G_TYPE_OBJECT);

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


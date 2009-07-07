#include <GL/glew.h>

#include "libmotoutil/numdef.h"
#include "libmotoutil/matrix.h"

#include "moto-messager.h"
#include "moto-world.h"
#include "moto-types.h"
#include "moto-time-node.h"

/* forwards */

static void _moto_time_node_update(MotoNode *self);

/* class TimeNode */

static GObjectClass *time_node_parent_class = NULL;

struct _MotoTimeNodePriv
{
    gboolean disposed;

    gfloat time;
    gfloat frame;
};

static void
moto_time_node_dispose(GObject *obj)
{
    MotoTimeNode *self = (MotoTimeNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;
}

static void
moto_time_node_finalize(GObject *obj)
{
    MotoTimeNode *self = (MotoTimeNode *)obj;

    g_slice_free(MotoTimeNodePriv, self->priv);

    time_node_parent_class->finalize(obj);
}

static void
moto_time_node_init(MotoTimeNode *self)
{
    self->priv = g_slice_new(MotoTimeNodePriv);
    self->priv->disposed = FALSE;

    self->priv->time    = 0.0;
    self->priv->frame   = 1.0;

    moto_node_add_params((MotoNode*)self,
        "time", "Time", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_OUT, 0.0, NULL, "Time",
        NULL);
}

static void
moto_time_node_class_init(MotoTimeNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    time_node_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_time_node_dispose;
    goclass->finalize   = moto_time_node_finalize;

    nclass->update      = _moto_time_node_update;
}

G_DEFINE_TYPE(MotoTimeNode, moto_time_node, MOTO_TYPE_NODE);

/* Methods of class TimeNode */

MotoTimeNode *moto_time_node_new(const gchar *name)
{
    MotoTimeNode *self = (MotoTimeNode *)g_object_new(MOTO_TYPE_TIME_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    moto_node_set_name(node, name);

    return self;
}

static void _moto_time_node_update(MotoNode *self)
{
    MotoWorld *world = moto_node_get_world(self);
    if( ! world)
    {
        /* TODO: Print smth? =) */
        return;
    }

    /*
    self->priv->time = moto_world_get_current_time(world);
    self->priv->frame = 1.0f + (gfloat)(moto_world_get_current_time(world) / moto_world_get_fps(world));
    */

    /* Chain up updates to all nodes which depend on this. */
    // moto_node_update_all_param_dests(self);
}

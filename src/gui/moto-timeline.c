#include "moto-timeline.h"

/* class MotoTimeline */

static GObjectClass *timeline_parent_class = NULL;

struct _MotoTimelinePriv
{
    gboolean disposed;

    MotoWorld *world;
};

static void
moto_timeline_dispose(GObject *obj)
{
    MotoTimeline *self = (MotoTimeline *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    timeline_parent_class->dispose(obj);
}

static void
moto_timeline_finalize(GObject *obj)
{
    g_slice_free(MotoTimelinePriv, ((MotoTimeline *)obj)->priv);
    timeline_parent_class->finalize(obj);
}

static void
moto_timeline_init(MotoTimeline *self)
{
    self->priv = g_slice_new(MotoTimelinePriv);
    self->priv->disposed = FALSE;

    gtk_widget_set_size_request((GtkWidget *)self, 100, 36);

    self->priv->world = NULL;
}

static void
moto_timeline_class_init(MotoTimelineClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    timeline_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_timeline_dispose;
    goclass->finalize   = moto_timeline_finalize;
}

G_DEFINE_TYPE(MotoTimeline, moto_timeline, GTK_TYPE_DRAWING_AREA);

GtkWidget *moto_timeline_new(MotoWorld *world)
{
    MotoTimeline *self = (MotoTimeline *)g_object_new(MOTO_TYPE_TIMELINE, NULL);

    self->priv->world = world;

    return (GtkWidget *)self;
}

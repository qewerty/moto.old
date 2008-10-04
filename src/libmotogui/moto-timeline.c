#include "moto-timeline.h"

/* callbacks */

gboolean on_expose_event(GtkWidget *widget,
                         GdkEventExpose *event,
                         gpointer data);

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

    gtk_widget_set_size_request((GtkWidget *)self, 100, 28);

    g_signal_connect(G_OBJECT(self), "expose-event",
                G_CALLBACK(on_expose_event), NULL);

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

gboolean on_expose_event(GtkWidget *widget,
                         GdkEventExpose *event,
                         gpointer data)
{
    GdkWindow *window = widget->window;
    GtkStyle *style = gtk_widget_get_style(widget);

    gint width, height;
    gdk_drawable_get_size(window, & width, & height);

    GdkRectangle rect = {0, 0, width, height};

    gdk_window_begin_paint_rect(window, & rect);

    cairo_t *ctx = gdk_cairo_create(window);
    cairo_set_antialias(ctx, CAIRO_ANTIALIAS_NONE);
    cairo_set_line_width(ctx, 1);

    // background
    cairo_rectangle(ctx, 0, 0, width, height);
    cairo_set_source_rgb(ctx,
            style->mid[0].red/255./255.,
            style->mid[0].green/255./255.,
            style->mid[0].blue/255./255.);
    cairo_fill(ctx);

    // line
    cairo_move_to(ctx, 0, height/(gfloat)2);
    cairo_line_to(ctx, width, height/(gfloat)2);
    cairo_set_source_rgb(ctx,
            style->dark[0].red/255./255.,
            style->dark[0].green/255./255.,
            style->dark[0].blue/255./255.);
    cairo_stroke(ctx);

    gdk_window_end_paint(window);

    cairo_destroy(ctx);

    return FALSE;
}

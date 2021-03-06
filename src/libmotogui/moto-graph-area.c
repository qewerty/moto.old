#include <stdlib.h>

#include <gtk/gtkgl.h>

#include "libmotoutil/moto-gl.h"
#include "libmoto/moto-scene-node.h"
#include "moto-graph-area.h"

/* Misc */

typedef struct _MotoGraphAreaNodeInfo MotoGraphAreaNodeInfo;

static void
moto_graph_area_on_node_created(MotoSystem *system, MotoNode *node, MotoGraphArea *area);

static void
moto_graph_area_on_node_deleted(MotoGraphAreaNodeInfo *info, MotoNode *node);

static gboolean
moto_graph_area_on_expose_event(GtkWidget *widget,
                                GdkEventExpose *event,
                                gpointer data);

static void
moto_graph_area_draw(MotoGraphArea *self, cairo_t* cairo, gint width, gint height);

/* class MotoGraphAreaNodeView */

typedef struct _MotoGraphAreaNodeView MotoGraphAreaNodeView;
typedef struct _MotoGraphAreaNodeViewClass MotoGraphAreaNodeViewClass;

static GObjectClass *graph_area_node_view_parent_class = NULL;

struct _MotoGraphAreaNodeView
{
    GObject parent;

    MotoNode *node;
    gint x, y, width, height;
};

struct _MotoGraphAreaNodeViewClass
{
    GObjectClass parent;
};

GType moto_graph_node_view_area_get_type(void);

#define MOTO_TYPE_GRAPH_AREA_NODE_VIEW (moto_graph_area_node_view_get_type())
#define MOTO_GRAPH_AREA_NODE_VIEW(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GRAPH_AREA_NODE_VIEW, MotoGraphAreaNodeView))
#define MOTO_GRAPH_AREA_NODE_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GRAPH_AREA_NODE_VIEW, MotoGraphAreaNodeViewClass))
#define MOTO_IS_GRAPH_AREA_NODE_VIEW(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GRAPH_AREA_NODE_VIEW))
#define MOTO_IS_GRAPH_AREA_NODE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GRAPH_AREA_NODE_VIEW))
#define MOTO_GRAPH_AREA_NODE_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GRAPH_AREA_NODE_VIEW, MotoGraphAreaNodeViewClass))

static void
moto_graph_area_node_view_dispose(GObject *obj)
{
    graph_area_node_view_parent_class->dispose(obj);
}

static void
moto_graph_area_node_view_finalize(GObject *obj)
{
    graph_area_node_view_parent_class->finalize(obj);
}

static void
moto_graph_area_node_view_init(MotoGraphAreaNodeView *self)
{
    self->node = NULL;

    self->x = self->y = 0;
    self->width  = 60;
    self->height = 30;
}

static void
moto_graph_area_node_view_class_init(MotoGraphAreaNodeViewClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    graph_area_node_view_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_graph_area_node_view_dispose;
    goclass->finalize   = moto_graph_area_node_view_finalize;
}

G_DEFINE_TYPE(MotoGraphAreaNodeView, moto_graph_area_node_view, G_TYPE_OBJECT);

GtkWidget *moto_graph_area_node_view_new(MotoNode *node)
{
    MotoGraphAreaNodeView *self = \
        (MotoGraphAreaNodeView *)g_object_new(MOTO_TYPE_GRAPH_AREA_NODE_VIEW, NULL);

    self->node = node;

    return (GtkWidget *)self;
}

static void
moto_graph_area_node_view_draw(MotoGraphAreaNodeView *self, cairo_t* cairo)
{
    cairo_set_antialias(cairo, CAIRO_ANTIALIAS_NONE);
    cairo_set_line_width(cairo, 1);

    cairo_rectangle(cairo, self->x, self->y, self->width, self->height);
    cairo_set_source_rgb(cairo, 0.6, 0.2, 0.1);
    cairo_fill(cairo);

    cairo_rectangle(cairo, self->x, self->y, self->width, self->height);
    cairo_set_source_rgb(cairo, 0.2, 0.2, 0.2);
    cairo_stroke(cairo);

    cairo_save(cairo);
    cairo_translate(cairo, self->x, self->y - 2);
    cairo_show_text(cairo, moto_node_get_name(self->node));
    cairo_restore(cairo);
}

/* class MotoGraphArea */

#define MOTO_GRAPH_AREA_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_GRAPH_AREA, MotoGraphAreaPriv)

static GObjectClass *graph_area_parent_class = NULL;

struct _MotoGraphAreaPriv
{
    gboolean disposed;

    MotoSystem *system;
    gulong node_created_handler_id;

    GList *node_views;
};

static void
moto_graph_area_dispose(GObject *obj)
{
    MotoGraphAreaPriv *priv = MOTO_GRAPH_AREA_GET_PRIVATE(obj);

    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    if(priv->system)
    {
        g_object_remove_weak_pointer(G_OBJECT(priv->system), (gpointer *)( & priv->system));
        g_signal_handler_disconnect(priv->system, priv->node_created_handler_id);
    }

    graph_area_parent_class->dispose(obj);
}

static void
moto_graph_area_finalize(GObject *obj)
{
    graph_area_parent_class->finalize(obj);
}

static void
moto_graph_area_init(MotoGraphArea *self)
{
    MotoGraphAreaPriv *priv = MOTO_GRAPH_AREA_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->node_views = NULL;

    GdkGLConfig *gl_config = \
        gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE));
    if( ! gl_config)
    {
        g_print("\nMoto: Error - Unable to initialize OpenGL!\n");
        exit(1);
    }
    gtk_widget_set_gl_capability((GtkWidget*)self, gl_config,
        NULL, TRUE, GDK_GL_RGBA_TYPE);

    gtk_widget_add_events(GTK_WIDGET(self), GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK |
            GDK_BUTTON_PRESS_MASK | GDK_VISIBILITY_NOTIFY_MASK | GDK_BUTTON_RELEASE_MASK |
            GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);

    g_signal_connect(G_OBJECT(self), "expose-event",
        G_CALLBACK(moto_graph_area_on_expose_event), NULL);
}

static void
moto_graph_area_class_init(MotoGraphAreaClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    graph_area_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_graph_area_dispose;
    goclass->finalize   = moto_graph_area_finalize;

    g_type_class_add_private(goclass, sizeof(MotoGraphAreaPriv));
}

G_DEFINE_TYPE(MotoGraphArea, moto_graph_area, GTK_TYPE_DRAWING_AREA);

static gboolean
add_node_view(MotoSceneNode *scene_node, MotoNode *node, MotoGraphArea *area)
{
    MotoGraphAreaPriv *priv = MOTO_GRAPH_AREA_GET_PRIVATE(area);

    MotoGraphAreaNodeView *nv = \
        (MotoGraphAreaNodeView *)moto_graph_area_node_view_new(node);

    nv->x = g_random_int_range(10, 300);
    nv->y = g_random_int_range(10, 300);

    priv->node_views = g_list_prepend(priv->node_views, nv);

    return TRUE;
}

GtkWidget *moto_graph_area_new(MotoSystem *system)
{
    if(!system)
        return NULL;

    MotoSceneNode *scene_node = moto_system_get_current_scene(system);
    if( ! scene_node)
        return NULL;

    MotoGraphArea *self = (MotoGraphArea *)g_object_new(MOTO_TYPE_GRAPH_AREA, NULL);
    MotoGraphAreaPriv *priv = MOTO_GRAPH_AREA_GET_PRIVATE(self);

    /*
    moto_scene_node_foreach_node(scene_node, MOTO_TYPE_NODE,
        (MotoSceneNodeForeachNodeFunc)add_node_view, self);
        */

    const GList* child = moto_node_get_children((MotoNode*)scene_node);
    for(; child; child = g_list_next(child))
    {
        MotoNode* node = (MotoNode*)child->data;
        add_node_view(scene_node, node, self);
    }

    g_object_add_weak_pointer(G_OBJECT(system), (gpointer *)( & priv->system));

    priv->node_created_handler_id = \
        g_signal_connect(G_OBJECT(system), "node-created",
            G_CALLBACK(moto_graph_area_on_node_created), self);

    return (GtkWidget *)self;
}

struct _MotoGraphAreaNodeInfo
{
    MotoGraphArea *area;
    MotoGraphAreaNodeView *node_view;
    GList *list;
};

static void
moto_graph_area_on_node_created(MotoSystem *system, MotoNode *node, MotoGraphArea *area)
{
    MotoGraphAreaPriv *priv = MOTO_GRAPH_AREA_GET_PRIVATE(area);

    MotoGraphAreaNodeView *node_view = (MotoGraphAreaNodeView *)moto_graph_area_node_view_new(node);
    priv->node_views = g_list_prepend(priv->node_views, node_view);

    MotoGraphAreaNodeInfo *info = g_slice_new(MotoGraphAreaNodeInfo);
    info->area      = area;
    info->node_view = node_view;
    info->list      = priv->node_views;
    g_object_weak_ref(G_OBJECT(node_view),
        (GWeakNotify)moto_graph_area_on_node_deleted, info);
}

static void
moto_graph_area_on_node_deleted(MotoGraphAreaNodeInfo *info, MotoNode *node)
{
    MotoGraphAreaPriv *priv = MOTO_GRAPH_AREA_GET_PRIVATE(info->area);

    g_object_unref(info->node_view);
    priv->node_views = g_list_delete_link(priv->node_views, info->list);

    g_slice_free(MotoGraphAreaNodeInfo, info);
}

static gboolean
moto_graph_area_on_expose_event(GtkWidget *widget,
                                GdkEventExpose *event,
                                gpointer data)
{
    if(!GTK_WIDGET_REALIZED(widget))
        return FALSE;

    GdkWindow *window = widget->window;
    GtkStyle *style = gtk_widget_get_style(widget);

    gint width, height;
    gdk_drawable_get_size(window, &width, &height);

    GdkRectangle rect = {0, 0, width, height};

    gdk_window_begin_paint_rect(window, &rect);

    cairo_t *cairo = gdk_cairo_create(window);

    // Fill background.
    cairo_rectangle(cairo, 0, 0, width, height);
    cairo_set_source_rgb(cairo,
            style->mid[0].red/255./255.,
            style->mid[0].green/255./255.,
            style->mid[0].blue/255./255.);
    cairo_fill(cairo);

    moto_graph_area_draw((MotoGraphArea*)widget, cairo, width, height);

    gdk_window_end_paint(window);

    return TRUE;
}

static void
moto_graph_area_draw(MotoGraphArea *self, cairo_t* cairo, gint width, gint height)
{
    MotoGraphAreaPriv *priv = MOTO_GRAPH_AREA_GET_PRIVATE(self);

    GList *nv = g_list_last(priv->node_views);
    for(; nv; nv = g_list_previous(nv))
    {
        moto_graph_area_node_view_draw((MotoGraphAreaNodeView *)nv->data, cairo);
    }
}

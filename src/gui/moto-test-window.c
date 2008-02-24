#include "stdlib.h"

#include <gtk/gtkgl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "moto-test-window.h"
#include "motocore/moto-world.h"
#include "motocore/moto-system.h"
#include "motocore/moto-node.h"

/* forwards */

static void init_gl(GtkWidget *widget, gpointer data);
static gboolean draw(GtkWidget *widget, GdkEventExpose *event, gpointer data);

static gboolean
reshape(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

/* class TestWindow */

static GObjectClass *test_window_parent_class = NULL;

struct _MotoTestWindowPriv
{
    MotoSystem *system;
    MotoWorld *world;

    GtkDrawingArea *area;
};

static void
moto_test_window_dispose(GObject *obj)
{
    MotoTestWindow *self = (MotoTestWindow *)obj;

    g_object_unref(self->priv->system);
    g_slice_free(MotoTestWindowPriv, self->priv);

    G_OBJECT_CLASS(test_window_parent_class)->dispose(obj);
}

static void
moto_test_window_finalize(GObject *obj)
{
    test_window_parent_class->finalize(obj);
}

static void
moto_test_window_init(MotoTestWindow *self)
{
    self->priv = g_slice_new(MotoTestWindowPriv);

    self->priv->system = moto_system_new();
    moto_system_get_library(self->priv->system);
    self->priv->world = moto_world_new("My Test World", moto_system_get_library(self->priv->system));
    moto_system_add_world(self->priv->system, self->priv->world, TRUE);

    MotoNode *obj_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "Object");
    moto_world_set_root(self->priv->world, (MotoObjectNode *)obj_node);
    MotoNode *view_node = moto_world_create_node(self->priv->world, "MotoMeshViewNode", "View");
    MotoNode *cube_node = moto_world_create_node(self->priv->world, "MotoCubeNode", "Cube");

    MotoParam *in = moto_node_get_param(obj_node, "view", "view");
    MotoParam *out = moto_node_get_param(view_node, "main", "view");
    moto_param_set_source(in, out);

    in = moto_node_get_param(view_node, "main", "mesh");
    out = moto_node_get_param(cube_node, "main", "mesh");
    moto_param_set_source(in, out);

    self->priv->area = (GtkDrawingArea *)gtk_drawing_area_new();
    GtkWidget *area = (GtkWidget *)self->priv->area;
    GdkGLConfig *gl_config =\
                gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
        if (gl_config == NULL) {
                g_print("\nNUll: Error - Unable to initialize OpenGL!\n");
                exit(1);
        }
    gtk_widget_set_gl_capability(area, gl_config,
                NULL, TRUE, GDK_GL_RGBA_TYPE);

    gtk_container_add(GTK_CONTAINER(self), area);

    g_signal_connect(G_OBJECT(self), "delete-event",
                G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(G_OBJECT(area), "expose-event",
                G_CALLBACK(draw), NULL);
    g_signal_connect(G_OBJECT(area), "realize",
                G_CALLBACK(init_gl), NULL);
    g_signal_connect(G_OBJECT(area), "configure-event",
                G_CALLBACK(reshape), NULL);
}

static void
moto_test_window_class_init(MotoTestWindowClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);

    test_window_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_test_window_dispose;
    goclass->finalize   = moto_test_window_finalize;
}

G_DEFINE_TYPE(MotoTestWindow, moto_test_window, GTK_TYPE_WINDOW);

/* methods of class TestWindow */

GtkWindow *moto_test_window_new()
{
    GtkWindow *self = (GtkWindow *)g_object_new(MOTO_TYPE_TEST_WINDOW, NULL);
    MotoTestWindow *twin = (MotoTestWindow *)self;

    // MotoWorld *w = twin->priv->world;

    return self;
}

static void init_gl(GtkWidget *widget, gpointer data)
{
    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);

    if(!GDK_IS_GL_DRAWABLE(gl_drawable)) return;
    if(!gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return;

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2, 0.2, 0.2, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, width/(float)height, 0.3, 150.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluLookAt(1.5, 2.0, 2.5, 0, 0, 0, 0, 0, 1);

    gdk_gl_drawable_gl_end(gl_drawable);
}

static gboolean
draw(GtkWidget *widget,
     GdkEventExpose *event,
     gpointer data)
{
    if(!GTK_WIDGET_REALIZED(widget)) return FALSE;

    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);
    if(!GDK_IS_GL_DRAWABLE(gl_drawable)) return FALSE;
    if(!gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return FALSE;

    MotoTestWindow *twin = (MotoTestWindow *)gtk_widget_get_parent(widget);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1, 0, 0);

    moto_world_draw(twin->priv->world);

    gdk_gl_drawable_swap_buffers(gl_drawable);

    return TRUE;
}

static gboolean
reshape(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);

    if( ! GDK_IS_GL_DRAWABLE(gl_drawable)) return FALSE;
    if( ! gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return FALSE;

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    glClearColor(0.2, 0.2, 0.2, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, width/(float)height, 0.3, 150.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluLookAt(1.5, 2.0, 2.5, 0, 0, 0, 0, 0, 1);

    gdk_gl_drawable_gl_end(gl_drawable);

    return FALSE;
}

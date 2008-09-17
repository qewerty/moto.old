#include "stdlib.h"
#include "math.h"

#include <gtk/gtkgl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "moto-test-window.h"
#include "moto-main-menu.h"
#include "moto-tool-box.h"
#include "moto-timeline.h"
#include "moto-message-bar.h"
#include "moto-param-editor.h"
#include "moto-outliner.h"

#include "motocore/moto-world.h"
#include "motocore/moto-system.h"
#include "motocore/moto-node.h"
#include "motocore/moto-object-node.h"
#include "common/numdef.h"

#include "motocore/moto-geometry-view-node.h"

#include "moto-shelf.h"

static MotoTestWindow *twin = NULL;

/* forwards */

static void init_gl(GtkWidget *widget, gpointer data);
static gboolean draw(GtkWidget *widget, GdkEventExpose *event, gpointer data);

static gboolean
reshape(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

static gboolean
press_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean
release_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean
mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data);

gboolean on_key_press_event(GtkWidget   *widget,
                            GdkEventKey *event,
                            gpointer     user_data);

/* class TestWindow */

static GObjectClass *test_window_parent_class = NULL;

struct _MotoTestWindowPriv
{
    MotoSystem *system;
    MotoWorld *world;

    MotoGeomViewNode *gv;

    MotoParamEditor *param_editor;

    GtkDrawingArea *area;

    MotoNode *vtest;
    MotoVariation *v1, *v2;

    gboolean disposed;
};

static void
moto_test_window_dispose(GObject *obj)
{
    MotoTestWindow *self = (MotoTestWindow *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_object_unref(self->priv->system);

    test_window_parent_class->dispose(obj);
}

static void
moto_test_window_finalize(GObject *obj)
{
    MotoTestWindow *self = (MotoTestWindow *)obj;
    g_slice_free(MotoTestWindowPriv, self->priv);

    test_window_parent_class->finalize(obj);
}

static void quit(MotoTestWindow *self)
{
    // g_object_unref(self->priv->world);
    gtk_main_quit();
}

static MotoNode *cube = NULL;

gboolean on_key_press_event(GtkWidget   *widget,
                            GdkEventKey *event,
                            gpointer     user_data)
{
    MotoTestWindow *self = (MotoTestWindow *)widget;

    if(0 == g_utf8_collate(event->string, "+") || \
       0 == g_utf8_collate(event->string, "="))
    {
        MotoWorld *w = moto_system_get_current_world(self->priv->system);
        if( ! w)
            return FALSE;
        MotoObjectNode *ob = moto_world_get_current_object(w);
        if( ! ob)
            return FALSE;
        MotoGeomViewNode *gv = moto_node_get_param_object((MotoNode *)ob, "view");
        if( ! gv)
            return FALSE;

        moto_geom_view_node_grow_selection(gv);
        draw(self->priv->area, (GdkEventExpose *)event, user_data);
    }
    else if(0 == g_utf8_collate(event->string, "-"))
    {
        MotoWorld *w = moto_system_get_current_world(self->priv->system);
        if( ! w)
            return FALSE;
        MotoObjectNode *ob = moto_world_get_current_object(w);
        if( ! ob)
            return FALSE;
        MotoGeomViewNode *gv = moto_node_get_param_object((MotoNode *)ob, "view");
        if( ! gv)
            return FALSE;
        moto_geom_view_node_select_less(gv);
        draw(self->priv->area, (GdkEventExpose *)event, user_data);
    }
    else if(0 == g_utf8_collate(event->string, "i"))
    {
        MotoWorld *w = moto_system_get_current_world(self->priv->system);
        if( ! w)
            return FALSE;
        MotoObjectNode *ob = moto_world_get_current_object(w);
        if( ! ob)
            return FALSE;
        MotoGeomViewNode *gv = moto_node_get_param_object((MotoNode *)ob, "view");
        if( ! gv)
            return FALSE;
        moto_geom_view_node_invert_selection(gv);
        draw(self->priv->area, (GdkEventExpose *)event, user_data);
    }
    else if(0 == g_utf8_collate(event->string, "z"))
    {
        // FIXME: TEMP
        MotoWorld *w = moto_system_get_current_world(self->priv->system);
        if( ! w)
            return FALSE;
        MotoObjectNode *ob = moto_world_get_current_object(w);
        if( ! ob)
            return FALSE;
        MotoGeomViewNode *gv = moto_node_get_param_object((MotoNode *)ob, "view");
        if( ! gv)
            return FALSE;
        MotoParam *p = moto_node_get_param((MotoNode *)gv, "mesh");
        if( ! p)
            return FALSE;
        MotoParam *s = moto_param_get_source(p);
        if( ! s)
            return FALSE;
        MotoNode *node = moto_param_get_node(s);

        MotoParam *dx = moto_node_get_param(node, "div_x");
        if( ! dx)
            return FALSE;

        moto_param_set_uint(dx, moto_param_get_uint(dx) + 1);
        moto_node_update(node);

        moto_test_window_redraw_3dview(self);
    }
    else if(65289 == event->keyval)
    {
        MotoWorld *w = moto_system_get_current_world(self->priv->system);
        if( ! w)
            return FALSE;
        MotoObjectNode *ob = moto_world_get_current_object(w);
        if( ! ob)
            return FALSE;
        MotoGeomViewNode *gv = moto_node_get_param_object((MotoNode *)ob, "view");
        if( ! gv)
            return FALSE;
        moto_geom_view_node_goto_next_state(gv);
        moto_test_window_redraw_3dview(self);
    }

    return FALSE;
}

gboolean redraw_world_idle(gpointer data)
{
    MotoTestWindow *tw = (MotoTestWindow *)data;
    MotoSystem *system = tw->priv->system;

    if( ! system)
        return FALSE;

    MotoWorld *world = moto_system_get_current_world(system);
    if( ! world)
        return FALSE;

    MotoNode *node = (MotoNode *)moto_world_get_current_object(world);
    if( ! node)
        return FALSE;

    /*
    moto_object_node_set_translate((MotoObjectNode *)node,
            moto_node_get_param_float(node, "tx"),
            moto_node_get_param_float(node, "ty") + 0.02,
            moto_node_get_param_float(node, "tz"));
            */

    moto_test_window_redraw_3dview(tw);

    return FALSE;
}

static gpointer anim_thread(MotoTestWindow *window)
{
    MotoSystem *system = window->priv->system;

    if( ! system)
        return FALSE;

    MotoWorld *world = moto_system_get_current_world(system);
    if( ! world)
        return FALSE;

    while(TRUE)
    {

        MotoNode *node = (MotoNode *)moto_world_get_current_object(world);
        if( ! node)
        {
            g_usleep(100000);
            continue;
        }

        moto_object_node_set_translate((MotoObjectNode *)node,
            moto_node_get_param_float(node, "tx"),
            moto_node_get_param_float(node, "ty") + 0.02,
            moto_node_get_param_float(node, "tz"));

        g_idle_add(redraw_world_idle, window);

        g_usleep(100000);
    }
}

static void
moto_test_window_init(MotoTestWindow *self)
{
    self->priv = g_slice_new(MotoTestWindowPriv);
    self->priv->disposed = FALSE;

    gtk_window_maximize((GtkWindow *)self);

    // Test scene

    self->priv->v1 = moto_variation_new("v1");
    self->priv->v2 = moto_variation_new("v2");
    moto_variation_set_parent(self->priv->v2, self->priv->v1);

    MotoParam *param;

    self->priv->system = moto_system_new();
    moto_system_get_library(self->priv->system);
    self->priv->world = moto_world_new("My Test World", moto_system_get_library(self->priv->system));
    moto_system_add_world(self->priv->system, self->priv->world, TRUE);

    MotoNode *root_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "Root", NULL);
    moto_world_set_root(self->priv->world, (MotoObjectNode *)root_node);

    param = moto_node_get_param(root_node, "ty");

    MotoNode *obj_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "Object", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    MotoNode *grid_view_node = moto_world_create_node(self->priv->world, "MotoGridViewNode", "GridView", NULL);
    moto_node_link(obj_node, "view", grid_view_node, "view");

    obj_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "Object", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");

    moto_world_set_axes(self->priv->world, (MotoObjectNode *)obj_node);

    MotoNode *axes_view_node = moto_world_create_node(self->priv->world, "MotoAxesViewNode", "AxesView", NULL);
    moto_node_link(obj_node, "view", axes_view_node, "view");

    // cube instance
    obj_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "CubeObject2", NULL);
    moto_node_link(obj_node, "parent", root_node, "transform");
    // moto_node_link(obj_node, "view", view_node, "view");

    self->priv->vtest = obj_node;
    moto_node_save_to_variation(obj_node, self->priv->v1);

    moto_node_set_params(obj_node,
            "tx", 2.5f,
            "ty", 3.1f,
            "tz", -1.5f,
            "rx", 45.f,
            "ry", 30.f,
            "rz", 15.f);

    moto_node_save_to_variation(obj_node, self->priv->v2);

    moto_node_restore_from_variation(obj_node, self->priv->v1);

    // gfloat tx, ty, tz;
    // moto_node_get_params(obj_node, "tx", &tx, "ty", &ty, "tz", &tz, NULL);
    // g_print("tx, ty, tz: %f %f %f\n", tx, ty, tz);

    moto_node_restore_from_variation(obj_node, self->priv->v2);

    // moto_node_get_params(obj_node, "tx", &tx, "ty", &ty, "tz", &tz, NULL);
    // g_print("tx, ty, tz: %f %f %f\n", tx, ty, tz);

    // camera 
    MotoNode *cam_obj = moto_world_create_node(self->priv->world, "MotoObjectNode", "CameraObject", NULL);
    // MotoNode *cam = moto_world_create_node(self->priv->world, "MotoCameraNode", "CameraObject");
    moto_world_set_camera(self->priv->world, (MotoObjectNode *)cam_obj);

    // Window
    self->priv->area = (GtkDrawingArea *)gtk_drawing_area_new();
    gtk_widget_set_size_request((GtkWidget *)self->priv->area, 360, 240);
    GtkWidget *area = (GtkWidget *)self->priv->area;
    GdkGLConfig *gl_config =\
                gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
    if( ! gl_config)
    {
            g_print("\nNUll: Error - Unable to initialize OpenGL!\n");
            exit(1);
    }
    gtk_widget_set_gl_capability(area, gl_config,
                NULL, TRUE, GDK_GL_RGBA_TYPE);

    // Widgets
    gtk_widget_add_events(GTK_WIDGET(area), GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK |
            GDK_BUTTON_PRESS_MASK | GDK_VISIBILITY_NOTIFY_MASK | GDK_BUTTON_RELEASE_MASK |
            GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);

    GtkBox *hbox = (GtkBox *)gtk_hbox_new(FALSE, 1);

    gtk_box_pack_start(hbox, moto_tool_box_new(self->priv->system), FALSE, FALSE, 0);
    GtkPaned *hp = (GtkPaned *)gtk_hpaned_new();
    gtk_box_pack_start(hbox, (GtkWidget *)hp, TRUE, TRUE, 0);
    gtk_paned_pack1(hp, (GtkWidget *)area, TRUE, FALSE);
    self->priv->param_editor = moto_param_editor_new(self);
    GtkScrolledWindow *sw = (GtkScrolledWindow *)gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request((GtkWidget *)sw, 320, 36);
    gtk_scrolled_window_set_policy(sw, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_add_with_viewport(sw, (GtkWidget *)self->priv->param_editor);
    GtkPaned *vp = (GtkPaned *)gtk_vpaned_new();
    gtk_paned_pack1(vp, (GtkWidget *)sw, FALSE, FALSE);
    gtk_paned_pack2(vp, (GtkWidget *)moto_outliner_new(self->priv->world), FALSE, FALSE);
    gtk_paned_pack2(hp, (GtkWidget *)vp, FALSE, FALSE);
    // gtk_box_pack_start(hbox, self->priv->param_editor, FALSE, FALSE, 0);

    GtkBox *vbox = (GtkBox *)gtk_vbox_new(FALSE, 1);

    gtk_box_pack_start(vbox, moto_main_menu_new(), FALSE, FALSE, 0);
    gtk_box_pack_start(vbox, moto_shelf_new(self->priv->system, (GtkWindow *)self), FALSE, FALSE, 0);
    gtk_box_pack_start(vbox, (GtkWidget *)hbox, TRUE, TRUE, 0);
    gtk_box_pack_start(vbox, moto_timeline_new(self->priv->world), FALSE, FALSE, 0);
    gtk_box_pack_start(vbox, moto_message_bar_new(moto_messager_singleton()), FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(self), (GtkWidget *)vbox);

    g_signal_connect(G_OBJECT(self), "delete-event",
                G_CALLBACK(quit), NULL);
    g_signal_connect(G_OBJECT(self), "destroy",
                G_CALLBACK(quit), NULL);
    g_signal_connect(G_OBJECT(self), "key-press-event",
                G_CALLBACK(on_key_press_event), NULL);

    g_signal_connect(G_OBJECT(area), "expose-event",
                G_CALLBACK(draw), NULL);
    g_signal_connect(G_OBJECT(area), "realize",
                G_CALLBACK(init_gl), NULL);
    g_signal_connect(G_OBJECT(area), "configure-event",
                G_CALLBACK(reshape), NULL);

    g_signal_connect(G_OBJECT(area), "motion-notify-event",
            G_CALLBACK(mouse_motion), NULL);

    g_signal_connect(G_OBJECT(area), "button-press-event",
        G_CALLBACK(press_mouse_button), NULL);
    g_signal_connect(G_OBJECT(area), "button-release-event",
        G_CALLBACK(release_mouse_button), NULL);


    // g_object_unref(G_OBJECT(grid_view_node));

    gtk_widget_grab_focus(GTK_WIDGET(area));

    gtk_window_set_title((GtkWindow *)self, "Moto v0.0");
    gtk_widget_set_size_request((GtkWidget *)self, 800, 480);

    // g_thread_create((GThreadFunc)anim_thread, self, TRUE, NULL);
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
    // MotoTestWindow *twin = (MotoTestWindow *)self;

    // MotoWorld *w = twin->priv->world;
    //

    twin = (MotoTestWindow *)self;

    return self;
}

void moto_test_window_redraw_3dview(MotoTestWindow *self)
{
    GdkEvent *event = gdk_event_new(GDK_NOTHING);
    g_signal_emit_by_name(self->priv->area, "expose-event",  self->priv->area, event, NULL);
    gdk_event_free(event);
}

void moto_test_window_update_param_editor(MotoTestWindow *self)
{
    MotoNode *obj = (MotoNode *)moto_world_get_current_object(self->priv->world);
    MotoNode *view = (MotoNode *)moto_node_get_param_object(obj, "view");

    MotoParam *p = moto_node_get_param(view, "mesh");
    if( ! p)
        return;
    MotoParam *s = moto_param_get_source(p);
    if( ! s)
        return;
    moto_param_editor_update(self->priv->param_editor, moto_param_get_node(s));
    // moto_param_editor_update(self->priv->param_editor, obj);
}

static void init_gl(GtkWidget *widget, gpointer data)
{
    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);

    if(!GDK_IS_GL_DRAWABLE(gl_drawable)) return;
    if(!gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return;

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

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    moto_world_draw(twin->priv->world, width, height);

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

    gdk_gl_drawable_gl_end(gl_drawable);

    return FALSE;
}

static gboolean move = FALSE;
static gboolean zoom = FALSE;
static gboolean roll = FALSE;
static gdouble prev_x = 0;
static gdouble prev_y = 0;

static gboolean
press_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if( ! GTK_WIDGET_REALIZED(widget))
        return FALSE;

    MotoWorld *world = twin->priv->world;
    MotoObjectNode *cam = moto_world_get_camera(world);

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    prev_x = event->x;
    prev_y = event->y;

    switch(event->button)
    {
        case 1:
            if(event->state & GDK_MOD1_MASK)
                roll = TRUE;
            if( ! event->state)
                moto_world_button_press(world, event->x, event->y, width, height);
        break;
        case 2:
            if(event->state & GDK_MOD1_MASK)
                move = TRUE;
        break;
        case 3:
            if(event->state & GDK_MOD1_MASK)
                zoom = TRUE;
        break;
    }

    draw(widget, (GdkEventExpose *)event, data);
    return TRUE;
}

static gboolean
release_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if(!GTK_WIDGET_REALIZED(widget)) return FALSE;

    move = FALSE;
    zoom = FALSE;
    roll = FALSE;

    if(event->button == 2)
    {

    }

    draw(widget, (GdkEventExpose *)event, data);
    return TRUE;
}

static guint motion_ticks = 0;
static gboolean
mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    if( ! event->state)
        return TRUE;

    motion_ticks++;
    if(motion_ticks > 1)
        motion_ticks = 0;
    else
        return TRUE;

    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    // temp
    if( (! prev_x) && (! prev_y))
    {
        prev_x = event->x;
        prev_y = event->y;
    }

    if(!GDK_IS_GL_DRAWABLE(gl_drawable)) return TRUE;

    if (!gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return FALSE;

    MotoWorld *world = twin->priv->world;
    MotoObjectNode *cam = moto_world_get_camera(world);

    if(cam && move)
    {
        moto_object_node_slide(cam,
                (prev_x - event->x) / width * 15,
                (event->y - prev_y) / height * 15,
                0);
    }
    else if(cam && zoom)
    {
        gfloat xx = prev_x - event->x;
        gfloat yy = prev_y - event->y;
        gfloat factor = xx;

        if(fabs(yy) > fabs(xx))
            factor = yy;

        moto_object_node_zoom(cam, factor*0.025);
        // moto_object_node_slide(cam, 0, 0, factor*0.025);
    }
    else if(cam && roll)
    {
        gfloat xx = prev_x - event->x;
        gfloat yy = prev_y - event->y;

        if(event->state & GDK_SHIFT_MASK)
        {
            gfloat factor = xx;

            if(fabs(yy) > fabs(xx))
            {
                factor  = yy;
            }
            gint sign = (factor>0)?1:-1;
            moto_object_node_roll(cam, 1*sign);
        }
        else
        {
            moto_object_node_tumble_h(cam, yy*0.2);
            moto_object_node_tumble_v(cam, xx*0.2);
        }
    }
    else
        return TRUE;

    draw(widget, (GdkEventExpose *)event, data);

    prev_x = event->x;
    prev_y = event->y;

    return TRUE;
}



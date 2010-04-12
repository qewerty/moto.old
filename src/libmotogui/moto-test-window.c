#include "stdlib.h"
#include "math.h"

#include <gtk/gtkgl.h>
#include <gdk/gdkkeysyms.h>
#include "libmotoutil/moto-gl.h"

#include "moto-test-window.h"
#include "moto-main-menu.h"
#include "moto-tool-box.h"
#include "moto-timeline.h"
#include "moto-message-bar.h"
#include "moto-param-editor.h"
#include "moto-outliner.h"
#include "moto-graph-area.h"

#include "libmoto/moto-scene-node.h"
#include "libmoto/moto-system.h"
#include "libmoto/moto-node.h"
#include "libmoto/moto-variation.h"
#include "libmoto/moto-object-node.h"
#include "libmoto/moto-shape-node.h"
#include "libmoto/moto-mesh.h"
#include "libmotoutil/numdef.h"

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
    MotoSceneNode *scene_node;

    MotoShapeNode *gv;

    MotoParamEditor *param_editor;
    MotoOutliner *outliner;
    MotoGraphArea *graph_area;

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
    gtk_main_quit();
}

static gboolean
select_more(MotoSceneNode *scene_node, MotoObjectNode *node, gpointer user_data)
{
    moto_object_node_select_more(node);
    return TRUE;
}

static gboolean
select_less(MotoSceneNode *scene_node, MotoObjectNode *node, gpointer user_data)
{
    moto_object_node_select_less(node);
    return TRUE;
}

static gboolean
select_inverse(MotoSceneNode *scene_node, MotoObjectNode *node, gpointer user_data)
{
    moto_object_node_select_inverse(node);
    return TRUE;
}

gboolean on_key_press_event(GtkWidget   *widget,
                            GdkEventKey *event,
                            gpointer     user_data)
{
    MotoTestWindow *self = (MotoTestWindow *)widget;

    if(GDK_F11 == event->keyval)
    {
        static fullscreen = FALSE;
        if(fullscreen)
            gtk_window_unfullscreen(GTK_WINDOW(widget));
        else
            gtk_window_fullscreen(GTK_WINDOW(widget));
        fullscreen = !fullscreen;
        return TRUE;
    }

    if(0 == g_utf8_collate(event->string, "+") || \
       0 == g_utf8_collate(event->string, "="))
    {
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if(!w)
            return FALSE;
        moto_scene_node_foreach_node(w, MOTO_TYPE_OBJECT_NODE, (MotoSceneNodeForeachNodeFunc)select_more, NULL);
        draw((GtkWidget*)self->priv->area, (GdkEventExpose *)event, user_data);
    }
    else if(0 == g_utf8_collate(event->string, "-"))
    {
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if(!w)
            return FALSE;
        moto_scene_node_foreach_node(w, MOTO_TYPE_OBJECT_NODE, (MotoSceneNodeForeachNodeFunc)select_less, NULL);
        draw((GtkWidget*)self->priv->area, (GdkEventExpose *)event, user_data);
    }
    else if(0 == g_utf8_collate(event->string, "i"))
    {
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if(!w)
            return FALSE;
        moto_scene_node_foreach_node(w, MOTO_TYPE_OBJECT_NODE, (MotoSceneNodeForeachNodeFunc)select_inverse, NULL);
        draw((GtkWidget*)self->priv->area, (GdkEventExpose *)event, user_data);
    }
    else if(0 == g_utf8_collate(event->string, "z"))
    {
        // FIXME: TEMP
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if( ! w)
            return FALSE;
        MotoObjectNode *ob = moto_scene_node_get_current_object(w);
        if( ! ob)
            return FALSE;
        MotoShapeNode *gv;
        moto_node_get_param_object((MotoNode *)ob, "view", (GObject**)&gv);
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

        moto_param_set_int(dx, moto_param_get_int(dx) + 1);
        moto_node_update(node);

        moto_test_window_redraw_3dview(self);
    }
    else if(0 == g_utf8_collate(event->string, "b"))
    {
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if( ! w)
            return FALSE;

        moto_scene_node_set_use_vbo(w, ! moto_scene_node_get_use_vbo(w));

        if(moto_scene_node_get_use_vbo(w))
            moto_info("VBO enabled");
        else
            moto_info("VBO disabled");

        moto_scene_node_reset(w);
        moto_test_window_redraw_3dview(self);
    }
    else if(0 == g_utf8_collate(event->string, "a"))
    {
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if( ! w)
            return FALSE;

        moto_scene_node_set_use_arrays(w, ! moto_scene_node_get_use_arrays(w));

        if(moto_scene_node_get_use_arrays(w))
            moto_info("Arrays enabled");
        else
            moto_info("Arrays disabled");

        moto_scene_node_reset(w);
        moto_test_window_redraw_3dview(self);
    }
    else if(0 == g_utf8_collate(event->string, "n"))
    {
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if( ! w)
            return FALSE;

        moto_scene_node_set_show_normals(w, ! moto_scene_node_get_show_normals(w));

        if(moto_scene_node_get_show_normals(w))
            moto_info("Normals enabled");
        else
            moto_info("Normals disabled");

        moto_scene_node_reset(w);
        moto_test_window_redraw_3dview(self);
    }
    else if(0 == g_utf8_collate(event->string, "c"))
    {
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if( ! w)
            return FALSE;

        moto_scene_node_set_cull_faces(w, ! moto_scene_node_get_cull_faces(w));

        if(moto_scene_node_get_cull_faces(w))
            moto_info("Cull enabled");
        else
            moto_info("Cull disabled");

        moto_scene_node_reset(w);
        moto_test_window_redraw_3dview(self);
    }
    else if(65289 == event->keyval && (GDK_CONTROL_MASK & event->state))
    {
        MotoSceneNode *w = moto_system_get_current_scene(self->priv->system);
        if(!w)
            return FALSE;
        MotoObjectNode *ob = moto_scene_node_get_current_object(w);
        if(!ob)
            return FALSE;

        MotoSelectionMode mode = \
            moto_object_node_get_selection_mode(ob);

        MotoShapeNode* shape_node = \
            moto_object_node_get_shape(ob);
        if(shape_node)
        {
            MotoShape* shape = moto_shape_node_get_shape(shape_node);
            if(shape && MOTO_IS_MESH(shape))
            {
                MotoMesh* mesh = (MotoMesh*)shape;

                MotoShapeSelection* selection = \
                    moto_object_node_get_selection(ob);

                switch(mode)
                {
                    case MOTO_SELECTION_MODE_VERTEX:
                        moto_mesh_update_selection_from_verts(mesh, selection);
                    break;
                    case MOTO_SELECTION_MODE_EDGE:
                        moto_mesh_update_selection_from_edges(mesh, selection);
                    break;
                    case MOTO_SELECTION_MODE_FACE:
                        moto_mesh_update_selection_from_faces(mesh, selection);
                    break;
                    default:
                    break;
                }
            }
        }

        GEnumClass *ec = \
            (GEnumClass *)g_type_class_ref(MOTO_TYPE_SELECTION_MODE);

        gint new_mode = (mode + 1) % ec->n_values;
        moto_object_node_set_selection_mode(ob, (MotoSelectionMode)new_mode);

        g_type_class_unref(ec);

        moto_test_window_redraw_3dview(self);
    }

    return FALSE;
}

static void on_scene_node_changed(MotoSceneNode* scene_node, MotoTestWindow* window)
{
    moto_test_window_redraw_3dview(window);
    moto_test_window_update_param_editor(window);
    moto_test_window_update_outliner(window);
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

    self->priv->system = moto_system_new();
    moto_system_get_library(self->priv->system);
    self->priv->scene_node = moto_scene_node_new("MyTestSceneNode", moto_system_get_library(self->priv->system));
    moto_node_set_name(self->priv->scene_node, "scene");
    g_signal_connect(G_OBJECT(self->priv->scene_node), "changed", G_CALLBACK(on_scene_node_changed), self);
    moto_system_add_scene(self->priv->system, self->priv->scene_node, TRUE);

    MotoNode *root_node = moto_node_create_child_by_name(self->priv->scene_node, "MotoObjectNode", "root");
    moto_scene_node_set_root(self->priv->scene_node, (MotoObjectNode *)root_node);

    MotoNode *obj_node = moto_node_create_child_by_name(root_node, "MotoObjectNode", "grid");
    MotoNode *grid_node = moto_node_create_child_by_name(obj_node, "MotoGridNode", "grid");
    moto_node_link(obj_node, "shape", grid_node, "self");

    obj_node = moto_node_create_child_by_name(root_node, "MotoObjectNode", "axes");
    MotoNode *axes_node = moto_node_create_child_by_name(obj_node, "MotoAxesNode", "axes");
    moto_node_link(obj_node, "shape", axes_node, "self");

    moto_scene_node_set_axes(self->priv->scene_node, (MotoObjectNode *)obj_node);

    // Camera.
    MotoNode *cam = moto_node_create_child_by_name(self->priv->scene_node, "MotoObjectNode", "camera");
    moto_scene_node_set_camera(self->priv->scene_node, (MotoObjectNode *)cam);

    // Window
    self->priv->area = (GtkDrawingArea *)gtk_drawing_area_new();
    GTK_WIDGET_SET_FLAGS(self->priv->area, GTK_CAN_FOCUS);
    gtk_widget_set_size_request((GtkWidget *)self->priv->area, 360, 240);
    GtkWidget *area = (GtkWidget *)self->priv->area;
    GdkGLConfig *gl_config =\
                gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
    if( ! gl_config)
    {
            g_print("\nMoto: Error - Unable to initialize OpenGL!\n");
            exit(1);
    }
    gtk_widget_set_gl_capability(area, gl_config,
                NULL, TRUE, GDK_GL_RGBA_TYPE);

    // Widgets
    gtk_widget_add_events(GTK_WIDGET(area), GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK |
            GDK_BUTTON_PRESS_MASK | GDK_VISIBILITY_NOTIFY_MASK | GDK_BUTTON_RELEASE_MASK |
            GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);

    moto_scene_node_update(self->priv->scene_node);

    GtkBox *hbox = (GtkBox *)gtk_hbox_new(FALSE, 1);

    gtk_box_pack_start(hbox, moto_tool_box_new(self->priv->system), FALSE, FALSE, 0);
    GtkPaned *hp = (GtkPaned *)gtk_hpaned_new();
    gtk_box_pack_start(hbox, (GtkWidget *)hp, TRUE, TRUE, 0);
    gtk_paned_pack1(hp, (GtkWidget *)area, TRUE, FALSE);
    self->priv->param_editor = (MotoParamEditor *)moto_param_editor_new(self);
    GtkScrolledWindow *sw = (GtkScrolledWindow *)gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request((GtkWidget *)sw, 380, 36);
    gtk_scrolled_window_set_policy(sw, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_add_with_viewport(sw, (GtkWidget *)self->priv->param_editor);
    GtkPaned *vp = (GtkPaned *)gtk_vpaned_new();
    gtk_paned_pack1(vp, (GtkWidget *)sw, TRUE, FALSE);

    GtkNotebook *notebook = (GtkNotebook*)gtk_notebook_new();

    self->priv->graph_area = (MotoGraphArea *)moto_graph_area_new(self->priv->system);
    GtkScrolledWindow *sw0 = (GtkScrolledWindow *)gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(sw0, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_add_with_viewport(sw0, (GtkWidget *)self->priv->graph_area);
    gtk_notebook_append_page(notebook, sw0, gtk_label_new("Graph"));

    self->priv->outliner = (MotoOutliner *)moto_outliner_new(self, self->priv->scene_node);
    sw0 = (GtkScrolledWindow *)gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(sw0, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_add_with_viewport(sw0, (GtkWidget *)self->priv->outliner);
    gtk_notebook_append_page(notebook, sw0, gtk_label_new("Outliner"));

    gtk_paned_pack2(vp, (GtkWidget *)notebook, FALSE, FALSE);
    gtk_paned_pack2(hp, (GtkWidget *)vp, FALSE, FALSE);
    // gtk_box_pack_start(hbox, self->priv->param_editor, FALSE, FALSE, 0);

    GtkBox *vbox = (GtkBox *)gtk_vbox_new(FALSE, 1);

    gtk_box_pack_start(vbox, moto_main_menu_new(self->priv->system), FALSE, FALSE, 0);
    gtk_box_pack_start(vbox, moto_shelf_new(self->priv->system, (GtkWindow *)self), FALSE, FALSE, 0);
    gtk_box_pack_start(vbox, (GtkWidget *)hbox, TRUE, TRUE, 0);
    gtk_box_pack_start(vbox, moto_timeline_new(self->priv->scene_node), FALSE, FALSE, 0);
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


    // g_object_unref(G_OBJECT(grid_node));

    gtk_widget_grab_focus(GTK_WIDGET(area));

    gtk_window_set_title((GtkWindow *)self, "Moto v0.0");
    gtk_widget_set_size_request((GtkWidget *)self, 800, 480);
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

/* Methods of class TestWindow */

GtkWindow *moto_test_window_new()
{
    GtkWindow *self = (GtkWindow *)g_object_new(MOTO_TYPE_TEST_WINDOW, NULL);
    // MotoTestWindow *twin = (MotoTestWindow *)self;

    // MotoSceneNode *w = twin->priv->scene_node;
    //

    twin = (MotoTestWindow *)self;

    return self;
}

void moto_test_window_redraw_3dview(MotoTestWindow *self)
{
    if(!self->priv->area)
        return;

    moto_scene_node_update(self->priv->scene_node);

    GdkEvent *event = gdk_event_new(GDK_NOTHING);
    g_signal_emit_by_name(self->priv->area, "expose-event",  self->priv->area, event, NULL);
    gdk_event_free(event);
}

void moto_test_window_update_param_editor(MotoTestWindow *self)
{
    if(!self->priv->param_editor)
        return;

    MotoNode *obj = (MotoNode *)moto_scene_node_get_current_object(self->priv->scene_node);
    if(!obj)
        return;

    MotoShapeNode* shape = moto_object_node_get_shape((MotoObjectNode*)obj);
    if(!shape)
        return;

    moto_param_editor_set_node(self->priv->param_editor, shape);
}

void moto_test_window_update_param_editor_full(MotoTestWindow *self, MotoNode *node)
{
    moto_param_editor_set_node(self->priv->param_editor, node);
}

void moto_test_window_update_outliner(MotoTestWindow *self)
{
    if(!self->priv->outliner)
        return;

    moto_outliner_update(self->priv->outliner);
}

static void init_gl(GtkWidget *widget, gpointer data)
{
    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);

    if(!GDK_IS_GL_DRAWABLE(gl_drawable)) return;
    if(!gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return;

    moto_gl_init();

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

    gint width  = widget->allocation.width;
    gint height = widget->allocation.height;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    moto_scene_node_draw(twin->priv->scene_node, width, height);

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

    MotoSceneNode *scene_node = twin->priv->scene_node;
    // MotoObjectNode *cam = moto_scene_node_get_camera(scene_node);

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    prev_x = event->x;
    prev_y = event->y;

    gtk_widget_grab_focus(widget);

    switch(event->button)
    {
        case 1:
        {
            if(event->state & GDK_MOD1_MASK)
            {
                roll = TRUE;
            }
            else if( ! (event->state & GDK_SHIFT_MASK))
            {
                moto_scene_node_button_press(scene_node, event->x, event->y, width, height);
            }
        }
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

    MotoSceneNode *scene_node = twin->priv->scene_node;
    MotoObjectNode *cam = moto_scene_node_get_camera(scene_node);

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



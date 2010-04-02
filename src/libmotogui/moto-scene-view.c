#include <gtk/gtkgl.h>
#include "moto-scene-view.h"

// MotoSceneView

static GObjectClass *scene_view_parent_class = NULL;

#define MOTO_SCENE_VIEW_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_SCENE_VIEW, MotoSceneViewPriv)

typedef struct
{
    MotoWorld* world;
} MotoSceneViewPriv;

static void
moto_scene_view_dispose(GObject* obj)
{
    scene_view_parent_class->dispose(obj);
}

static void
moto_scene_view_finalize(GObject* obj)
{
    scene_view_parent_class->finalize(obj);
}

static void
moto_scene_view_init(MotoSceneView* self)
{
    GtkWidget* widget = (GtkWidget*)self;
    MotoSceneViewPriv* priv = MOTO_SCENE_VIEW_GET_PRIVATE(self);

    priv->world = NULL;

    GTK_WIDGET_SET_FLAGS(widget, GTK_CAN_FOCUS);
    gtk_widget_add_events(widget, GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK |
            GDK_BUTTON_PRESS_MASK | GDK_VISIBILITY_NOTIFY_MASK | GDK_BUTTON_RELEASE_MASK |
            GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);

    gtk_widget_set_size_request(widget, 360, 240);

    // Initializing OpenGL.
    GdkGLConfig* gl_config = \
        gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));

    gtk_widget_set_gl_capability(widget, gl_config, NULL, TRUE, GDK_GL_RGBA_TYPE);
}

static void
moto_scene_view_class_init(MotoSceneViewClass* klass)
{
    scene_view_parent_class = g_type_class_peek_parent(klass);
    g_type_class_add_private(klass, sizeof(MotoSceneViewPriv));

    ((GObjectClass*)klass)->dispose = moto_scene_view_dispose;
    ((GObjectClass*)klass)->finalize = moto_scene_view_finalize;
}

G_DEFINE_TYPE(MotoSceneView, moto_scene_view, GTK_TYPE_DRAWING_AREA);

void on_world_destroyed(MotoSceneView* sv, MotoWorld* where_the_world_was)
{
    MotoSceneViewPriv* priv = MOTO_SCENE_VIEW_GET_PRIVATE(sv);
    priv->world = NULL;
}

GtkWidget *moto_scene_view_new(MotoWorld* world)
{
    MotoSceneView *self = (MotoSceneView*)g_object_new(MOTO_TYPE_SCENE_VIEW, NULL);
    MotoSceneViewPriv* priv = MOTO_SCENE_VIEW_GET_PRIVATE(self);

    priv->world = world;
    if(world)
        g_object_weak_ref(world, (GWeakNotify)on_world_destroyed, self);

    return (GtkWidget*)self;
}

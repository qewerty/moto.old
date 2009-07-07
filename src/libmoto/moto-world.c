#include <GL/glew.h>
#include <GL/glu.h>

#include "libmotoutil/numdef.h"
#include "libmotoutil/matrix.h"

#include "moto-factory.h"
#include "moto-marshal.h"

#include "moto-world.h"
#include "moto-node.h"
#include "moto-system.h"
#include "moto-library.h"
#include "moto-object-node.h"
#include "moto-messager.h"
#include "moto-ray.h"
#include "moto-intersection.h"
#include "moto-ray-view-node.h"
#include "moto-transform-info.h"
#include "moto-time-node.h"

/* utils */

static void unref_gobject(gpointer data, gpointer user_data)
{
    g_object_unref(G_OBJECT(data));
}


/* class World */

/* for mutex factory */
static gpointer create_mutex(gpointer user_data)
{
    return (gpointer)g_mutex_new();
}
static void free_mutex(GQuark key_id, gpointer data, gpointer user_data)
{
    g_mutex_free((GMutex *)data);
}

#define get_mutex(factory, name) ((GMutex *)moto_factory_get(factory, name))

static GObjectClass *world_parent_class = NULL;

struct _MotoWorldPriv
{
    GString *name;
    GString *filename;
    gboolean changed;

    GSList *nodes;
    GSList *selected_nodes;
    MotoNode *current_node;

    MotoObjectNode *current_object;
    MotoObjectNode *root;
    MotoObjectNode *camera;
    MotoObjectNode *global_axes;
    MotoTimeNode *time_node;

    MotoLibrary *library;

    /* Default camera settings */
    gfloat fovy;
    gfloat z_near, z_far;

    /* Animation */
    GTimer *timer;
    gfloat fps;

    /* Misc */
    gboolean left_coords;
    gfloat select_bound_extent;

    MotoFactory mutex_factory;

    GMutex *node_list_mutex;
    GMutex *current_object_mutex;
    GMutex *root_object_mutex;
    GMutex *camera_object_mutex;
    GMutex *axes_object_mutex;

    MotoWorldManipulator *manipulator;

    MotoDrawMode draw_mode;

    // cache
    guint prev_width,
          prev_height;

    gboolean use_vbo;

    GThreadPool *thread_pool;
    gint max_thread_for_update;
    gboolean updating_done;
    GList *updateable_nodes;
};

static void
moto_world_dispose(GObject *obj)
{
    MotoWorld *self = (MotoWorld *)obj;
    MotoWorldPriv *priv = self->priv;

    g_timer_destroy(priv->timer);

    moto_factory_free_all(& priv->mutex_factory);

    g_slist_foreach(priv->nodes, unref_gobject, NULL);
    g_slist_free(priv->nodes);
    g_slist_free(priv->selected_nodes);

    g_string_free(priv->filename, TRUE);
    g_slice_free(MotoWorldPriv, priv);

    if(priv->thread_pool)
        g_thread_pool_free(priv->thread_pool, TRUE, FALSE);
    g_list_free(priv->updateable_nodes);

    G_OBJECT_CLASS(world_parent_class)->dispose(obj);
}

static void
moto_world_finalize(GObject *obj)
{
    world_parent_class->finalize(obj);
}

static gboolean
test_manip_button_press(MotoWorldManipulator *manip, MotoWorld *world,
        gint x, gint y, gint width, gint height, gpointer user_data)
{
    return FALSE;
}

static void
moto_world_init(MotoWorld *self)
{
    MotoWorldPriv *priv = self->priv = g_slice_new(MotoWorldPriv);

    priv->library = NULL;
    priv->manipulator = moto_world_manipulator_new();

    g_signal_connect(self->priv->manipulator, "button-press", G_CALLBACK(test_manip_button_press), NULL);

    priv->name = g_string_new("");
    priv->filename = g_string_new("");
    priv->changed = FALSE;

    priv->nodes           = NULL;
    priv->selected_nodes  = NULL;
    priv->current_node    = NULL;

    priv->current_object = NULL;
    priv->root = NULL;
    priv->camera = NULL;
    priv->global_axes = NULL;

    /* Default camera settings */
    priv->fovy = 60*RAD_PER_DEG;
    priv->z_near = 0.3;
    priv->z_far = 150;

    priv->left_coords = FALSE;
    priv->select_bound_extent = 0.2;

    moto_factory_init(& priv->mutex_factory, create_mutex, free_mutex, NULL);

    priv->draw_mode = MOTO_DRAW_MODE_WIREFRAME;

    /* Animation */
    priv->timer = g_timer_new();

    /* Misc */
    priv->node_list_mutex      = get_mutex(& self->priv->mutex_factory, "node_list_mutex");
    priv->current_object_mutex = get_mutex(& self->priv->mutex_factory, "current_object_mutex");
    priv->root_object_mutex    = get_mutex(& self->priv->mutex_factory, "root_object_mutex");
    priv->camera_object_mutex  = get_mutex(& self->priv->mutex_factory, "camera_object_mutex");
    priv->axes_object_mutex    = get_mutex(& self->priv->mutex_factory, "axes_object_mutex");

    // cache
    priv->prev_width = 640;
    priv->prev_height = 480;

    priv->use_vbo = FALSE;

    priv->thread_pool = NULL;
    priv->max_thread_for_update = 4;
    priv->updating_done = TRUE;
}

static void
moto_world_class_init(MotoWorldClass *klass)
{
    GObjectClass *goclass = (GObjectClass*)klass;

    world_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose  = moto_world_dispose;
    goclass->finalize = moto_world_finalize;

    klass->updated_signal_id = g_signal_newv ("updated",
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

G_DEFINE_TYPE(MotoWorld, moto_world, G_TYPE_OBJECT);

/* Methods of class World */

MotoWorld *moto_world_new(const gchar *name, MotoLibrary *lib)
{
    MotoWorld *self = (MotoWorld *)g_object_new(MOTO_TYPE_WORLD, NULL);

    g_string_assign(self->priv->name, name);
    self->priv->library = lib;

    return self;
}

MotoWorld *moto_world_new_from_dump(const gchar *filename, MotoLibrary *lib)
{
    MotoWorld *self = moto_world_new("", lib);

    /* load world from dump */
        /* Loading ... */

    return self;
}

const gchar *moto_world_get_name(MotoWorld *self)
{
    return self->priv->name->str;
}

void moto_world_add_node(MotoWorld *self, MotoNode *node)
{
    moto_node_set_world(node, self);

    g_mutex_lock(self->priv->node_list_mutex);
    self->priv->nodes = g_slist_append(self->priv->nodes, node);
    g_mutex_unlock(self->priv->node_list_mutex);
}

MotoNode *moto_world_create_node(MotoWorld *self,
        GType type,
        const gchar *node_name,
        const gchar *path)
{
    MotoNode *node = moto_create_node(type, node_name);
    if(node)
        moto_world_add_node(self, node);

    return node;
}

MotoNode *moto_world_create_node_by_name(MotoWorld *self,
        const gchar *type_name,
        const gchar *node_name,
        const gchar *path)
{
    MotoNode *node = moto_create_node_by_name(type_name, node_name);
    if(node)
        moto_world_add_node(self, node);

    return node;
}

void moto_world_binary_dump(MotoWorld *self,
        const gchar *filename, gboolean change_filename)
{

}

void moto_world_xml_dump(MotoWorld *self,
        const gchar *filename, gboolean change_filename)
{

}

void moto_world_binary_dump_selected(MotoWorld *self,
        const gchar *filename, gboolean change_filename)
{

}

void moto_world_xml_dump_selected(MotoWorld *self,
        const gchar *filename, gboolean change_filename)
{

}

void moto_world_merge(MotoWorld *self, MotoWorld *other)
{

}

MotoObjectNode *moto_world_get_current_object(MotoWorld *self)
{
    g_mutex_lock(self->priv->current_object_mutex);
    MotoObjectNode *co = self->priv->current_object;
    g_mutex_unlock(self->priv->current_object_mutex);
    return co;
}

void moto_world_set_object_current(MotoWorld *self, MotoObjectNode *obj)
{
    g_mutex_lock(self->priv->current_object_mutex);
    self->priv->current_object = obj;
    g_mutex_unlock(self->priv->current_object_mutex);
}

MotoObjectNode *moto_world_get_root(MotoWorld *self)
{
    g_mutex_lock(self->priv->root_object_mutex);
    MotoObjectNode *root = self->priv->root;
    g_mutex_unlock(self->priv->root_object_mutex);
    return root;
}

void moto_world_set_root(MotoWorld *self, MotoObjectNode *root)
{
    /* TODO: Check that new root is a node in this world! */
    g_mutex_lock(self->priv->root_object_mutex);
    self->priv->root = root;
    g_mutex_unlock(self->priv->root_object_mutex);
}

MotoObjectNode * moto_world_get_camera(MotoWorld *self)
{
    g_mutex_lock(self->priv->camera_object_mutex);
    MotoObjectNode *cam = self->priv->camera;
    g_mutex_unlock(self->priv->camera_object_mutex);
    return cam;
}

void moto_world_set_camera(MotoWorld *self, MotoObjectNode *camera)
{
    g_mutex_lock(self->priv->camera_object_mutex);
    self->priv->camera = camera;
    g_mutex_unlock(self->priv->camera_object_mutex);
}

MotoObjectNode * moto_world_get_axes(MotoWorld *self)
{
    g_mutex_lock(self->priv->axes_object_mutex);
    MotoObjectNode *axes = self->priv->global_axes;
    g_mutex_unlock(self->priv->axes_object_mutex);
    return axes;
}

void moto_world_set_axes(MotoWorld *self, MotoObjectNode *axes)
{
    g_mutex_lock(self->priv->axes_object_mutex);
    self->priv->global_axes = axes;
    g_mutex_unlock(self->priv->axes_object_mutex);
}

gboolean __draw_object(MotoWorld *world, MotoNode *node, gpointer user_data)
{
    moto_object_node_draw_full((MotoObjectNode *)node, FALSE, TRUE);

    return TRUE;
}

void moto_world_redraw(MotoWorld *self)
{
    moto_world_draw(self, self->priv->prev_width, self->priv->prev_height);
}

void moto_world_draw(MotoWorld *self, gint width, gint height)
{
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3, 0.3, 0.3, 1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    if(moto_world_get_camera(self))
    {
        moto_object_node_apply_camera_transform(moto_world_get_camera(self), width, height);
    }
    else
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        moto_world_apply_default_camera(self, width, height);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, width, height);
        gluLookAt(1.5, 2.0, 2.5, 0, 0, 0, 0, 0, 1);
    }

    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
    GLfloat white_light[]    = {1.0,1.0,1.0,1.0};
    // glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE,  white_light);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

    if(self->priv->left_coords)
        glScalef(1, 1, -1);

    glColor4f(1, 1, 1, 1);

    // if(moto_world_get_root(self))
    //    moto_object_node_draw_full(moto_world_get_root(self), FALSE, TRUE);

    moto_world_foreach_node(self, MOTO_TYPE_OBJECT_NODE,
            __draw_object, NULL);

    if(moto_world_get_axes(self))
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        moto_object_node_draw(moto_world_get_axes(self));
    }

    if(self->priv->manipulator)
        moto_world_manipulator_draw(self->priv->manipulator, self);
}

void moto_world_draw_fps_test(MotoWorld *self)
{
    int iters = 1000;

    GTimer *timer = g_timer_new();

    int i;
    for(i = 0; i < iters; i++)
    {
        moto_world_redraw(self);
    }
    double time = g_timer_elapsed(timer, NULL);
    moto_info("iters: %d, time: %f, fps: %.2f", iters, time, iters / time);

    g_timer_destroy(timer);
}

void moto_world_apply_default_camera(MotoWorld *self, gint width, gint height)
{
    gluPerspective(self->priv->fovy*DEG_PER_RAD, width/(GLdouble)height,
            self->priv->z_near, self->priv->z_far);
}

MotoLibrary *moto_world_get_library(MotoWorld *self)
{
    return self->priv->library;
}

MotoNode *moto_world_get_node(MotoWorld *self, const gchar *name)
{
    g_mutex_lock(self->priv->node_list_mutex);

    MotoNode *node = NULL;
    GSList *l = self->priv->nodes;
    for(; l; l = g_slist_next(l))
    {
        if(g_utf8_collate(name, moto_node_get_name((MotoNode *)l->data)) == 0)
        {
            node = (MotoNode *)l->data;
            break;
        }
    }

    g_mutex_unlock(self->priv->node_list_mutex);
    return node;
}

void moto_world_foreach_node(MotoWorld *self, GType type,
        MotoWorldForeachNodeFunc func, gpointer user_data)
{
    g_mutex_lock(self->priv->node_list_mutex);

    GSList *l = self->priv->nodes;
    for(; l; l=g_slist_next(l))
    {
        if(g_type_is_a(G_TYPE_FROM_INSTANCE(l->data), type))
        {
            if( ! func(self, (MotoNode *)l->data, user_data))
            {
                break;
            }
        }
    }

    g_mutex_unlock(self->priv->node_list_mutex);
}

void moto_world_set_use_vbo(MotoWorld *self, gboolean use)
{
    self->priv->use_vbo = use;
}

gboolean moto_world_get_use_vbo(MotoWorld *self)
{
    return self->priv->use_vbo;
}

static gboolean
__reset_geometry_view(MotoWorld *world, MotoNode *node, gpointer user_data)
{
    moto_geom_view_node_set_prepared((MotoGeomViewNode *)node, FALSE);

    return TRUE;
}

void moto_world_reset(MotoWorld *self)
{
    moto_world_foreach_node(self, MOTO_TYPE_GEOM_VIEW_NODE, __reset_geometry_view, NULL);
}

typedef struct _MotoIntersectData
{
    MotoObjectNode *obj;
    MotoRay ray;
    gfloat dist;
} MotoIntersectData;

static gboolean intersect_object(MotoWorld *world, MotoNode *node, gpointer user_data)
{

    MotoObjectNode *obj = (MotoObjectNode *)node;
    MotoIntersectData *idata = (MotoIntersectData *)user_data;
    gfloat dist;

    gfloat *iom = moto_object_node_get_inverse_matrix(obj, TRUE);

    MotoRay ray;
    moto_ray_set_transformed(& ray, & idata->ray, iom);
    moto_ray_normalize(& ray);

    MotoBound *b = moto_object_node_get_bound(obj, FALSE);
    MotoBound bb;
    moto_bound_set_extended(& bb, b, world->priv->select_bound_extent);

    if( !  moto_ray_intersect_bound_dist(& ray, & dist, bb.bound))
        return TRUE;

    if(( ! idata->obj) || dist < idata->dist)
    {
        if(dist > MICRO)
        {
            idata->obj = obj;
            idata->dist = dist;
        }
    }

    return TRUE;
}

/* Animation */

void moto_world_start_anim(MotoWorld *self)
{
    g_timer_start(self->priv->timer);
}

void moto_world_stop_anim(MotoWorld *self)
{
    g_timer_stop(self->priv->timer);
}

static gboolean post_update_node(MotoNode *node)
{
    MotoWorld *world = moto_node_get_world(node);
}

static void update_node(MotoNode *node, MotoWorld *world)
{
    moto_node_update(node);
}

void push_node(MotoNode *node, GThreadPool *tp)
{
    g_thread_pool_push(tp, node, NULL);
}

guint moto_world_get_update_complexity(MotoWorld *self)
{
    return 2000; // TODO: Implement
}

void moto_world_prepare_updateable_nodes(MotoWorld *self)
{
    
}

void moto_world_update(MotoWorld *self)
{
    MotoWorldPriv *priv = self->priv;

    if(moto_world_get_update_complexity(self) < 1000)
    {
        // TODO
    }
    else
    {
        priv->thread_pool = \
            g_thread_pool_new((GFunc)update_node, self,
                              priv->max_thread_for_update,
                              TRUE, NULL);

        moto_world_prepare_updateable_nodes(self);
        priv->updating_done = FALSE;
        while(1)
        {
            g_list_foreach(priv->updateable_nodes, (GFunc)push_node, priv->thread_pool);
            g_list_free(priv->updateable_nodes);
            priv->updateable_nodes = NULL;

            if(priv->updating_done)
                break;
        }

        g_thread_pool_free(priv->thread_pool, FALSE, TRUE);
        priv->thread_pool = NULL;
    }
}

/*  */

void moto_world_button_press(MotoWorld *self,
    gint x, gint y, gint width, gint height)
{
    if(self->priv->manipulator)
    {
        if(moto_world_manipulator_button_press(self->priv->manipulator, self,
                x, y, width, height))
            return;
    }

    /* detect intersection */

    MotoIntersectData idata;
    idata.obj = NULL;
    idata.dist = MACRO;

    MotoTransformInfo tinfo;
    tinfo.view[0] = 0;
    tinfo.view[1] = 0;
    tinfo.view[2] = width;
    tinfo.view[3] = height;

    gfloat point[3];
    if(moto_world_get_camera(self))
    {
        gfloat *cim = moto_object_node_get_inverse_matrix(moto_world_get_camera(self), TRUE);
        matrix44_copy(tinfo.model, cim);
    }
    else
    {
        matrix44_identity(tinfo.model);
    }

    GLdouble ar = width/(GLdouble)height;
    matrix44_perspective(tinfo.proj, self->priv->fovy, ar,
            self->priv->z_near, self->priv->z_far);

    GLdouble tmp_x, tmp_y, tmp_z;
    if( ! gluUnProject(x, height-y, 0.0, tinfo.model, tinfo.proj, tinfo.view,
            & tmp_x, & tmp_y, & tmp_z))
    {
        // TODO: Error

        return;
    }
    idata.ray.pos[0] = (gfloat)tmp_x;
    idata.ray.pos[1] = (gfloat)tmp_y;
    idata.ray.pos[2] = (gfloat)tmp_z;

    if( ! gluUnProject(x, height-y, 1.0, tinfo.model, tinfo.proj, tinfo.view,
            & tmp_x, & tmp_y, & tmp_z))
    {
        // TODO: Error

        return;
    }
    point[0] = (gfloat)tmp_x;
    point[1] = (gfloat)tmp_y;
    point[2] = (gfloat)tmp_z;

    vector3_dif(idata.ray.dir, point, idata.ray.pos);
    moto_ray_normalize(& idata.ray);

    moto_world_foreach_node(self, MOTO_TYPE_OBJECT_NODE,
            intersect_object, & idata);

    if(idata.obj)
    {
        gfloat *om  = moto_object_node_get_matrix(idata.obj, TRUE);
        gfloat *iom = moto_object_node_get_inverse_matrix(idata.obj, TRUE);

        MotoTransformInfo tinfo2 = tinfo;
        matrix44_mult(tinfo2.model, tinfo.model, om);

        MotoRay ray;
        moto_ray_set_transformed(& ray, & idata.ray, iom);
        moto_ray_normalize(& ray);
        moto_object_node_button_press(idata.obj, x, y, width, height, & ray,
                & tinfo2);
    }
}

void moto_world_process_button_release(MotoWorld *self,
    gint x, gint y, gint width, gint height)
{
    if(self->priv->manipulator)
    {
        moto_world_manipulator_button_release(self->priv->manipulator, self,
                x, y, width, height);

        return;
    }

    if(moto_world_get_root(self))
        moto_object_node_button_release(moto_world_get_root(self), x, y, width, height);
}

void moto_world_process_motion(MotoWorld *self,
    gint x, gint y, gint width, gint height)
{
    if(self->priv->manipulator)
    {
        moto_world_manipulator_motion_notify(self->priv->manipulator, self,
                x, y, width, height);

        return;
    }

    if(moto_world_get_root(self))
        moto_object_node_motion(moto_world_get_root(self), x, y, width, height);
}

/* MotoWorldManipulator */

static GObjectClass *world_manipulator_parent_class = NULL;

struct _MotoWorldManipulatorPriv
{
    gboolean disposed;
};

static void
moto_world_manipulator_dispose(GObject *obj)
{
    MotoWorldManipulator *self = (MotoWorldManipulator *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    world_manipulator_parent_class->dispose(obj);
}

static void
moto_world_manipulator_finalize(GObject *obj)
{
    MotoWorldManipulator *self = (MotoWorldManipulator *)obj;
    g_slice_free(MotoWorldManipulatorPriv, self->priv);

    world_manipulator_parent_class->finalize(obj);
}

static void
moto_world_manipulator_init(MotoWorldManipulator *self)
{
    self->priv = g_slice_new(MotoWorldManipulatorPriv);
    self->priv->disposed = FALSE;
}

static void
moto_world_manipulator_class_init(MotoWorldManipulatorClass *klass)
{
    GObjectClass *goclass = (GObjectClass*)klass;

    world_manipulator_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_world_manipulator_dispose;
    goclass->finalize   = moto_world_manipulator_finalize;

    GType draw_signal_types[] = {MOTO_TYPE_WORLD};
    klass->draw_signal_id = g_signal_newv ("draw",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE /* return_type */,
                 1     /* n_params */,
                 draw_signal_types  /* param_types */);

    GType button_press_signal_types[] = {MOTO_TYPE_WORLD, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT};
    klass->button_press_signal_id = g_signal_newv ("button-press",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 moto_cclosure_marshal_BOOLEAN__OBJECT_INT_INT_INT_INT,
                 G_TYPE_BOOLEAN /* return_type */,
                 5     /* n_params */,
                 button_press_signal_types  /* param_types */);

    GType button_release_signal_types[] = {MOTO_TYPE_WORLD, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT};
    klass->button_release_signal_id = g_signal_newv ("button-release",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 moto_cclosure_marshal_VOID__OBJECT_INT_INT_INT_INT,
                 G_TYPE_NONE /* return_type */,
                 5     /* n_params */,
                 button_release_signal_types /* param_types */);

    GType motion_notify_signal_types[] = {MOTO_TYPE_WORLD, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT};
    klass->motion_notify_signal_id = g_signal_newv ("motion-notify",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 moto_cclosure_marshal_VOID__OBJECT_INT_INT_INT_INT,
                 G_TYPE_NONE /* return_type */,
                 5     /* n_params */,
                 motion_notify_signal_types  /* param_types */);
}

G_DEFINE_TYPE(MotoWorldManipulator, moto_world_manipulator, G_TYPE_OBJECT);

MotoWorldManipulator *moto_world_manipulator_new()
{
    MotoWorldManipulator *self = \
        (MotoWorldManipulator *)g_object_new(MOTO_TYPE_WORLD_MANIPULATOR, NULL);

    return self;
}

void moto_world_manipulator_draw(MotoWorldManipulator *self, MotoWorld *world)
{
    g_signal_emit(self, MOTO_WORLD_MANIPULATOR_GET_CLASS(self)->draw_signal_id, 0, world);
}

gboolean moto_world_manipulator_button_press(MotoWorldManipulator *self, MotoWorld *world,
        gint x, gint y, gint width, gint height)
{
    gboolean return_value = FALSE;
    g_signal_emit(self, MOTO_WORLD_MANIPULATOR_GET_CLASS(self)->button_press_signal_id, 0,
            world, x, y, width, height, & return_value);
    return return_value;
}

void moto_world_manipulator_button_release(MotoWorldManipulator *self, MotoWorld *world,
        gint x, gint y, gint width, gint height)
{
    g_signal_emit(self, MOTO_WORLD_MANIPULATOR_GET_CLASS(self)->button_release_signal_id, 0,
            world, x, y, width, height);
}

void moto_world_manipulator_motion_notify(MotoWorldManipulator *self, MotoWorld *world,
        gint x, gint y, gint width, gint height)
{
    g_signal_emit(self, MOTO_WORLD_MANIPULATOR_GET_CLASS(self)->motion_notify_signal_id, 0,
            world, x, y, width, height);
}

void moto_world_set_draw_mode(MotoWorld *self, MotoDrawMode draw_mode)
{
    self->priv->draw_mode = draw_mode;
}

MotoDrawMode moto_world_get_draw_mode(MotoWorld *self)
{
    return self->priv->draw_mode;
}

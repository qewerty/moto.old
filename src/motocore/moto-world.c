#include <GL/glew.h>
#include <GL/glu.h>

#include "common/numdef.h"
#include "common/matrix.h"

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

    GMutex *loading_mutex;
    GMutex *add_node_mutex;
    GMutex *delete_node_mutex;
    GMutex *set_current_object_mutex;
    GMutex *set_root_mutex;
    GMutex *set_camera_mutex;
    GMutex *set_axes_mutex;

    MotoWorldManipulator *manipulator;

    // cache
    guint prev_width,
          prev_height;
};

static void
moto_world_dispose(GObject *obj)
{
    MotoWorld *self = (MotoWorld *)obj;

    g_timer_destroy(self->priv->timer);

    moto_factory_free_all(& self->priv->mutex_factory);

    g_string_free(self->priv->filename, TRUE);
    g_slice_free(MotoWorldPriv, self->priv);

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
    self->priv = g_slice_new(MotoWorldPriv);

    self->priv->library = NULL;
    self->priv->manipulator = moto_world_manipulator_new();

    g_signal_connect(self->priv->manipulator, "button-press", G_CALLBACK(test_manip_button_press), NULL);

    self->priv->name = g_string_new("");
    self->priv->filename = g_string_new("");
    self->priv->changed = FALSE;

    self->priv->nodes           = NULL;
    self->priv->selected_nodes  = NULL;
    self->priv->current_node    = NULL;

    self->priv->current_object = NULL;
    self->priv->root = NULL;
    self->priv->camera = NULL;
    self->priv->global_axes = NULL;

    /* Default camera settings */
    self->priv->fovy = 60*RAD_PER_DEG;
    self->priv->z_near = 0.3;
    self->priv->z_far = 150;

    self->priv->left_coords = FALSE;
    self->priv->select_bound_extent = 0.2;

    moto_factory_init(& self->priv->mutex_factory, create_mutex, free_mutex, NULL);

    /* Animation */
    self->priv->timer = g_timer_new();

    /* Misc */
    self->priv->loading_mutex               = get_mutex(& self->priv->mutex_factory, "loading");
    self->priv->add_node_mutex              = get_mutex(& self->priv->mutex_factory, "add_node");
    self->priv->delete_node_mutex           = get_mutex(& self->priv->mutex_factory, "delete_node");
    self->priv->set_current_object_mutex    = get_mutex(& self->priv->mutex_factory, "set_current_object");
    self->priv->set_root_mutex              = get_mutex(& self->priv->mutex_factory, "set_root");
    self->priv->set_camera_mutex            = get_mutex(& self->priv->mutex_factory, "set_camera");
    self->priv->set_axes_mutex              = get_mutex(& self->priv->mutex_factory, "set_axes");

    // cache
    self->priv->prev_width = 640;
    self->priv->prev_height = 480;
}

static void
moto_world_class_init(MotoWorldClass *klass)
{
    GObjectClass *goclass = (GObjectClass*)klass;

    world_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_world_dispose;
    goclass->finalize   = moto_world_finalize;

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

/* methods of class World */

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
    g_mutex_lock(self->priv->loading_mutex);
        /* Loading ... */
    g_mutex_unlock(self->priv->loading_mutex);

    return self;
}

const gchar *moto_world_get_name(MotoWorld *self)
{
    return self->priv->name->str;
}

void moto_world_add_node(MotoWorld *self, MotoNode *node)
{
    moto_node_set_world(node, self);

    g_mutex_lock(self->priv->add_node_mutex);
    self->priv->nodes = g_slist_append(self->priv->nodes, node);
    g_mutex_unlock(self->priv->add_node_mutex);
}

MotoNode *moto_world_create_node(MotoWorld *self,
        const gchar *type_name,
        const gchar *node_name,
        const gchar *path)
{
    MotoLibrary *lib = moto_world_get_library(self);
    if( ! lib)
    {
        GString *msg = g_string_new("I have no library and can't create nodes. :(");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return NULL;
    }

    MotoNode *node = moto_create_node_by_name(type_name, node_name);
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
    return self->priv->current_object;
}

void moto_world_set_object_current(MotoWorld *self, MotoObjectNode *obj)
{
    g_mutex_lock(self->priv->set_current_object_mutex);
    self->priv->current_object = obj;
    g_mutex_unlock(self->priv->set_current_object_mutex);
}

MotoObjectNode *moto_world_get_root(MotoWorld *self)
{
    return self->priv->root;
}

void moto_world_set_root(MotoWorld *self, MotoObjectNode *root)
{
    /* TODO: Check that new root is a node in this world! */
    g_mutex_lock(self->priv->set_root_mutex);
    self->priv->root = root;
    g_mutex_unlock(self->priv->set_root_mutex);
}

MotoObjectNode * moto_world_get_camera(MotoWorld *self)
{
    return self->priv->camera;
}

void moto_world_set_camera(MotoWorld *self, MotoObjectNode *camera)
{
    g_mutex_lock(self->priv->set_camera_mutex);
    self->priv->camera = camera;
    g_mutex_unlock(self->priv->set_camera_mutex);
}

MotoObjectNode * moto_world_get_axes(MotoWorld *self)
{
    return self->priv->global_axes;
}

void moto_world_set_axes(MotoWorld *self, MotoObjectNode *axes)
{
    g_mutex_lock(self->priv->set_axes_mutex);
    self->priv->global_axes = axes;
    g_mutex_unlock(self->priv->set_axes_mutex);
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
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3, 0.3, 0.3, 1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    if(self->priv->camera)
    {
        moto_object_node_apply_camera_transform(self->priv->camera, width, height);
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

    if(self->priv->left_coords)
        glScalef(1, 1, -1);

    glColor4f(1, 1, 1, 1);

    // if(self->priv->root)
    //    moto_object_node_draw_full(self->priv->root, FALSE, TRUE);

    moto_world_foreach_node(self, MOTO_TYPE_OBJECT_NODE,
            __draw_object, NULL);

    if(self->priv->global_axes)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        moto_object_node_draw(self->priv->global_axes);
    }

    if(self->priv->manipulator)
        moto_world_manipulator_draw(self->priv->manipulator, self);
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
    GSList *node = self->priv->nodes;
    for(; node; node = g_slist_next(node))
    {
        if(g_utf8_collate(name, moto_node_get_name((MotoNode *)node->data)) == 0)
            return (MotoNode *)node->data;
    }
    return NULL;
}

void moto_world_foreach_node(MotoWorld *self, GType type,
        MotoWorldForeachNodeFunc func, gpointer user_data)
{
    GSList *node = self->priv->nodes;
    for(; node; node=g_slist_next(node))
    {
        if(G_TYPE_CHECK_INSTANCE_TYPE(node->data, type))
        {
            if( ! func(self, (MotoNode *)node->data, user_data))
                return;
        }
    }
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

void moto_world_update(MotoWorld *self)
{
    // moto_node_update(self->priv->time_node);
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
    if(self->priv->camera)
    {
        gfloat *cim = moto_object_node_get_inverse_matrix(self->priv->camera, TRUE);
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
        /*
        g_signal_emit(G_OBJECT(idata.obj),
                MOTO_OBJECT_NODE_GET_CLASS(idata.obj)->button_press_signal_id,
                0, NULL);
                */
        gfloat *om = moto_object_node_get_matrix(idata.obj, TRUE);
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

    if(self->priv->root)
        moto_object_node_button_release(self->priv->root, x, y, width, height);
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

    if(self->priv->root)
        moto_object_node_motion(self->priv->root, x, y, width, height);
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

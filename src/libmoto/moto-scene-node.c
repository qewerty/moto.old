#include <GL/glew.h>
#include <GL/glu.h>

#include "libmotoutil/numdef.h"
#include "libmotoutil/xform.h"

#include "moto-factory.h"
#include "moto-marshal.h"

#include "moto-scene-node.h"
#include "moto-node.h"
#include "moto-system.h"
#include "moto-library.h"
#include "moto-object-node.h"
#include "moto-messager.h"
#include "moto-ray.h"
#include "moto-intersection.h"
#include "moto-transform-info.h"
#include "moto-time-node.h"

/* utils */

static void unref_gobject(gpointer data, gpointer user_data)
{
    g_object_unref(G_OBJECT(data));
}


/* class SceneNode */

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

static GObjectClass *scene_node_parent_class = NULL;

struct _MotoSceneNodePriv
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

    MotoSceneNodeManipulator *manipulator;

    MotoDrawMode draw_mode;

    // cache
    guint prev_width,
          prev_height;

    gboolean use_vbo;
    gboolean use_arrays;
    gboolean show_normals;
    gboolean cull_faces;

    GThreadPool *thread_pool;
    gint max_thread_for_update;
    gboolean updating_done;
    GList *updateable_nodes;
};

static void
moto_scene_node_dispose(GObject *obj)
{
    MotoSceneNode *self = (MotoSceneNode *)obj;
    MotoSceneNodePriv *priv = self->priv;

    g_timer_destroy(priv->timer);

    moto_factory_free_all(& priv->mutex_factory);

    g_slist_foreach(priv->nodes, unref_gobject, NULL);
    g_slist_free(priv->nodes);
    g_slist_free(priv->selected_nodes);

    g_string_free(priv->filename, TRUE);
    g_slice_free(MotoSceneNodePriv, priv);

    if(priv->thread_pool)
        g_thread_pool_free(priv->thread_pool, TRUE, FALSE);
    g_list_free(priv->updateable_nodes);

    G_OBJECT_CLASS(scene_node_parent_class)->dispose(obj);
}

static void
moto_scene_node_finalize(GObject *obj)
{
    scene_node_parent_class->finalize(obj);
}

static gboolean
test_manip_button_press(MotoSceneNodeManipulator *manip, MotoSceneNode *scene_node,
        gint x, gint y, gint width, gint height, gpointer user_data)
{
    return FALSE;
}

static void
moto_scene_node_init(MotoSceneNode *self)
{
    MotoNode* node = (MotoNode*)self;
    MotoSceneNodePriv *priv = self->priv = g_slice_new(MotoSceneNodePriv);

    priv->library = NULL;
    priv->manipulator = moto_scene_node_manipulator_new();

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

    moto_node_add_params(node,
            "cull_face", "Call Face", MOTO_TYPE_CULL_FACE_MODE, MOTO_PARAM_MODE_INOUT, MOTO_CULL_FACE_MODE_BACK, NULL, "View",
            NULL);
}

static void
moto_scene_node_class_init(MotoSceneNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass*)klass;

    scene_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose  = moto_scene_node_dispose;
    goclass->finalize = moto_scene_node_finalize;

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

    klass->changed_signal_id = g_signal_newv ("changed",
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

G_DEFINE_TYPE(MotoSceneNode, moto_scene_node, MOTO_TYPE_NODE);

/* Methods of class SceneNode */

MotoSceneNode *moto_scene_node_new(const gchar *name, MotoLibrary *lib)
{
    MotoSceneNode *self = (MotoSceneNode *)g_object_new(MOTO_TYPE_SCENE_NODE, NULL);

    g_string_assign(self->priv->name, name);
    self->priv->library = lib;

    return self;
}

MotoSceneNode *moto_scene_node_new_from_dump(const gchar *filename, MotoLibrary *lib)
{
    MotoSceneNode *self = moto_scene_node_new("", lib);

    /* load scene_node from dump */
        /* Loading ... */

    return self;
}

const gchar *moto_scene_node_get_name(MotoSceneNode *self)
{
    return self->priv->name->str;
}

void moto_scene_node_add_node(MotoSceneNode *self, MotoNode *node)
{
    const char* name = moto_node_get_name(node);
    if(moto_scene_node_get_node(self, name))
    {
        moto_error("Can't add node '%s' into the scene_node. Name is busy.\n", name);
        return;
    }

    moto_node_set_scene_node(node, self);

    g_mutex_lock(self->priv->node_list_mutex);
    self->priv->nodes = g_slist_append(self->priv->nodes, node);
    g_mutex_unlock(self->priv->node_list_mutex);

    g_signal_emit_by_name(self, "changed");
}

MotoNode *moto_scene_node_create_node(MotoSceneNode *self,
        GType type,
        const gchar *node_name,
        const gchar *path)
{
    MotoNode *node = moto_create_node(type, node_name);
    if(node)
        moto_scene_node_add_node(self, node);

    return node;
}

MotoNode *moto_scene_node_create_node_by_name(MotoSceneNode *self,
        const gchar *type_name,
        const gchar *node_name,
        const gchar *path)
{
    MotoNode *node = moto_create_node_by_name(type_name, node_name);
    if(node)
    {
        int count = 0;
        GString* new_name = g_string_new(moto_node_get_name(node));
        while(moto_scene_node_get_node(self, new_name->str))
        {
            g_string_printf(new_name, "%s%d", node_name, count++);
        }

        moto_node_set_name(node, new_name->str);
        moto_scene_node_add_node(self, node);
    }

    return node;
}

void moto_scene_node_binary_dump(MotoSceneNode *self,
        const gchar *filename, gboolean change_filename)
{

}

void moto_scene_node_xml_dump(MotoSceneNode *self,
        const gchar *filename, gboolean change_filename)
{

}

void moto_scene_node_binary_dump_selected(MotoSceneNode *self,
        const gchar *filename, gboolean change_filename)
{

}

void moto_scene_node_xml_dump_selected(MotoSceneNode *self,
        const gchar *filename, gboolean change_filename)
{

}

void moto_scene_node_merge(MotoSceneNode *self, MotoSceneNode *other)
{

}

MotoObjectNode *moto_scene_node_get_current_object(MotoSceneNode *self)
{
    g_mutex_lock(self->priv->current_object_mutex);
    MotoObjectNode *co = self->priv->current_object;
    g_mutex_unlock(self->priv->current_object_mutex);
    return co;
}

void moto_scene_node_set_object_current(MotoSceneNode *self, MotoObjectNode *obj)
{
    g_mutex_lock(self->priv->current_object_mutex);
    self->priv->current_object = obj;
    g_mutex_unlock(self->priv->current_object_mutex);
}

MotoObjectNode *moto_scene_node_get_root(MotoSceneNode *self)
{
    g_mutex_lock(self->priv->root_object_mutex);
    MotoObjectNode *root = self->priv->root;
    g_mutex_unlock(self->priv->root_object_mutex);
    return root;
}

void moto_scene_node_set_root(MotoSceneNode *self, MotoObjectNode *root)
{
    /* TODO: Check that new root is a node in this scene_node! */
    g_mutex_lock(self->priv->root_object_mutex);
    self->priv->root = root;
    g_mutex_unlock(self->priv->root_object_mutex);
}

MotoObjectNode * moto_scene_node_get_camera(MotoSceneNode *self)
{
    g_mutex_lock(self->priv->camera_object_mutex);
    MotoObjectNode *cam = self->priv->camera;
    g_mutex_unlock(self->priv->camera_object_mutex);
    return cam;
}

void moto_scene_node_set_camera(MotoSceneNode *self, MotoObjectNode *camera)
{
    g_mutex_lock(self->priv->camera_object_mutex);
    self->priv->camera = camera;
    g_mutex_unlock(self->priv->camera_object_mutex);
}

MotoObjectNode * moto_scene_node_get_axes(MotoSceneNode *self)
{
    g_mutex_lock(self->priv->axes_object_mutex);
    MotoObjectNode *axes = self->priv->global_axes;
    g_mutex_unlock(self->priv->axes_object_mutex);
    return axes;
}

void moto_scene_node_set_axes(MotoSceneNode *self, MotoObjectNode *axes)
{
    g_mutex_lock(self->priv->axes_object_mutex);
    self->priv->global_axes = axes;
    g_mutex_unlock(self->priv->axes_object_mutex);
}

void moto_scene_node_redraw(MotoSceneNode *self)
{
    moto_scene_node_draw(self, self->priv->prev_width, self->priv->prev_height);
}

void moto_scene_node_draw(MotoSceneNode *self, gint width, gint height)
{
    moto_scene_node_update(self);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3, 0.3, 0.3, 1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);

    MotoObjectNode* cam = moto_scene_node_get_camera(self);

    if(cam)
    {
        moto_object_node_apply_camera_transform(cam, width, height);
    }
    else
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        moto_scene_node_apply_default_camera(self, width, height);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, width, height);
        gluLookAt(1.5, 2.0, 2.5, 0, 0, 0, 0, 0, 1);
    }

    if(self->priv->left_coords)
        glScalef(1, 1, -1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    if(cam)
    {
        glMultMatrixf(moto_object_node_get_matrix(cam, TRUE));
    }

    GLfloat light_position[] = {0, 0, 0, 1};
    GLfloat light_direction[] = {0, 0, 1, 0};
    gfloat tmp;
    vector3_normalize(light_direction, tmp);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat ambi[] = {1, 1, 1, 1};
    GLfloat diff[] = {1, 1, 1, 1};
    GLfloat spec[] = {1, 1, 1, 1};
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    glPopMatrix();

    glColor4f(1, 1, 1, 1);

    const GList* child = moto_node_get_children((MotoNode*)self);
    for(; child; child = g_list_next(child))
    {
        MotoObjectNode* obj = (MotoObjectNode*)child->data;
        if(MOTO_IS_OBJECT_NODE(obj))
            moto_object_node_draw(obj);
    }

    if(moto_scene_node_get_axes(self))
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        moto_object_node_draw(moto_scene_node_get_axes(self));
    }

    if(self->priv->manipulator)
        moto_scene_node_manipulator_draw(self->priv->manipulator, self);
}

void moto_scene_node_draw_fps_test(MotoSceneNode *self)
{
    int iters = 1000;

    GTimer *timer = g_timer_new();

    int i;
    for(i = 0; i < iters; i++)
    {
        moto_scene_node_redraw(self);
    }
    double time = g_timer_elapsed(timer, NULL);
    moto_info("iters: %d, time: %f, fps: %.2f", iters, time, iters / time);

    g_timer_destroy(timer);
}

void moto_scene_node_apply_default_camera(MotoSceneNode *self, gint width, gint height)
{
    gluPerspective(self->priv->fovy*DEG_PER_RAD, width/(GLdouble)height,
            self->priv->z_near, self->priv->z_far);
}

MotoLibrary *moto_scene_node_get_library(MotoSceneNode *self)
{
    return self->priv->library;
}

MotoNode *moto_scene_node_get_node(MotoSceneNode *self, const gchar *name)
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

gboolean moto_node_foreach_child(MotoNode *self, GType type,
    MotoSceneNodeForeachNodeFunc func, gpointer user_data)
{
    GList *l = moto_node_get_children(self);
    for(; l; l = g_list_next(l))
    {
        if(g_type_is_a(G_TYPE_FROM_INSTANCE(l->data), type))
        {
            if(!func(self, (MotoNode *)l->data, user_data))
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

void moto_scene_node_foreach_node(MotoSceneNode *self, GType type,
    MotoSceneNodeForeachNodeFunc func, gpointer user_data)
{
    GList *l = moto_node_get_children((MotoNode*)self);
    for(; l; l = g_list_next(l))
    {
        MotoNode* node = (MotoNode *)l->data;

        if(g_type_is_a(G_TYPE_FROM_INSTANCE(l->data), type))
        {
            if(!func(self, node, user_data))
            {
                break;
            }
        }

        if(!moto_node_foreach_child(node, type, func, user_data))
            break;
    }
}

void moto_scene_node_set_use_vbo(MotoSceneNode *self, gboolean use)
{
    self->priv->use_vbo = use;
}

gboolean moto_scene_node_get_use_vbo(MotoSceneNode *self)
{
    return self->priv->use_vbo;
}

void moto_scene_node_set_use_arrays(MotoSceneNode *self, gboolean use)
{
    self->priv->use_arrays = use;
}

gboolean moto_scene_node_get_use_arrays(MotoSceneNode *self)
{
    return self->priv->use_arrays;
}

void moto_scene_node_set_show_normals(MotoSceneNode *self, gboolean show)
{
    self->priv->show_normals = show;
}

gboolean moto_scene_node_get_show_normals(MotoSceneNode *self)
{
    return self->priv->show_normals;
}

void moto_scene_node_set_cull_faces(MotoSceneNode *self, gboolean cull)
{
    self->priv->cull_faces = cull;
}

gboolean moto_scene_node_get_cull_faces(MotoSceneNode *self)
{
    return self->priv->cull_faces;
}

static gboolean
reset_shape(MotoSceneNode *scene_node, MotoNode *node, gpointer user_data)
{
    moto_shape_node_reset((MotoShapeNode *)node);
    return TRUE;
}

void moto_scene_node_reset(MotoSceneNode *self)
{
    moto_scene_node_foreach_node(self, MOTO_TYPE_SHAPE_NODE, reset_shape, NULL);
}

typedef struct _MotoIntersectData
{
    MotoObjectNode *obj;
    MotoRay ray;
    gfloat dist;
} MotoIntersectData;

static gboolean intersect_object(MotoSceneNode *scene_node, MotoNode *node, gpointer user_data)
{
    if(!MOTO_IS_OBJECT_NODE(node))
        return TRUE;

    MotoObjectNode *obj = (MotoObjectNode *)node;
    MotoIntersectData *idata = (MotoIntersectData *)user_data;
    gfloat dist;

    gfloat *iom = moto_object_node_get_inverse_matrix(obj, TRUE);

    MotoRay ray;
    moto_ray_set_transformed(& ray, & idata->ray, iom);
    moto_ray_normalize(& ray);

    MotoBound *b = moto_object_node_get_bound(obj, FALSE);
    MotoBound bb;
    moto_bound_set_extended(& bb, b, scene_node->priv->select_bound_extent);

    if(!moto_ray_intersect_bound_dist(& ray, & dist, bb.bound))
        return TRUE;

    if((!idata->obj) || dist < idata->dist)
    {
        if(dist > MICRO)
        {
            idata->obj = obj;
            idata->dist = dist;
        }
    }

    GList* child = moto_node_get_children(node);
    for(; child; child = g_list_next(child))
    {
        if(intersect_object(scene_node, (MotoNode*)child->data, user_data))
            return TRUE;
    }

    return TRUE;
}

/* Animation */

void moto_scene_node_start_anim(MotoSceneNode *self)
{
    g_timer_start(self->priv->timer);
}

void moto_scene_node_stop_anim(MotoSceneNode *self)
{
    g_timer_stop(self->priv->timer);
}

static void update_node(MotoNode *node, MotoSceneNode *scene_node)
{
    moto_node_update(node);
}

void push_node(MotoNode *node, GThreadPool *tp)
{
    g_thread_pool_push(tp, node, NULL);
}

guint moto_scene_node_get_update_complexity(MotoSceneNode *self)
{
    return 0; // TODO: Implement
}

void moto_scene_node_prepare_updateable_nodes(MotoSceneNode *self)
{
    
}


static gboolean __update_node(MotoNode* node)
{
    gboolean smth_updated = FALSE;

    if(moto_node_is_ready_to_update(node) && moto_node_needs_update(node))
    {
        moto_node_update(node);
        smth_updated = TRUE;
    }

    GList* child = moto_node_get_children(node);
    for(; child; child = g_list_next(child))
    {
        if(__update_node((MotoNode*)child->data))
            smth_updated = TRUE;
    }

    return smth_updated;
}

static void scene_node_update(MotoSceneNode *self)
{
    MotoSceneNodePriv *priv = self->priv;

    gboolean smth_updated = FALSE;

    const GList* child = moto_node_get_children((MotoNode*)self);
    for(; child; child = g_list_next(child))
    {
        MotoNode *node = (MotoNode*)child->data;
        if(__update_node(node))
            smth_updated = TRUE;
    }

    if(smth_updated)
        scene_node_update(self);
}

void moto_scene_node_update(MotoSceneNode *self)
{
    MotoSceneNodePriv *priv = self->priv;

    if(moto_scene_node_get_update_complexity(self) < 1000)
    {
        scene_node_update(self);
    }
    else
    {
        priv->thread_pool = \
            g_thread_pool_new((GFunc)update_node, self,
                              priv->max_thread_for_update,
                              TRUE, NULL);

        moto_scene_node_prepare_updateable_nodes(self);
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

void moto_scene_node_button_press(MotoSceneNode *self,
    gint x, gint y, gint width, gint height)
{
    if(self->priv->manipulator)
    {
        if(moto_scene_node_manipulator_button_press(self->priv->manipulator, self,
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

    GLdouble ar = width/(GLdouble)height;

    gfloat fov = self->priv->fovy;

    MotoObjectNode* camera = moto_scene_node_get_camera(self);

    gfloat point[3];
    if(camera)
    {
        gfloat *cim = moto_object_node_get_inverse_matrix(camera, TRUE);
        matrix44_copy(tinfo.model, cim);

        moto_node_get_param_float((MotoNode*)camera, "fov", &fov);
        fov *= RAD_PER_DEG;
    }
    else
    {
        matrix44_identity(tinfo.model);
    }

    matrix44_perspective(tinfo.proj, fov, ar, 0.3, 150);

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

    const GList* child = moto_node_get_children((MotoNode*)self);
    for(; child; child = g_list_next(child))
    {
        MotoNode* node = (MotoNode*)child->data;
        if(MOTO_IS_OBJECT_NODE(node))
            intersect_object(self, node, &idata);
    }

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

void moto_scene_node_process_button_release(MotoSceneNode *self,
    gint x, gint y, gint width, gint height)
{
    if(self->priv->manipulator)
    {
        moto_scene_node_manipulator_button_release(self->priv->manipulator, self,
                x, y, width, height);

        return;
    }

    if(moto_scene_node_get_root(self))
        moto_object_node_button_release(moto_scene_node_get_root(self), x, y, width, height);
}

void moto_scene_node_process_motion(MotoSceneNode *self,
    gint x, gint y, gint width, gint height)
{
    if(self->priv->manipulator)
    {
        moto_scene_node_manipulator_motion_notify(self->priv->manipulator, self,
                x, y, width, height);

        return;
    }

    if(moto_scene_node_get_root(self))
        moto_object_node_motion(moto_scene_node_get_root(self), x, y, width, height);
}

/* MotoSceneNodeManipulator */

static GObjectClass *scene_node_manipulator_parent_class = NULL;

struct _MotoSceneNodeManipulatorPriv
{
    gboolean disposed;
};

static void
moto_scene_node_manipulator_dispose(GObject *obj)
{
    MotoSceneNodeManipulator *self = (MotoSceneNodeManipulator *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    scene_node_manipulator_parent_class->dispose(obj);
}

static void
moto_scene_node_manipulator_finalize(GObject *obj)
{
    MotoSceneNodeManipulator *self = (MotoSceneNodeManipulator *)obj;
    g_slice_free(MotoSceneNodeManipulatorPriv, self->priv);

    scene_node_manipulator_parent_class->finalize(obj);
}

static void
moto_scene_node_manipulator_init(MotoSceneNodeManipulator *self)
{
    self->priv = g_slice_new(MotoSceneNodeManipulatorPriv);
    self->priv->disposed = FALSE;
}

static void
moto_scene_node_manipulator_class_init(MotoSceneNodeManipulatorClass *klass)
{
    GObjectClass *goclass = (GObjectClass*)klass;

    scene_node_manipulator_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_scene_node_manipulator_dispose;
    goclass->finalize   = moto_scene_node_manipulator_finalize;

    GType draw_signal_types[] = {MOTO_TYPE_SCENE_NODE};
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

    GType button_press_signal_types[] = {MOTO_TYPE_SCENE_NODE, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT};
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

    GType button_release_signal_types[] = {MOTO_TYPE_SCENE_NODE, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT};
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

    GType motion_notify_signal_types[] = {MOTO_TYPE_SCENE_NODE, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT};
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

G_DEFINE_TYPE(MotoSceneNodeManipulator, moto_scene_node_manipulator, G_TYPE_OBJECT);

MotoSceneNodeManipulator *moto_scene_node_manipulator_new()
{
    MotoSceneNodeManipulator *self = \
        (MotoSceneNodeManipulator *)g_object_new(MOTO_TYPE_SCENE_NODE_MANIPULATOR, NULL);

    return self;
}

void moto_scene_node_manipulator_draw(MotoSceneNodeManipulator *self, MotoSceneNode *scene_node)
{
    g_signal_emit(self, MOTO_SCENE_NODE_MANIPULATOR_GET_CLASS(self)->draw_signal_id, 0, scene_node);
}

gboolean moto_scene_node_manipulator_button_press(MotoSceneNodeManipulator *self, MotoSceneNode *scene_node,
        gint x, gint y, gint width, gint height)
{
    gboolean return_value = FALSE;
    g_signal_emit(self, MOTO_SCENE_NODE_MANIPULATOR_GET_CLASS(self)->button_press_signal_id, 0,
            scene_node, x, y, width, height, & return_value);
    return return_value;
}

void moto_scene_node_manipulator_button_release(MotoSceneNodeManipulator *self, MotoSceneNode *scene_node,
        gint x, gint y, gint width, gint height)
{
    g_signal_emit(self, MOTO_SCENE_NODE_MANIPULATOR_GET_CLASS(self)->button_release_signal_id, 0,
            scene_node, x, y, width, height);
}

void moto_scene_node_manipulator_motion_notify(MotoSceneNodeManipulator *self, MotoSceneNode *scene_node,
        gint x, gint y, gint width, gint height)
{
    g_signal_emit(self, MOTO_SCENE_NODE_MANIPULATOR_GET_CLASS(self)->motion_notify_signal_id, 0,
            scene_node, x, y, width, height);
}

void moto_scene_node_set_draw_mode(MotoSceneNode *self, MotoDrawMode draw_mode)
{
    self->priv->draw_mode = draw_mode;
    moto_scene_node_reset(self);
    moto_scene_node_redraw(self);
}

MotoDrawMode moto_scene_node_get_draw_mode(MotoSceneNode *self)
{
    return self->priv->draw_mode;
}

#include <GL/gl.h>
#include <GL/glu.h>

#include "moto-world.h"
#include "moto-node.h"
#include "moto-system.h"
#include "moto-library.h"
#include "moto-object-node.h"
#include "moto-messager.h"
#include "moto-ray.h"
#include "moto-intersection.h"
// #include "moto-time-node.h"

/* class World */

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
    // MotoTimeNode *time;

    MotoLibrary *library;
};

static void
moto_world_dispose(GObject *obj)
{
    MotoWorld *self = (MotoWorld *)obj;

    g_string_free(self->priv->filename, TRUE);
    g_slice_free(MotoWorldPriv, self->priv);

    G_OBJECT_CLASS(world_parent_class)->dispose(obj);
}

static void
moto_world_finalize(GObject *obj)
{
    world_parent_class->finalize(obj);
}

static void
moto_world_init(MotoWorld *self)
{
    self->priv = g_slice_new(MotoWorldPriv);

    self->priv->library = NULL;

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
}

static void
moto_world_class_init(MotoWorldClass *klass)
{
    GObjectClass *goclass = (GObjectClass*)klass;

    world_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_world_dispose;
    goclass->finalize   = moto_world_finalize;
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

    return self;
}

const gchar *moto_world_get_name(MotoWorld *self)
{
    return self->priv->name->str;
}

void moto_world_add_node(MotoWorld *self, MotoNode *node)
{
    moto_node_set_world(node, self);
    self->priv->nodes = g_slist_append(self->priv->nodes, node);
}

MotoNode *moto_world_create_node(MotoWorld *self,
        const gchar *type_name, const gchar *node_name)
{
    MotoLibrary *lib = moto_world_get_library(self);
    if( ! lib)
    {
        GString *msg = g_string_new("I have no library and can't create nodes. :(");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return NULL;
    }

    MotoNodeFactory *fac = (MotoNodeFactory *)moto_library_get_entry(lib, "node", type_name);
    if( ! fac)
    {
        GString *msg = g_string_new("There is no node type with name \"");
        g_string_append(msg, type_name);
        g_string_append(msg, "\" in my library. I can't create it. ");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return NULL;
    }

    MotoNode *node = moto_node_factory_create_node(fac, node_name);
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

void moto_world_merge(MotoWorld *self, const gchar *filename)
{

}

MotoObjectNode *moto_world_get_current_object(MotoWorld *self)
{
    return self->priv->current_object;
}

void moto_world_set_object_current(MotoWorld *self, MotoObjectNode *obj)
{
    self->priv->current_object = obj;
}

MotoObjectNode *moto_world_get_root(MotoWorld *self)
{
    return self->priv->root;
}

void moto_world_set_root(MotoWorld *self, MotoObjectNode *root)
{
    /* TODO: Check that new root is a node in this world! */
    self->priv->root = root;
}

MotoObjectNode * moto_world_get_camera(MotoWorld *self)
{
    return self->priv->camera;
}

void moto_world_set_camera(MotoWorld *self, MotoObjectNode *camera)
{
    self->priv->camera = camera;
}

MotoObjectNode * moto_world_get_axes(MotoWorld *self)
{
    return self->priv->global_axes;
}

void moto_world_set_axes(MotoWorld *self, MotoObjectNode *axes)
{
    self->priv->global_axes = axes;
}

void moto_world_draw(MotoWorld *self, gint width, gint height)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2, 0.2, 0.2, 1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    if(self->priv->camera)
    {
        moto_object_node_apply_camera_transform(self->priv->camera, width, height);
    }
    else
    {
        // g_print("NO CAMERA ;(\n");
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        moto_world_apply_default_camera(self, width, height);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, width, height);
        gluLookAt(1.5, 2.0, 2.5, 0, 0, 0, 0, 0, 1);
    }

    // Uncommet to make world coordinate system left
    // glScalef(1, 1, -1);

    glColor4f(1, 1, 1, 1);

    if(self->priv->root)
        moto_object_node_draw(self->priv->root);

    if(self->priv->global_axes)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        moto_object_node_draw(self->priv->global_axes);
    }
}

void moto_world_apply_default_camera(MotoWorld *self, gint width, gint height)
{
    gluPerspective(60, width/(float)height, 0.3, 150.);
}

MotoLibrary *moto_world_get_library(MotoWorld *self)
{
    return self->priv->library;
}

typedef struct _MotoIntersectData
{
    MotoObjectNode *obj;
    MotoRay ray;
    gfloat dist;
} MotoIntersectData;

static void intersect_object(gpointer data, gpointer user_data)
{

    MotoObjectNode *obj = (MotoObjectNode *)data;
    MotoIntersectData *idata = (MotoIntersectData *)user_data;

    gfloat *iom = moto_object_node_get_inverse_matrix(obj, TRUE);

    MotoRay ray;
    moto_ray_set_transformed(& ray, & idata->ray, iom);

    MotoIntersection intersection;
    moto_intersection_reset(& intersection);

    MotoBound *b = moto_object_node_get_bound(obj, FALSE);
    moto_bound_intersect_ray(b, & ray, & intersection);

    if( ! intersection.hits_num)
        return;
    moto_intersection_reset(& intersection);

    moto_object_node_intersect_ray(b, & ray, & intersection);

    if( ! intersection.hits_num)
        return;



    if(intersection.dist < idata->dist || ( ! idata->obj))
    {
        idata->obj = obj;

        idata->dist = intersection.dist;
    }
}

void moto_world_process_button_press(MotoWorld *self,
    gint x, gint y, gint width, gint height)
{
    /*
    if(self->priv->manipulator)
    {
        moto_world_manipulator_process_button_press(self->priv->manipulator,
                x, y, width, height);

        return;
    }
    */

    /* detect intersection */

    /* TODO: Temporary camera data */
    gfloat fovy = 60*RAD_PER_DEG;
    gfloat z_near = 0.3;
    gfloat z_far = 150;

    MotoIntersectData idata;
    idata.obj = NULL;
    idata.dist = MACRO;

    gfloat tmp;
    gfloat point[3];
    gfloat model[16];
    matrix44_identity(model);
    gfloat proj[16];
    matrix44_perspective(proj, fovy, width/(gfloat)height, z_near, z_far);
    gfloat viewport[] = {0 , 0, width, height};

    if( ! gluUnProject(x, y, 0, model, proj, viewport,
            & idata.ray.pos[0], & idata.ray.pos[1], & idata.ray.pos[2]))
    {
        // TODO: Error

        return;
    }
    if( ! gluUnProject(x, y, 1, model, proj, viewport,
            & point[0], & point[1], & point[2]))
    {
        // TODO: Error

        return;
    }
    vector3_dif(idata.ray.dir, point, idata.ray.pos);
    vector3_normalize(idata.ray.dir, tmp);

    moto_world_foreach_node(self, MOTO_TYPE_OBJECT_NODE,
            intersect_object, & idata);
    if(idata.obj)
    {
        g_signal_emit(G_OBJECT(idata.obj),
                MOTO_OBJECT_NODE_GET_CLASS(object)->button_press_signal_id,
                0, NULL);
    }
}

void moto_world_process_button_release(MotoWorld *self,
    gint x, gint y, gint width, gint height)
{
    /*
    if(self->priv->manipulator)
    {
        moto_world_manipulator_process_button_release(self->priv->manipulator,
                x, y, width, height);

        return;
    }
    */

    if(self->priv->root)
        moto_object_node_process_button_release(self->priv->root, x, y, width, height);
}

void moto_world_process_motion(MotoWorld *self,
    gint x, gint y, gint width, gint height)
{
    /*
    if(self->priv->manipulator)
    {
        moto_world_manipulator_process_motion(self->priv->manipulator,
                x, y, width, height);

        return;
    }
    */

    if(self->priv->root)
        moto_object_node_process_motion(self->priv->root, x, y, width, height);
}

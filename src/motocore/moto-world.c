#include <GL/gl.h>
#include <GL/glu.h>

#include "common/numdef.h"
#include "common/matrix.h"

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

    // Default camera settings
    gfloat fovy;
    gfloat z_near, z_far;

    // Misc
    gboolean left_coords;
    gfloat select_bound_extent;
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

    // Default camera settings
    self->priv->fovy = 60*RAD_PER_DEG;
    self->priv->z_near = 0.3;
    self->priv->z_far = 150;

    self->priv->left_coords = FALSE;
    self->priv->select_bound_extent = 0.2;
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

void moto_world_merge(MotoWorld *self, MotoWorld *other)
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
        if(G_TYPE_CHECK_INSTANCE_TYPE(node->data, MOTO_TYPE_OBJECT_NODE))
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

void moto_world_button_press(MotoWorld *self,
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

    // TEMP
    MotoRayViewNode *rv = (MotoRayViewNode *)moto_world_get_node(self, "RayView");
    moto_ray_view_node_set_ray(rv, & idata.ray);

    if(idata.obj)
    {
        /*
        g_signal_emit(G_OBJECT(idata.obj),
                MOTO_OBJECT_NODE_GET_CLASS(idata.obj)->button_press_signal_id,
                0, NULL);
                */
        gfloat *om = moto_object_node_get_matrix(idata.obj, TRUE);
        gfloat *iom = moto_object_node_get_inverse_matrix(idata.obj, TRUE);

        MotoTransformInfo tinfo2;
        moto_transform_info_set_proj(& tinfo2, tinfo.proj);
        moto_transform_info_set_view(& tinfo2, tinfo.view);
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
    /*
    if(self->priv->manipulator)
    {
        moto_world_manipulator_process_button_release(self->priv->manipulator,
                x, y, width, height);

        return;
    }
    */

    if(self->priv->root)
        moto_object_node_button_release(self->priv->root, x, y, width, height);
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
        moto_object_node_motion(self->priv->root, x, y, width, height);
}

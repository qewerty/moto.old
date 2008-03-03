#include <GL/gl.h>
#include <GL/glu.h>

#include "moto-world.h"
#include "moto-node.h"
#include "moto-system.h"
#include "moto-library.h"
#include "moto-object-node.h"
#include "moto-messager.h"
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

    MotoObjectNode *root;
    MotoObjectNode *camera;
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

    self->priv->root = NULL;
    self->priv->camera = NULL;
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

void moto_world_draw(MotoWorld *self, gint width, gint height)
{
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

    // glScalef(1, 1, -1);

    if(self->priv->root)
        moto_object_node_draw(self->priv->root);
}

void moto_world_apply_default_camera(MotoWorld *self, gint width, gint height)
{
    gluPerspective(60, width/(float)height, 0.3, 150.);
}

MotoLibrary *moto_world_get_library(MotoWorld *self)
{
    return self->priv->library;
}


#include "moto-world.h"
#include "moto-object-node.h"
#include "moto-time-node.h"

ХУЙ

/* class World */

static GObjectClass *world_parent_class = NULL;

struct _MotoWorldPriv
{
    GString *filename;
    gboolean changed;

    GList *nodes;
    GList *selected_nodes;
    MotoNode *current_node;

    MotoObjectNode *root;
    MotoTimeNode *time;
};

Бла-бла!

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

    self->priv->filename = g_string_new("");
    self->priv->changed = FALSE;
}

static void
moto_world_class_init(MotoWorldClass *klass)
{
    world_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    world_parent_class->dispose = moto_world_dispose;
    world_parent_class->finalize = moto_world_finalize;
}

G_DEFINE_TYPE(MotoWorld, moto_world, G_TYPE_OBJECT);

/* methods of class World */

MotoWorld *moto_world_new(const gchar *name)
{
    MotoWorld *self = (MotoWorld *)g_object_new(MOTO_TYPE_WORLD, NULL);

    return self;
}

MotoWorld *moto_world_new_from_dump(const gchar *filename)
{
    MotoWorld *self = moto_world_new("");

    return self;
}

void moto_world_binary_dump(MotoWorld *self,
        const *gchar filename, gboolean change_filename)
{

}

void moto_world_xml_dump(MotoWorld *self,
        const *gchar filename, gboolean change_filename)
{

}

void moto_world_binary_dump_selected(MotoWorld *self,
        const *gchar filename, gboolean change_filename)
{

}

void moto_world_xml_dump_selected(MotoWorld *self,
        const *gchar filename, gboolean change_filename)
{

}

void moto_world_merge(MotoWorld *self, const gchar *filename)
{

}


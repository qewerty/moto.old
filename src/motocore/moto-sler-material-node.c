#include <GL/gl.h>

#include "moto-sler-material-node.h"

/* forward */

static void moto_sler_material_node_use(MotoMaterialNode *self);

/* class SlerMaterialNode */

static GObjectClass *sler_material_node_parent_class = NULL;

struct _MotoSlerMaterialNodePriv
{
    gboolean use_external;
    GString *external_sler_project_filename;
};

static void
moto_sler_material_node_dispose(GObject *obj)
{
    MotoSlerMaterialNode *self = (MotoSlerMaterialNode *)obj;

    g_string_free(self->priv->external_sler_project_filename, TRUE);

    sler_material_node_parent_class->dispose(obj);
}

static void
moto_sler_material_node_finalize(GObject *obj)
{
    MotoSlerMaterialNode *self = (MotoSlerMaterialNode *)obj;

    g_slice_free(MotoSlerMaterialNodePriv, self->priv);

    sler_material_node_parent_class->finalize(obj);
}

static void
moto_sler_material_node_init(MotoSlerMaterialNode *self)
{
    self->priv = g_slice_new(MotoSlerMaterialNodePriv);

    // TODO: Temporary! When internal Sler editor is integrated 'use_external' default value will be FALSE.
    self->priv->use_external = TRUE;
    self->priv->external_sler_project_filename = g_string_new("");
}

static void
moto_sler_material_node_class_init(MotoSlerMaterialNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoMaterialNodeClass *mnclass = (MotoMaterialNodeClass *)klass;

    sler_material_node_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_sler_material_node_dispose;
    goclass->finalize   = moto_sler_material_node_finalize;

    mnclass->use = moto_sler_material_node_use;
}

G_DEFINE_TYPE(MotoSlerMaterialNode, moto_sler_material_node, MOTO_TYPE_MATERIAL_NODE);

/* methods of class SlerMaterialNode */

MotoSlerMaterialNode *moto_sler_material_node()
{
    MotoSlerMaterialNode *self = \
        (MotoSlerMaterialNode *)g_object_new(MOTO_TYPE_SLER_MATERIAL_NODE, NULL);

    return self;
}

static void moto_sler_material_node_use(MotoMaterialNode *self)
{
}

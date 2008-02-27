#include <GL/gl.h>

#include "moto-material-node.h"

/* class MaterialNode */

static GObjectClass *material_node_parent_class = NULL;

/*
static void
moto_material_node_dispose(GObject *obj)
{
    material_node_parent_class->dispose(obj);
}

static void
moto_material_node_finalize(GObject *obj)
{
    material_node_parent_class->finalize(obj);
}
*/

static void
moto_material_node_init(MotoMaterialNode *self)
{
}

static void
moto_material_node_class_init(MotoMaterialNodeClass *klass)
{
    // GObjectClass *goclass = (GObjectClass *)klass;

    material_node_parent_class = g_type_class_peek_parent(klass);

    /*
    goclass->dispose    = moto_material_node_dispose;
    goclass->finalize   = moto_material_node_finalize;
    */

    klass->use = NULL;
}

G_DEFINE_ABSTRACT_TYPE(MotoMaterialNode, moto_material_node, MOTO_TYPE_NODE);

/* methods of class MaterialNode */

void moto_material_node_use(MotoMaterialNode *self)
{
    MotoMaterialNodeClass *klass = MOTO_MATERIAL_NODE_GET_CLASS(self);

    if(klass->use)
        klass->use(self);
}

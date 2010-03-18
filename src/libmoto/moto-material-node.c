#include <GL/glew.h>

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

void moto_material_node_use_DEFAULT(MotoMaterialNode* self);

static void
moto_material_node_init(MotoMaterialNode *self)
{
    MotoNode* node = (MotoNode*)self;

    gfloat ambi[] = {0.1, 0.1, 0.1, 1};
    gfloat diff[] = {0.8, 0.8, 0.8, 1};
    gfloat spec[] = {0.05, 0.05, 0.05, 1};
    gfloat emis[] = {0, 0, 0, 1};
    gfloat shin = 0;

    moto_node_add_params(node,
        "ambi", "Ambient", MOTO_TYPE_FLOAT_4, MOTO_PARAM_MODE_INOUT, ambi, NULL, "Color",
        "diff", "Diffuse", MOTO_TYPE_FLOAT_4, MOTO_PARAM_MODE_INOUT, diff, NULL, "Color",
        "spec", "Specular", MOTO_TYPE_FLOAT_4, MOTO_PARAM_MODE_INOUT, spec, NULL, "Color",
        "emis", "Emission", MOTO_TYPE_FLOAT_4, MOTO_PARAM_MODE_INOUT, emis, NULL, "Color",
        "shin", "Shininess", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, shin, NULL, "Color",
        "blend", "Blend", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, FALSE, NULL, "Color",
        "material", "Material", MOTO_TYPE_MATERIAL_NODE, MOTO_PARAM_MODE_INOUT, self, NULL, "Output",
        NULL);
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

    klass->use = moto_material_node_use_DEFAULT;
}

G_DEFINE_TYPE(MotoMaterialNode, moto_material_node, MOTO_TYPE_NODE);

/* Methods of class MaterialNode */

void moto_material_node_use(MotoMaterialNode *self)
{
    MOTO_MATERIAL_NODE_GET_CLASS(self)->use(self);
}

void moto_material_node_use_DEFAULT(MotoMaterialNode* self)
{
    MotoNode* node = (MotoNode*)self;

    gfloat ambi[4], diff[4], spec[4], emis[4], shin;
    moto_node_get_param_4fv(node, "ambi", ambi);
    moto_node_get_param_4fv(node, "diff", diff);
    moto_node_get_param_4fv(node, "spec", spec);
    moto_node_get_param_4fv(node, "emis", emis);
    moto_node_get_param_float(node, "shin", &shin);

    gboolean blend;
    moto_node_get_param_boolean(node, "blend", &blend);

    if(blend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shin);
}


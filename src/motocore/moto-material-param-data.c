#include "moto-material-param-data.h"

/* class MaterialParamData */

static GObjectClass *material_param_data_parent_class = NULL;

static void
moto_material_param_data_dispose(GObject *obj)
{
    material_param_data_parent_class->dispose(obj);
}

static void
moto_material_param_data_finalize(GObject *obj)
{
    material_param_data_parent_class->finalize(obj);
}

static void
moto_material_param_data_init(MotoMaterialParamData *self)
{
    self->value = self->default_value = FALSE;
}

static void
moto_material_param_data_class_init(MotoMaterialParamDataClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    material_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose = moto_material_param_data_dispose;
    goclass->finalize = moto_material_param_data_finalize;
}

G_DEFINE_TYPE(MotoMaterialParamData, moto_material_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class MaterialParamData */

MotoParamData *
moto_material_param_data_new(MotoMaterialNode *default_value)
{
    MotoMaterialParamData *self = \
        (MotoMaterialParamData *)g_object_new(MOTO_TYPE_MATERIAL_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    self->value = self->default_value = default_value;

    return data;
}

#include "moto-float-param-data.h"

/* class FloatParamData */

static GObjectClass *float_param_data_parent_class = NULL;

static void
moto_float_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(float_param_data_parent_class)->dispose(obj);
}

static void
moto_float_param_data_finalize(GObject *obj)
{
    float_param_data_parent_class->finalize(obj);
}

static void
moto_float_param_data_init(MotoFloatParamData *self)
{
    self->value = self->default_value = 0;
}

static void
moto_float_param_data_class_init(MotoFloatParamDataClass *klass)
{
    float_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    float_param_data_parent_class->dispose = moto_float_param_data_dispose;
    float_param_data_parent_class->finalize = moto_float_param_data_finalize;
}

G_DEFINE_TYPE(MotoFloatParamData, moto_float_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class FloatParamData */

MotoParamData *
moto_float_param_data_new(MotoParamDataUpdateFunc update,
        MotoParamDataGetFunc get, MotoParamDataSetFunc set,
        gfloat default_value)
{
    MotoFloatParamData *self = \
        (MotoFloatParamData *)g_object_new(MOTO_TYPE_FLOAT_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    data->update    = update;
    data->get       = get;
    data->set       = set;

    self->value = self->default_value = default_value;

    return data;
}

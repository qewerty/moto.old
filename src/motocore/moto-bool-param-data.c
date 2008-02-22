#include "moto-bool-param-data.h"

/* class BoolParamData */

static GObjectClass *bool_param_data_parent_class = NULL;

static void
moto_bool_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(bool_param_data_parent_class)->dispose(obj);
}

static void
moto_bool_param_data_finalize(GObject *obj)
{
    bool_param_data_parent_class->finalize(obj);
}

static void
moto_bool_param_data_init(MotoBoolParamData *self)
{
    self->value = self->default_value = FALSE;
}

static void
moto_bool_param_data_class_init(MotoBoolParamDataClass *klass)
{
    bool_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    bool_param_data_parent_class->dispose = moto_bool_param_data_dispose;
    bool_param_data_parent_class->finalize = moto_bool_param_data_finalize;
}

G_DEFINE_TYPE(MotoBoolParamData, moto_bool_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class BoolParamData */

MotoParamData *
moto_bool_param_data_new(MotoParamDataUpdateFunc update,
        MotoParamDataGetFunc get, MotoParamDataSetFunc set,
        gboolean default_value)
{
    MotoBoolParamData *self = \
        (MotoBoolParamData *)g_object_new(MOTO_TYPE_BOOL_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    data->update    = update;
    data->get       = get;
    data->set       = set;

    self->value = self->default_value = default_value;

    return data;
}

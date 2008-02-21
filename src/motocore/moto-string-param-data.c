#include "moto-string-param-data.h"

/* class StringParamData */

static GObjectClass *string_param_data_parent_class = NULL;

static void
moto_string_param_data_dispose(GObject *obj)
{
    MotoStringParamData *self = (MotoStringParamData *)obj;

    g_string_free(self->value, TRUE);
    g_string_free(self->default_value, TRUE);

    string_param_data_parent_class->dispose(obj);
}

static void
moto_string_param_data_finalize(GObject *obj)
{
    string_param_data_parent_class->finalize(obj);
}

static void
moto_string_param_data_init(MotoStringParamData *self)
{
    self->value = self->default_value = NULL;
}

static void
moto_string_param_data_class_init(MotoStringParamDataClass *klass)
{
    string_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    string_param_data_parent_class->dispose = moto_string_param_data_dispose;
    string_param_data_parent_class->finalize = moto_string_param_data_finalize;
}

G_DEFINE_TYPE(MotoStringParamData, moto_string_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class StringParamData */

MotoParamData *
moto_string_param_data_new(MotoParamDataGetFunc get,
        MotoParamDataSetFunc set, const gchar *default_value)
{
    MotoStringParamData *self = \
        (MotoStringParamData *)g_object_new(MOTO_TYPE_STRING_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    data->get = get;
    data->set = set;

    self->value = g_string_new(default_value);
    self->default_value = g_string_new(default_value);

    return data;
}

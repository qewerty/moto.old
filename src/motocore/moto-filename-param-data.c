#include "moto-filename-param-data.h"

/* class FilenameParamData */

static GObjectClass *filename_param_data_parent_class = NULL;

static void
moto_filename_param_data_dispose(GObject *obj)
{
    filename_param_data_parent_class->dispose(obj);
}

static void
moto_filename_param_data_finalize(GObject *obj)
{
    filename_param_data_parent_class->finalize(obj);
}

static void
moto_filename_param_data_init(MotoFilenameParamData *self)
{
}

static void
moto_filename_param_data_class_init(MotoFilenameParamDataClass *klass)
{
    filename_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    filename_param_data_parent_class->dispose = moto_filename_param_data_dispose;
    filename_param_data_parent_class->finalize = moto_filename_param_data_finalize;
}

G_DEFINE_TYPE(MotoFilenameParamData, moto_filename_param_data, MOTO_TYPE_STRING_PARAM_DATA);

/* methods of class FilenameParamData */

MotoParamData *
moto_filename_param_data_new(MotoParamDataUpdateFunc update,
        MotoParamDataGetFunc get, MotoParamDataSetFunc set,
        const gchar *default_value)
{
    MotoFilenameParamData *self = \
        (MotoFilenameParamData *)g_object_new(MOTO_TYPE_FILENAME_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;
    MotoStringParamData *string_param_data = (MotoStringParamData *)self;

    data->update    = update;
    data->get       = get;
    data->set       = set;

    string_param_data->value = g_string_new(default_value);
    string_param_data->default_value = g_string_new(default_value);

    return data;
}

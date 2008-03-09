#include "moto-object-param-data.h"

/* class ObjectParamData */

static GObjectClass *object_param_data_parent_class = NULL;

static void
moto_object_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(object_param_data_parent_class)->dispose(obj);
}

static void
moto_object_param_data_finalize(GObject *obj)
{
    object_param_data_parent_class->finalize(obj);
}

static void
moto_object_param_data_init(MotoObjectParamData *self)
{
    self->value = self->default_value = FALSE;
}

static void
moto_object_param_data_class_init(MotoObjectParamDataClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    object_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose = moto_object_param_data_dispose;
    goclass->finalize = moto_object_param_data_finalize;
}

G_DEFINE_TYPE(MotoObjectParamData, moto_object_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class ObjectParamData */

MotoParamData *
moto_object_param_data_new(MotoObjectNode *default_value)
{
    MotoObjectParamData *self = \
        (MotoObjectParamData *)g_object_new(MOTO_TYPE_OBJECT_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    self->value = self->default_value = default_value;

    return data;
}

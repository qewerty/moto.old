#include "moto-camera-param-data.h"

/* class CameraParamData */

static GObjectClass *camera_param_data_parent_class = NULL;

static void
moto_camera_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(camera_param_data_parent_class)->dispose(obj);
}

static void
moto_camera_param_data_finalize(GObject *obj)
{
    camera_param_data_parent_class->finalize(obj);
}

static void
moto_camera_param_data_init(MotoCameraParamData *self)
{
    self->value = self->default_value = FALSE;
}

static void
moto_camera_param_data_class_init(MotoCameraParamDataClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    camera_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose = moto_camera_param_data_dispose;
    goclass->finalize = moto_camera_param_data_finalize;
}

G_DEFINE_TYPE(MotoCameraParamData, moto_camera_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class CameraParamData */

MotoParamData *
moto_camera_param_data_new(MotoCameraNode *default_value)
{
    MotoCameraParamData *self = \
        (MotoCameraParamData *)g_object_new(MOTO_TYPE_CAMERA_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    self->value = self->default_value = default_value;

    return data;
}

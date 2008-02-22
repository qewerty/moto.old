#include "moto-transform-order-param-data.h"

/* class TransformOrderParamData */

static GObjectClass *transform_order_param_data_parent_class = NULL;

static void
moto_transform_order_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(transform_order_param_data_parent_class)->dispose(obj);
}

static void
moto_transform_order_param_data_finalize(GObject *obj)
{
    transform_order_param_data_parent_class->finalize(obj);
}

static void
moto_transform_order_param_data_init(MotoTransformOrderParamData *self)
{
    self->value = self->default_value = FALSE;
}

static void
moto_transform_order_param_data_class_init(MotoTransformOrderParamDataClass *klass)
{
    transform_order_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    transform_order_param_data_parent_class->dispose = moto_transform_order_param_data_dispose;
    transform_order_param_data_parent_class->finalize = moto_transform_order_param_data_finalize;
}

G_DEFINE_TYPE(MotoTransformOrderParamData, moto_transform_order_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class TransformOrderParamData */

MotoParamData *
moto_transform_order_param_data_new(MotoTransformOrder default_value)
{
    MotoTransformOrderParamData *self = \
        (MotoTransformOrderParamData *)g_object_new(MOTO_TYPE_TRANSFORM_ORDER_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    self->value = self->default_value = default_value;

    return data;
}

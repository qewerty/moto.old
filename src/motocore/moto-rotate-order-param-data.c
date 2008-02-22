#include "moto-rotate-order-param-data.h"

/* class RotateOrderParamData */

static GObjectClass *rotate_order_param_data_parent_class = NULL;

static void
moto_rotate_order_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(rotate_order_param_data_parent_class)->dispose(obj);
}

static void
moto_rotate_order_param_data_finalize(GObject *obj)
{
    rotate_order_param_data_parent_class->finalize(obj);
}

static void
moto_rotate_order_param_data_init(MotoRotateOrderParamData *self)
{
    self->value = self->default_value = FALSE;
}

static void
moto_rotate_order_param_data_class_init(MotoRotateOrderParamDataClass *klass)
{
    rotate_order_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    rotate_order_param_data_parent_class->dispose = moto_rotate_order_param_data_dispose;
    rotate_order_param_data_parent_class->finalize = moto_rotate_order_param_data_finalize;
}

G_DEFINE_TYPE(MotoRotateOrderParamData, moto_rotate_order_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class RotateOrderParamData */

MotoParamData *
moto_rotate_order_param_data_new(MotoRotateOrder default_value)
{
    MotoRotateOrderParamData *self = \
        (MotoRotateOrderParamData *)g_object_new(MOTO_TYPE_ROTATE_ORDER_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    self->value = self->default_value = default_value;

    return data;
}

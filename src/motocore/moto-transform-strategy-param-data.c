#include "moto-transform-strategy-param-data.h"

/* class TransformStrategyParamData */

static GObjectClass *transform_strategy_param_data_parent_class = NULL;

static void
moto_transform_strategy_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(transform_strategy_param_data_parent_class)->dispose(obj);
}

static void
moto_transform_strategy_param_data_finalize(GObject *obj)
{
    transform_strategy_param_data_parent_class->finalize(obj);
}

static void
moto_transform_strategy_param_data_init(MotoTransformStrategyParamData *self)
{
    self->value = self->default_value = FALSE;
}

static void
moto_transform_strategy_param_data_class_init(MotoTransformStrategyParamDataClass *klass)
{
    transform_strategy_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    transform_strategy_param_data_parent_class->dispose = moto_transform_strategy_param_data_dispose;
    transform_strategy_param_data_parent_class->finalize = moto_transform_strategy_param_data_finalize;
}

G_DEFINE_TYPE(MotoTransformStrategyParamData, moto_transform_strategy_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class TransformStrategyParamData */

MotoParamData *
moto_transform_strategy_param_data_new(MotoParamDataGetFunc get,
        MotoParamDataSetFunc set, MotoTransformStrategy default_value)
{
    MotoTransformStrategyParamData *self = \
        (MotoTransformStrategyParamData *)g_object_new(MOTO_TYPE_TRANSFORM_STRATEGY_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    data->get = get;
    data->set = set;

    self->value = self->default_value = default_value;

    return data;
}

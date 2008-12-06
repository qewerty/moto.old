#include "moto-param-spec.h"

// MotoParamSpec

static void
moto_param_spec_init(MotoParamSpec *self)
{
}

static void
moto_param_spec_class_init(MotoParamSpecClass *klass)
{
    klass->value_type = G_TYPE_INVALID;
}

G_DEFINE_ABSTRACT_TYPE(MotoParamSpec, moto_param_spec, G_TYPE_INITIALLY_UNOWNED);

// Methods of MotoParamSpec

GType moto_param_spec_get_value_type(MotoParamSpec *self)
{
    return MOTO_PARAM_SPEC_GET_CLASS(self)->value_type;
}

// MotoParamSpecBoolean

static void
moto_param_spec_boolean_init(MotoParamSpecBoolean *self)
{
    self->default_value = FALSE;
}

static void
moto_param_spec_boolean_class_init(MotoParamSpecBooleanClass *klass)
{
    MOTO_PARAM_SPEC_CLASS(klass)->value_type = G_TYPE_BOOLEAN;
}

G_DEFINE_TYPE(MotoParamSpecBoolean, moto_param_spec_boolean, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBoolean

MotoParamSpec *moto_param_spec_boolean_new(gboolean default_value)
{
    MotoParamSpec *self = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_BOOLEAN, NULL);
    MotoParamSpecBoolean *bs = (MotoParamSpecBoolean *)self;

    bs->default_value = default_value;

    return self;
}

// MotoParamSpecInt

static void
moto_param_spec_int_init(MotoParamSpecInt *self)
{
    self->default_value = 0;
    self->min       = -1000000;
    self->max       =  1000000;
    self->step      =  1;
    self->page_step =  10;
}

static void
moto_param_spec_int_class_init(MotoParamSpecIntClass *klass)
{
    MOTO_PARAM_SPEC_CLASS(klass)->value_type = G_TYPE_INT;
}

G_DEFINE_TYPE(MotoParamSpecInt, moto_param_spec_int, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBoolean

MotoParamSpec *
moto_param_spec_int_new(gint default_value,
    gint min, gint max, gint step, gint page_step)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_INT, NULL);
    MotoParamSpecInt *is = (MotoParamSpecInt *)self;

    is->default_value = default_value;
    is->min       = min;
    is->max       = max;
    is->step      = step;
    is->page_step = page_step;

    return self;
}

// MotoParamSpecFloat

static void
moto_param_spec_float_init(MotoParamSpecFloat *self)
{
    self->default_value = 0;
    self->min       = -1000000;
    self->max       =  1000000;
    self->step      =  0.1;
    self->page_step =  1;
}

static void
moto_param_spec_float_class_init(MotoParamSpecFloatClass *klass)
{
    MOTO_PARAM_SPEC_CLASS(klass)->value_type = G_TYPE_FLOAT;
}

G_DEFINE_TYPE(MotoParamSpecFloat, moto_param_spec_float, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBoolean

MotoParamSpec *
moto_param_spec_float_new(gfloat default_value,
    gfloat min, gfloat max, gfloat step, gfloat page_step)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_FLOAT, NULL);
    MotoParamSpecFloat *fs = (MotoParamSpecFloat *)self;

    fs->default_value = default_value;
    fs->min       = min;
    fs->max       = max;
    fs->step      = step;
    fs->page_step = page_step;

    return self;
}

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

    klass->copy = NULL;
}

G_DEFINE_ABSTRACT_TYPE(MotoParamSpec, moto_param_spec, G_TYPE_OBJECT);

// Methods of MotoParamSpec

GType moto_param_spec_get_value_type(MotoParamSpec *self)
{
    return MOTO_PARAM_SPEC_GET_CLASS(self)->value_type;
}

MotoParamSpec *moto_param_spec_copy(MotoParamSpec *self)
{
    MotoParamSpecClass *klass = MOTO_PARAM_SPEC_GET_CLASS(self);

    if(klass->copy)
        return klass->copy(self);

    return NULL;
}

// MotoParamSpecBoolean

static void
moto_param_spec_boolean_init(MotoParamSpecBoolean *self)
{
    self->default_value = FALSE;
}

MotoParamSpec *moto_param_spec_boolean_copy(MotoParamSpec *self)
{
    MotoParamSpecBoolean *bs = MOTO_PARAM_SPEC_BOOLEAN(self);

    return moto_param_spec_boolean_new(bs->default_value);
}

static void
moto_param_spec_boolean_class_init(MotoParamSpecBooleanClass *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = G_TYPE_BOOLEAN;
    ps_class->copy = moto_param_spec_boolean_copy;
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

MotoParamSpec *moto_param_spec_int_copy(MotoParamSpec *self)
{
    MotoParamSpecInt *is = MOTO_PARAM_SPEC_INT(self);

    return moto_param_spec_int_new(is->default_value,
        is->min, is->max, is->step, is->page_step);
}

static void
moto_param_spec_int_class_init(MotoParamSpecIntClass *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = G_TYPE_INT;
    ps_class->copy = moto_param_spec_int_copy;
}

G_DEFINE_TYPE(MotoParamSpecInt, moto_param_spec_int, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecInt

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

MotoParamSpec *moto_param_spec_float_copy(MotoParamSpec *self)
{
    MotoParamSpecFloat *fs = MOTO_PARAM_SPEC_FLOAT(self);

    return moto_param_spec_float_new(fs->default_value,
        fs->min, fs->max, fs->step, fs->page_step);
}

static void
moto_param_spec_float_class_init(MotoParamSpecFloatClass *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = G_TYPE_FLOAT;
    ps_class->copy = moto_param_spec_float_copy;
}

G_DEFINE_TYPE(MotoParamSpecFloat, moto_param_spec_float, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecFloat

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

// MotoParamSpecBoolean_2

static void
moto_param_spec_boolean_2_init(MotoParamSpecBoolean_2 *self)
{
    self->default_value[0] = FALSE;
    self->default_value[1] = FALSE;
}

MotoParamSpec *moto_param_spec_boolean_2_copy(MotoParamSpec *self)
{
    MotoParamSpecBoolean_2 *bs2 = MOTO_PARAM_SPEC_BOOLEAN_2(self);

    return moto_param_spec_boolean_2_new(bs2->default_value[0], bs2->default_value[1]);
}

static void
moto_param_spec_boolean_2_class_init(MotoParamSpecBoolean_2Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_BOOLEAN_2;
    ps_class->copy = moto_param_spec_boolean_2_copy;
}

G_DEFINE_TYPE(MotoParamSpecBoolean_2, moto_param_spec_boolean_2, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBoolean_2

MotoParamSpec *
moto_param_spec_boolean_2_new(gboolean default_value0, gboolean default_value1)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_BOOLEAN_2, NULL);
    MotoParamSpecBoolean_2 *bs2 = (MotoParamSpecBoolean_2 *)self;

    bs2->default_value[0] = default_value0;
    bs2->default_value[1] = default_value1;

    return self;
}

// MotoParamSpecInt_2

static void
moto_param_spec_int_2_init(MotoParamSpecInt_2 *self)
{
    gint i;
    for(i = 0; i < 2; i++)
    {
        self->default_value[i] = 0;
        self->min[i]       = -1000000;
        self->max[i]       =  1000000;
        self->step[i]      =  0.1;
        self->page_step[i] =  1;
    }
}

MotoParamSpec *moto_param_spec_int_2_copy(MotoParamSpec *self)
{
    MotoParamSpecInt_2 *is2 = MOTO_PARAM_SPEC_INT_2(self);

    return moto_param_spec_int_2_new(is2->default_value[0], is2->min[0], is2->max[0], is2->step[0], is2->page_step[0],
                                     is2->default_value[1], is2->min[1], is2->max[1], is2->step[1], is2->page_step[1]);
}

static void
moto_param_spec_int_2_class_init(MotoParamSpecInt_2Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_INT_2;
    ps_class->copy = moto_param_spec_int_2_copy;
}

G_DEFINE_TYPE(MotoParamSpecInt_2, moto_param_spec_int_2, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecInt_2

MotoParamSpec *
moto_param_spec_int_2_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_INT_2, NULL);
    MotoParamSpecInt_2 *is2 = (MotoParamSpecInt_2 *)self;

    is2->default_value[0] = default_value0;
    is2->min[0]       = min0;
    is2->max[0]       = max0;
    is2->step[0]      = step0;
    is2->page_step[0] = page_step0;

    is2->default_value[1] = default_value1;
    is2->min[1]       = min1;
    is2->max[1]       = max1;
    is2->step[1]      = step1;
    is2->page_step[1] = page_step1;

    return self;
}

// MotoParamSpecFloat_2

static void
moto_param_spec_float_2_init(MotoParamSpecFloat_2 *self)
{
    gint i;
    for(i = 0; i < 2; i++)
    {
        self->default_value[i] = 0;
        self->min[i]       = -1000000;
        self->max[i]       =  1000000;
        self->step[i]      =  0.1;
        self->page_step[i] =  1;
    }
}

MotoParamSpec *moto_param_spec_float_2_copy(MotoParamSpec *self)
{
    MotoParamSpecFloat_2 *fs2 = MOTO_PARAM_SPEC_FLOAT_2(self);

    return moto_param_spec_float_2_new(fs2->default_value[0], fs2->min[0], fs2->max[0], fs2->step[0], fs2->page_step[0],
                                       fs2->default_value[1], fs2->min[1], fs2->max[1], fs2->step[1], fs2->page_step[1]);
}

static void
moto_param_spec_float_2_class_init(MotoParamSpecFloat_2Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_FLOAT_2;
    ps_class->copy = moto_param_spec_float_2_copy;
}

G_DEFINE_TYPE(MotoParamSpecFloat_2, moto_param_spec_float_2, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecFloat_2

MotoParamSpec *
moto_param_spec_float_2_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_FLOAT_2, NULL);
    MotoParamSpecFloat_2 *fs2 = (MotoParamSpecFloat_2 *)self;

    fs2->default_value[0] = default_value0;
    fs2->min[0]       = min0;
    fs2->max[0]       = max0;
    fs2->step[0]      = step0;
    fs2->page_step[0] = page_step0;

    fs2->default_value[1] = default_value1;
    fs2->min[1]       = min1;
    fs2->max[1]       = max1;
    fs2->step[1]      = step1;
    fs2->page_step[1] = page_step1;

    return self;
}

// MotoParamSpecBoolean_3

static void
moto_param_spec_boolean_3_init(MotoParamSpecBoolean_3 *self)
{
    self->default_value[0] = FALSE;
    self->default_value[1] = FALSE;
    self->default_value[2] = FALSE;
}

MotoParamSpec *moto_param_spec_boolean_3_copy(MotoParamSpec *self)
{
    MotoParamSpecBoolean_3 *bs3 = MOTO_PARAM_SPEC_BOOLEAN_3(self);

    return moto_param_spec_boolean_3_new(bs3->default_value[0], bs3->default_value[1], bs3->default_value[2]);
}

static void
moto_param_spec_boolean_3_class_init(MotoParamSpecBoolean_3Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_BOOLEAN_3;
    ps_class->copy = moto_param_spec_boolean_3_copy;
}

G_DEFINE_TYPE(MotoParamSpecBoolean_3, moto_param_spec_boolean_3, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBoolean_3

MotoParamSpec *
moto_param_spec_boolean_3_new(gboolean default_value0, gboolean default_value1, gboolean default_value2)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_BOOLEAN_3, NULL);
    MotoParamSpecBoolean_3 *bs3 = (MotoParamSpecBoolean_3 *)self;

    bs3->default_value[0] = default_value0;
    bs3->default_value[1] = default_value1;
    bs3->default_value[2] = default_value1;

    return self;
}

// MotoParamSpecInt_3

static void
moto_param_spec_int_3_init(MotoParamSpecInt_3 *self)
{
    gint i;
    for(i = 0; i < 3; i++)
    {
        self->default_value[i] = 0;
        self->min[i]       = -1000000;
        self->max[i]       =  1000000;
        self->step[i]      =  0.1;
        self->page_step[i] =  1;
    }
}

MotoParamSpec *moto_param_spec_int_3_copy(MotoParamSpec *self)
{
    MotoParamSpecInt_3 *is3 = MOTO_PARAM_SPEC_INT_3(self);

    return moto_param_spec_int_3_new(is3->default_value[0], is3->min[0], is3->max[0], is3->step[0], is3->page_step[0],
                                     is3->default_value[1], is3->min[1], is3->max[1], is3->step[1], is3->page_step[1],
                                     is3->default_value[2], is3->min[2], is3->max[2], is3->step[2], is3->page_step[2]);
}

static void
moto_param_spec_int_3_class_init(MotoParamSpecInt_3Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_INT_3;
    ps_class->copy = moto_param_spec_int_3_copy;
}

G_DEFINE_TYPE(MotoParamSpecInt_3, moto_param_spec_int_3, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecInt_3

MotoParamSpec *
moto_param_spec_int_3_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1,
                          gint default_value2, gint min2, gint max2, gint step2, gint page_step2)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_INT_3, NULL);
    MotoParamSpecInt_3 *is3 = (MotoParamSpecInt_3 *)self;

    is3->default_value[0] = default_value0;
    is3->min[0]       = min0;
    is3->max[0]       = max0;
    is3->step[0]      = step0;
    is3->page_step[0] = page_step0;

    is3->default_value[1] = default_value1;
    is3->min[1]       = min1;
    is3->max[1]       = max1;
    is3->step[1]      = step1;
    is3->page_step[1] = page_step1;

    is3->default_value[2] = default_value2;
    is3->min[2]       = min2;
    is3->max[2]       = max2;
    is3->step[2]      = step2;
    is3->page_step[2] = page_step2;

    return self;
}

// MotoParamSpecFloat_3

static void
moto_param_spec_float_3_init(MotoParamSpecFloat_3 *self)
{
    gint i;
    for(i = 0; i < 3; i++)
    {
        self->default_value[i] = 0;
        self->min[i]       = -1000000;
        self->max[i]       =  1000000;
        self->step[i]      =  0.1;
        self->page_step[i] =  1;
    }
}

MotoParamSpec *moto_param_spec_float_3_copy(MotoParamSpec *self)
{
    MotoParamSpecFloat_3 *fs3 = MOTO_PARAM_SPEC_FLOAT_3(self);

    return moto_param_spec_float_3_new(fs3->default_value[0], fs3->min[0], fs3->max[0], fs3->step[0], fs3->page_step[0],
                                       fs3->default_value[1], fs3->min[1], fs3->max[1], fs3->step[1], fs3->page_step[1],
                                       fs3->default_value[2], fs3->min[2], fs3->max[2], fs3->step[2], fs3->page_step[2]);
}

static void
moto_param_spec_float_3_class_init(MotoParamSpecFloat_3Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_FLOAT_3;
    ps_class->copy = moto_param_spec_float_3_copy;
}

G_DEFINE_TYPE(MotoParamSpecFloat_3, moto_param_spec_float_3, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecFloat_3

MotoParamSpec *
moto_param_spec_float_3_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1,
                            gfloat default_value2, gfloat min2, gfloat max2, gfloat step2, gfloat page_step2)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_FLOAT_3, NULL);
    MotoParamSpecFloat_3 *fs3 = (MotoParamSpecFloat_3 *)self;

    fs3->default_value[0] = default_value0;
    fs3->min[0]       = min0;
    fs3->max[0]       = max0;
    fs3->step[0]      = step0;
    fs3->page_step[0] = page_step0;

    fs3->default_value[1] = default_value1;
    fs3->min[1]       = min1;
    fs3->max[1]       = max1;
    fs3->step[1]      = step1;
    fs3->page_step[1] = page_step1;

    fs3->default_value[2] = default_value2;
    fs3->min[2]       = min2;
    fs3->max[2]       = max2;
    fs3->step[2]      = step2;
    fs3->page_step[2] = page_step2;

    return self;
}

// MotoParamSpecBoolean_4

static void
moto_param_spec_boolean_4_init(MotoParamSpecBoolean_4 *self)
{
    self->default_value[0] = FALSE;
    self->default_value[1] = FALSE;
    self->default_value[2] = FALSE;
    self->default_value[3] = FALSE;
}

MotoParamSpec *moto_param_spec_boolean_4_copy(MotoParamSpec *self)
{
    MotoParamSpecBoolean_4 *bs4 = MOTO_PARAM_SPEC_BOOLEAN_4(self);

    return moto_param_spec_boolean_4_new(bs4->default_value[0], bs4->default_value[1],
                                         bs4->default_value[2], bs4->default_value[3]);
}

static void
moto_param_spec_boolean_4_class_init(MotoParamSpecBoolean_4Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_BOOLEAN_4;
    ps_class->copy = moto_param_spec_boolean_4_copy;
}

G_DEFINE_TYPE(MotoParamSpecBoolean_4, moto_param_spec_boolean_4, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBoolean_4

MotoParamSpec *
moto_param_spec_boolean_4_new(gboolean default_value0, gboolean default_value1, gboolean default_value2, gboolean default_value3)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_BOOLEAN_4, NULL);
    MotoParamSpecBoolean_4 *bs4 = (MotoParamSpecBoolean_4 *)self;

    bs4->default_value[0] = default_value0;
    bs4->default_value[1] = default_value1;
    bs4->default_value[2] = default_value1;
    bs4->default_value[3] = default_value2;

    return self;
}

// MotoParamSpecInt_4

static void
moto_param_spec_int_4_init(MotoParamSpecInt_4 *self)
{
    gint i;
    for(i = 0; i < 4; i++)
    {
        self->default_value[i] = 0;
        self->min[i]       = -1000000;
        self->max[i]       =  1000000;
        self->step[i]      =  0.1;
        self->page_step[i] =  1;
    }
}

MotoParamSpec *moto_param_spec_int_4_copy(MotoParamSpec *self)
{
    MotoParamSpecInt_4 *is4 = MOTO_PARAM_SPEC_INT_4(self);

    return moto_param_spec_int_4_new(is4->default_value[0], is4->min[0], is4->max[0], is4->step[0], is4->page_step[0],
                                     is4->default_value[1], is4->min[1], is4->max[1], is4->step[1], is4->page_step[1],
                                     is4->default_value[2], is4->min[2], is4->max[2], is4->step[2], is4->page_step[2],
                                     is4->default_value[3], is4->min[3], is4->max[3], is4->step[3], is4->page_step[3]);
}

static void
moto_param_spec_int_4_class_init(MotoParamSpecInt_4Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_INT_4;
    ps_class->copy = moto_param_spec_int_4_copy;
}

G_DEFINE_TYPE(MotoParamSpecInt_4, moto_param_spec_int_4, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecInt_4

MotoParamSpec *
moto_param_spec_int_4_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1,
                          gint default_value2, gint min2, gint max2, gint step2, gint page_step2,
                          gint default_value3, gint min3, gint max3, gint step3, gint page_step3)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_INT_4, NULL);
    MotoParamSpecInt_4 *is4 = (MotoParamSpecInt_4 *)self;

    is4->default_value[0] = default_value0;
    is4->min[0]       = min0;
    is4->max[0]       = max0;
    is4->step[0]      = step0;
    is4->page_step[0] = page_step0;

    is4->default_value[1] = default_value1;
    is4->min[1]       = min1;
    is4->max[1]       = max1;
    is4->step[1]      = step1;
    is4->page_step[1] = page_step1;

    is4->default_value[2] = default_value2;
    is4->min[2]       = min2;
    is4->max[2]       = max2;
    is4->step[2]      = step2;
    is4->page_step[2] = page_step2;

    is4->default_value[3] = default_value3;
    is4->min[3]       = min3;
    is4->max[3]       = max3;
    is4->step[3]      = step3;
    is4->page_step[3] = page_step3;

    return self;
}

// MotoParamSpecFloat_4

static void
moto_param_spec_float_4_init(MotoParamSpecFloat_4 *self)
{
    gint i;
    for(i = 0; i < 4; i++)
    {
        self->default_value[i] = 0;
        self->min[i]       = -1000000;
        self->max[i]       =  1000000;
        self->step[i]      =  0.1;
        self->page_step[i] =  1;
    }
}

MotoParamSpec *moto_param_spec_float_4_copy(MotoParamSpec *self)
{
    MotoParamSpecFloat_4 *fs4 = MOTO_PARAM_SPEC_FLOAT_4(self);

    return moto_param_spec_float_4_new(fs4->default_value[0], fs4->min[0], fs4->max[0], fs4->step[0], fs4->page_step[0],
                                       fs4->default_value[1], fs4->min[1], fs4->max[1], fs4->step[1], fs4->page_step[1],
                                       fs4->default_value[2], fs4->min[2], fs4->max[2], fs4->step[2], fs4->page_step[2],
                                       fs4->default_value[3], fs4->min[3], fs4->max[3], fs4->step[3], fs4->page_step[3]);
}

static void
moto_param_spec_float_4_class_init(MotoParamSpecFloat_4Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_FLOAT_4;
    ps_class->copy = moto_param_spec_float_4_copy;
}

G_DEFINE_TYPE(MotoParamSpecFloat_4, moto_param_spec_float_4, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecFloat_4

MotoParamSpec *
moto_param_spec_float_4_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1,
                            gfloat default_value2, gfloat min2, gfloat max2, gfloat step2, gfloat page_step2,
                            gfloat default_value3, gfloat min3, gfloat max3, gfloat step3, gfloat page_step3)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_FLOAT_4, NULL);
    MotoParamSpecFloat_4 *fs4 = (MotoParamSpecFloat_4 *)self;

    fs4->default_value[0] = default_value0;
    fs4->min[0]       = min0;
    fs4->max[0]       = max0;
    fs4->step[0]      = step0;
    fs4->page_step[0] = page_step0;

    fs4->default_value[1] = default_value1;
    fs4->min[1]       = min1;
    fs4->max[1]       = max1;
    fs4->step[1]      = step1;
    fs4->page_step[1] = page_step1;

    fs4->default_value[2] = default_value2;
    fs4->min[2]       = min2;
    fs4->max[2]       = max2;
    fs4->step[2]      = step2;
    fs4->page_step[2] = page_step2;

    fs4->default_value[3] = default_value3;
    fs4->min[3]       = min3;
    fs4->max[3]       = max3;
    fs4->step[3]      = step3;
    fs4->page_step[3] = page_step3;

    return self;
}

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

// MotoParamSpecBool

static void
moto_param_spec_bool_init(MotoParamSpecBool *self)
{
    self->default_value = FALSE;
}

MotoParamSpec *moto_param_spec_boolcopy(MotoParamSpec *self)
{
    MotoParamSpecBool *bs = MOTO_PARAM_SPEC_BOOL(self);

    return moto_param_spec_bool_new(bs->default_value);
}

static void
moto_param_spec_bool_class_init(MotoParamSpecBoolClass *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = G_TYPE_BOOLEAN;
    ps_class->copy = moto_param_spec_boolcopy;
}

G_DEFINE_TYPE(MotoParamSpecBool, moto_param_spec_bool, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBool

MotoParamSpec *moto_param_spec_bool_new(gboolean default_value)
{
    MotoParamSpec *self = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_BOOL, NULL);
    MotoParamSpecBool *bs = (MotoParamSpecBool *)self;

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

MotoParamSpec *moto_param_spec_intcopy(MotoParamSpec *self)
{
    MotoParamSpecInt *is = MOTO_PARAM_SPEC_INT(self);

    return moto_param_spec_intnew(is->default_value,
        is->min, is->max, is->step, is->page_step);
}

static void
moto_param_spec_int_class_init(MotoParamSpecIntClass *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = G_TYPE_INT;
    ps_class->copy = moto_param_spec_intcopy;
}

G_DEFINE_TYPE(MotoParamSpecInt, moto_param_spec_int, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecInt

MotoParamSpec *
moto_param_spec_intnew(gint default_value,
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

MotoParamSpec *moto_param_spec_floatcopy(MotoParamSpec *self)
{
    MotoParamSpecFloat *fs = MOTO_PARAM_SPEC_FLOAT(self);

    return moto_param_spec_floatnew(fs->default_value,
        fs->min, fs->max, fs->step, fs->page_step);
}

static void
moto_param_spec_float_class_init(MotoParamSpecFloatClass *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = G_TYPE_FLOAT;
    ps_class->copy = moto_param_spec_floatcopy;
}

G_DEFINE_TYPE(MotoParamSpecFloat, moto_param_spec_float, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecFloat

MotoParamSpec *
moto_param_spec_floatnew(gfloat default_value,
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

// MotoParamSpecBool2

static void
moto_param_spec_bool2_init(MotoParamSpecBool2 *self)
{
    self->default_value[0] = FALSE;
    self->default_value[1] = FALSE;
}

MotoParamSpec *moto_param_spec_bool2_copy(MotoParamSpec *self)
{
    MotoParamSpecBool2 *bs2 = MOTO_PARAM_SPEC_BOOL2(self);

    return moto_param_spec_bool2_new(bs2->default_value[0], bs2->default_value[1]);
}

static void
moto_param_spec_bool2_class_init(MotoParamSpecBool2Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_BOOL2;
    ps_class->copy = moto_param_spec_bool2_copy;
}

G_DEFINE_TYPE(MotoParamSpecBool2, moto_param_spec_bool2, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBool2

MotoParamSpec *
moto_param_spec_bool2_new(gboolean default_value0, gboolean default_value1)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_BOOL2, NULL);
    MotoParamSpecBool2 *bs2 = (MotoParamSpecBool2 *)self;

    bs2->default_value[0] = default_value0;
    bs2->default_value[1] = default_value1;

    return self;
}

// MotoParamSpecInt2

static void
moto_param_spec_int2_init(MotoParamSpecInt2 *self)
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

MotoParamSpec *moto_param_spec_int2_copy(MotoParamSpec *self)
{
    MotoParamSpecInt2 *is2 = MOTO_PARAM_SPEC_INT2(self);

    return moto_param_spec_int2_new(is2->default_value[0], is2->min[0], is2->max[0], is2->step[0], is2->page_step[0],
                                     is2->default_value[1], is2->min[1], is2->max[1], is2->step[1], is2->page_step[1]);
}

static void
moto_param_spec_int2_class_init(MotoParamSpecInt2Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_INT2;
    ps_class->copy = moto_param_spec_int2_copy;
}

G_DEFINE_TYPE(MotoParamSpecInt2, moto_param_spec_int2, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecInt2

MotoParamSpec *
moto_param_spec_int2_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_INT2, NULL);
    MotoParamSpecInt2 *is2 = (MotoParamSpecInt2 *)self;

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

// MotoParamSpecFloat2

static void
moto_param_spec_float2_init(MotoParamSpecFloat2 *self)
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

MotoParamSpec *moto_param_spec_float2_copy(MotoParamSpec *self)
{
    MotoParamSpecFloat2 *fs2 = MOTO_PARAM_SPEC_FLOAT2(self);

    return moto_param_spec_float2_new(fs2->default_value[0], fs2->min[0], fs2->max[0], fs2->step[0], fs2->page_step[0],
                                       fs2->default_value[1], fs2->min[1], fs2->max[1], fs2->step[1], fs2->page_step[1]);
}

static void
moto_param_spec_float2_class_init(MotoParamSpecFloat2Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_FLOAT2;
    ps_class->copy = moto_param_spec_float2_copy;
}

G_DEFINE_TYPE(MotoParamSpecFloat2, moto_param_spec_float2, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecFloat2

MotoParamSpec *
moto_param_spec_float2_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_FLOAT2, NULL);
    MotoParamSpecFloat2 *fs2 = (MotoParamSpecFloat2 *)self;

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

// MotoParamSpecBool3

static void
moto_param_spec_bool3_init(MotoParamSpecBool3 *self)
{
    self->default_value[0] = FALSE;
    self->default_value[1] = FALSE;
    self->default_value[2] = FALSE;
}

MotoParamSpec *moto_param_spec_bool3_copy(MotoParamSpec *self)
{
    MotoParamSpecBool3 *bs3 = MOTO_PARAM_SPEC_BOOL3(self);

    return moto_param_spec_bool3_new(bs3->default_value[0], bs3->default_value[1], bs3->default_value[2]);
}

static void
moto_param_spec_bool3_class_init(MotoParamSpecBool3Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_BOOL3;
    ps_class->copy = moto_param_spec_bool3_copy;
}

G_DEFINE_TYPE(MotoParamSpecBool3, moto_param_spec_bool3, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBool3

MotoParamSpec *
moto_param_spec_bool3_new(gboolean default_value0, gboolean default_value1, gboolean default_value2)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_BOOL3, NULL);
    MotoParamSpecBool3 *bs3 = (MotoParamSpecBool3 *)self;

    bs3->default_value[0] = default_value0;
    bs3->default_value[1] = default_value1;
    bs3->default_value[2] = default_value1;

    return self;
}

// MotoParamSpecInt3

static void
moto_param_spec_int3_init(MotoParamSpecInt3 *self)
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

MotoParamSpec *moto_param_spec_int3_copy(MotoParamSpec *self)
{
    MotoParamSpecInt3 *is3 = MOTO_PARAM_SPEC_INT3(self);

    return moto_param_spec_int3_new(is3->default_value[0], is3->min[0], is3->max[0], is3->step[0], is3->page_step[0],
                                     is3->default_value[1], is3->min[1], is3->max[1], is3->step[1], is3->page_step[1],
                                     is3->default_value[2], is3->min[2], is3->max[2], is3->step[2], is3->page_step[2]);
}

static void
moto_param_spec_int3_class_init(MotoParamSpecInt3Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_INT3;
    ps_class->copy = moto_param_spec_int3_copy;
}

G_DEFINE_TYPE(MotoParamSpecInt3, moto_param_spec_int3, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecInt3

MotoParamSpec *
moto_param_spec_int3_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1,
                          gint default_value2, gint min2, gint max2, gint step2, gint page_step2)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_INT3, NULL);
    MotoParamSpecInt3 *is3 = (MotoParamSpecInt3 *)self;

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

// MotoParamSpecFloat3

static void
moto_param_spec_float3_init(MotoParamSpecFloat3 *self)
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

MotoParamSpec *moto_param_spec_float3_copy(MotoParamSpec *self)
{
    MotoParamSpecFloat3 *fs3 = MOTO_PARAM_SPEC_FLOAT3(self);

    return moto_param_spec_float3_new(fs3->default_value[0], fs3->min[0], fs3->max[0], fs3->step[0], fs3->page_step[0],
                                       fs3->default_value[1], fs3->min[1], fs3->max[1], fs3->step[1], fs3->page_step[1],
                                       fs3->default_value[2], fs3->min[2], fs3->max[2], fs3->step[2], fs3->page_step[2]);
}

static void
moto_param_spec_float3_class_init(MotoParamSpecFloat3Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_FLOAT3;
    ps_class->copy = moto_param_spec_float3_copy;
}

G_DEFINE_TYPE(MotoParamSpecFloat3, moto_param_spec_float3, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecFloat3

MotoParamSpec *
moto_param_spec_float3_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1,
                            gfloat default_value2, gfloat min2, gfloat max2, gfloat step2, gfloat page_step2)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_FLOAT3, NULL);
    MotoParamSpecFloat3 *fs3 = (MotoParamSpecFloat3 *)self;

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

// MotoParamSpecBool4

static void
moto_param_spec_bool4_init(MotoParamSpecBool4 *self)
{
    self->default_value[0] = FALSE;
    self->default_value[1] = FALSE;
    self->default_value[2] = FALSE;
    self->default_value[3] = FALSE;
}

MotoParamSpec *moto_param_spec_bool4_copy(MotoParamSpec *self)
{
    MotoParamSpecBool4 *bs4 = MOTO_PARAM_SPEC_BOOL4(self);

    return moto_param_spec_bool4_new(bs4->default_value[0], bs4->default_value[1],
                                         bs4->default_value[2], bs4->default_value[3]);
}

static void
moto_param_spec_bool4_class_init(MotoParamSpecBool4Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_BOOL4;
    ps_class->copy = moto_param_spec_bool4_copy;
}

G_DEFINE_TYPE(MotoParamSpecBool4, moto_param_spec_bool4, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecBool4

MotoParamSpec *
moto_param_spec_bool4_new(gboolean default_value0, gboolean default_value1, gboolean default_value2, gboolean default_value3)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_BOOL4, NULL);
    MotoParamSpecBool4 *bs4 = (MotoParamSpecBool4 *)self;

    bs4->default_value[0] = default_value0;
    bs4->default_value[1] = default_value1;
    bs4->default_value[2] = default_value1;
    bs4->default_value[3] = default_value2;

    return self;
}

// MotoParamSpecInt4

static void
moto_param_spec_int4_init(MotoParamSpecInt4 *self)
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

MotoParamSpec *moto_param_spec_int4_copy(MotoParamSpec *self)
{
    MotoParamSpecInt4 *is4 = MOTO_PARAM_SPEC_INT4(self);

    return moto_param_spec_int4_new(is4->default_value[0], is4->min[0], is4->max[0], is4->step[0], is4->page_step[0],
                                     is4->default_value[1], is4->min[1], is4->max[1], is4->step[1], is4->page_step[1],
                                     is4->default_value[2], is4->min[2], is4->max[2], is4->step[2], is4->page_step[2],
                                     is4->default_value[3], is4->min[3], is4->max[3], is4->step[3], is4->page_step[3]);
}

static void
moto_param_spec_int4_class_init(MotoParamSpecInt4Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_INT4;
    ps_class->copy = moto_param_spec_int4_copy;
}

G_DEFINE_TYPE(MotoParamSpecInt4, moto_param_spec_int4, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecInt4

MotoParamSpec *
moto_param_spec_int4_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1,
                          gint default_value2, gint min2, gint max2, gint step2, gint page_step2,
                          gint default_value3, gint min3, gint max3, gint step3, gint page_step3)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_INT4, NULL);
    MotoParamSpecInt4 *is4 = (MotoParamSpecInt4 *)self;

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

// MotoParamSpecFloat4

static void
moto_param_spec_float4_init(MotoParamSpecFloat4 *self)
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

MotoParamSpec *moto_param_spec_float4_copy(MotoParamSpec *self)
{
    MotoParamSpecFloat4 *fs4 = MOTO_PARAM_SPEC_FLOAT4(self);

    return moto_param_spec_float4_new(fs4->default_value[0], fs4->min[0], fs4->max[0], fs4->step[0], fs4->page_step[0],
                                       fs4->default_value[1], fs4->min[1], fs4->max[1], fs4->step[1], fs4->page_step[1],
                                       fs4->default_value[2], fs4->min[2], fs4->max[2], fs4->step[2], fs4->page_step[2],
                                       fs4->default_value[3], fs4->min[3], fs4->max[3], fs4->step[3], fs4->page_step[3]);
}

static void
moto_param_spec_float4_class_init(MotoParamSpecFloat4Class *klass)
{
    MotoParamSpecClass *ps_class = MOTO_PARAM_SPEC_CLASS(klass);

    ps_class->value_type = MOTO_TYPE_FLOAT4;
    ps_class->copy = moto_param_spec_float4_copy;
}

G_DEFINE_TYPE(MotoParamSpecFloat4, moto_param_spec_float4, MOTO_TYPE_PARAM_SPEC);

// Methods of MotoParamSpecFloat4

MotoParamSpec *
moto_param_spec_float4_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1,
                            gfloat default_value2, gfloat min2, gfloat max2, gfloat step2, gfloat page_step2,
                            gfloat default_value3, gfloat min3, gfloat max3, gfloat step3, gfloat page_step3)
{
    MotoParamSpec *self  = (MotoParamSpec *)g_object_new(MOTO_TYPE_PARAM_SPEC_FLOAT4, NULL);
    MotoParamSpecFloat4 *fs4 = (MotoParamSpecFloat4 *)self;

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

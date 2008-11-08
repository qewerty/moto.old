#include <string.h>

#include <gobject/gvaluecollector.h>

#include "moto-types.h"

static gboolean int_zero_array[4];
static gint     int_zero_array[4];
static guint    uint_zero_array[4];
static glong    long_zero_array[4];
static gulong   ulong_zero_array[4];
static gint64   int64_zero_array[4];
static guint64  uint64_zero_array[4];
static gfloat   float_zero_array[4];
static gdouble  double_zero_array[4];

typedef struct _MotoBoolean_2 MotoBoolean_2;
typedef struct _MotoBoolean_3 MotoBoolean_3;
typedef struct _MotoBoolean_4 MotoBoolean_4;

typedef struct _MotoInt_2 MotoInt_2;
typedef struct _MotoInt_3 MotoInt_3;
typedef struct _MotoInt_4 MotoInt_4;

typedef struct _MotoUint_2 MotoUint_2;
typedef struct _MotoUint_3 MotoUint_3;
typedef struct _MotoUint_4 MotoUint_4;

typedef struct _MotoLong_2 MotoLong_2;
typedef struct _MotoLong_3 MotoLong_3;
typedef struct _MotoLong_4 MotoLong_4;

typedef struct _MotoUlong_2 MotoUlong_2;
typedef struct _MotoUlong_3 MotoUlong_3;
typedef struct _MotoUlong_4 MotoUlong_4;

typedef struct _MotoInt64_2 MotoInt64_2;
typedef struct _MotoInt64_3 MotoInt64_3;
typedef struct _MotoInt64_4 MotoInt64_4;

typedef struct _MotoUint64_2 MotoUint64_2;
typedef struct _MotoUint64_3 MotoUint64_3;
typedef struct _MotoUint64_4 MotoUint64_4;

typedef struct _MotoFloat_2 MotoFloat_2;
typedef struct _MotoFloat_3 MotoFloat_3;
typedef struct _MotoFloat_4 MotoFloat_4;

typedef struct _MotoDouble_2 MotoDouble_2;
typedef struct _MotoDouble_3 MotoDouble_3;
typedef struct _MotoDouble_4 MotoDouble_4;

typedef struct _MotoBoolean_2x2 MotoBoolean_2x2;
typedef struct _MotoBoolean_3x3 MotoBoolean_3x3;
typedef struct _MotoBoolean_4x4 MotoBoolean_4x4;

typedef struct _MotoInt_2x2 MotoInt_2x2;
typedef struct _MotoInt_3x3 MotoInt_3x3;
typedef struct _MotoInt_4x4 MotoInt_4x4;

typedef struct _MotoUint_2x2 MotoUint_2x2;
typedef struct _MotoUint_3x3 MotoUint_3x3;
typedef struct _MotoUint_4x4 MotoUint_4x4;

typedef struct _MotoLong_2x2 MotoLong_2x2;
typedef struct _MotoLong_3x3 MotoLong_3x3;
typedef struct _MotoLong_4x4 MotoLong_4x4;

typedef struct _MotoUlong_2x2 MotoUlong_2x2;
typedef struct _MotoUlong_3x3 MotoUlong_3x3;
typedef struct _MotoUlong_4x4 MotoUlong_4x4;

typedef struct _MotoInt64_2x2 MotoInt64_2x2;
typedef struct _MotoInt64_3x3 MotoInt64_3x3;
typedef struct _MotoInt64_4x4 MotoInt64_4x4;

typedef struct _MotoUint64_2x2 MotoUint64_2x2;
typedef struct _MotoUint64_3x3 MotoUint64_3x3;
typedef struct _MotoUint64_4x4 MotoUint64_4x4;

typedef struct _MotoFloat_2x2 MotoFloat_2x2;
typedef struct _MotoFloat_3x3 MotoFloat_3x3;
typedef struct _MotoFloat_4x4 MotoFloat_4x4;

typedef struct _MotoDouble_2x2 MotoDouble_2x2;
typedef struct _MotoDouble_3x3 MotoDouble_3x3;
typedef struct _MotoDouble_4x4 MotoDouble_4x4;

void __moto_transform_float_2_to_float_1(GValue *src_value, GValue *dest_value);
void __moto_transform_float_3_to_float_1(GValue *src_value, GValue *dest_value);
void __moto_transform_float_4_to_float_1(GValue *src_value, GValue *dest_value);

// MotoFloat_1 -> *

void __moto_transform_float_1_to_boolean_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_boolean_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_boolean_4(GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_int_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_int_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_int_4(GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_uint_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_uint_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_uint_4(GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_long_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_long_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_long_4(GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_ulong_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_ulong_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_ulong_4(GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_int64_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_int64_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_int64_4(GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_uint64_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_uint64_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_uint64_4(GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_float_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_float_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_float_4(GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_double_2(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_double_3(GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_double_4(GValue *src_value, GValue *dest_value);

void moto_types_init(void)
{
    g_value_register_transform_func(MOTO_TYPE_FLOAT_2, MOTO_TYPE_FLOAT_1, __moto_transform_float_2_to_float_1);
    /*
    g_value_register_transform_func(MOTO_TYPE_FLOAT_3, MOTO_TYPE_FLOAT_1, __moto_transform_float_3_to_float_1);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_4, MOTO_TYPE_FLOAT_1, __moto_transform_float_4_to_float_1);
    */

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_FLOAT_2, __moto_transform_float_1_to_float_2);
}

// MOTO_TYPE_FLOAT_2

struct _MotoFloat_2
{
    gfloat data[2];
};

static void __moto_float_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoFloat_2);
    memcpy(value->data[0].v_pointer, float_zero_array, sizeof(MotoFloat_2));
}

static void __moto_float_2_value_free(GValue *value)
{
    g_slice_free(MotoFloat_2, value->data[0].v_pointer);
}

static void __moto_float_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoFloat_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoFloat_2));
}

static gpointer __moto_float_2_value_peek_pointer(GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_float_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    memcpy(value->data[0].v_pointer, collect_values[0].v_pointer, sizeof(gfloat) << 1);

    return NULL;
}

static gchar *__moto_float_2_lcopy_value(GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{

    return NULL;
}

GType moto_float_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_float_2_value_init,          /* value_init */
            __moto_float_2_value_free,          /* value_free */
            __moto_float_2_value_copy,          /* value_copy */
            __moto_float_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_float_2_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_float_2_lcopy_value,         /* lcopy_value */
        };

        GTypeInfo info = \
        {
            0,              /* class_size */
            NULL,           /* base_init */
            NULL,           /* base_destroy */
            NULL,           /* class_init */
            NULL,           /* class_destroy */
            NULL,           /* class_data */
            0,              /* instance_size */
            0,              /* n_preallocs */
            NULL,           /* instance_init */
            & value_table,  /* value_table */
        };

        GTypeFundamentalInfo f_info = {0,};

        GType type_id = g_type_fundamental_next();
        if( ! type_id)
        {
            // TODO
        }
        type = g_type_register_fundamental(type_id, "MotoFloat_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_FLOAT_3

struct _MotoFloat_3
{
    gfloat data[3];
};

static void __moto_float_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoFloat_3);
    memcpy(value->data[0].v_pointer, float_zero_array, sizeof(MotoFloat_3));
}

static void __moto_float_3_value_free(GValue *value)
{
    g_slice_free(MotoFloat_3, value->data[0].v_pointer);
}

static void __moto_float_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoFloat_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoFloat_3));
}

static gpointer __moto_float_3_value_peek_pointer(GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_float_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    memcpy(value->data[0].v_pointer, collect_values[0].v_pointer, sizeof(gfloat) << 1);

    return NULL;
}

static gchar *__moto_float_3_lcopy_value(GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{

    return NULL;
}

GType moto_float_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_float_3_value_init,          /* value_init */
            __moto_float_3_value_free,          /* value_free */
            __moto_float_3_value_copy,          /* value_copy */
            __moto_float_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_float_3_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_float_3_lcopy_value,         /* lcopy_value */
        };

        GTypeInfo info = \
        {
            0,              /* class_size */
            NULL,           /* base_init */
            NULL,           /* base_destroy */
            NULL,           /* class_init */
            NULL,           /* class_destroy */
            NULL,           /* class_data */
            0,              /* instance_size */
            0,              /* n_preallocs */
            NULL,           /* instance_init */
            & value_table,  /* value_table */
        };

        GTypeFundamentalInfo f_info = {0,};

        GType type_id = g_type_fundamental_next();
        if( ! type_id)
        {
            // TODO
        }
        type = g_type_register_fundamental(type_id, "MotoFloat_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_FLOAT_4

struct _MotoFloat_4
{
    gfloat data[4];
};

static void __moto_float_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoFloat_4);
    memcpy(value->data[0].v_pointer, float_zero_array, sizeof(MotoFloat_4));
}

static void __moto_float_4_value_free(GValue *value)
{
    g_slice_free(MotoFloat_4, value->data[0].v_pointer);
}

static void __moto_float_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoFloat_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoFloat_4));
}

static gpointer __moto_float_4_value_peek_pointer(GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_float_4_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    memcpy(value->data[0].v_pointer, collect_values[0].v_pointer, sizeof(gfloat) << 1);

    return NULL;
}

static gchar *__moto_float_4_lcopy_value(GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{

    return NULL;
}

GType moto_float_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_float_4_value_init,          /* value_init */
            __moto_float_4_value_free,          /* value_free */
            __moto_float_4_value_copy,          /* value_copy */
            __moto_float_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_float_4_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_float_4_lcopy_value,         /* lcopy_value */
        };

        GTypeInfo info = \
        {
            0,              /* class_size */
            NULL,           /* base_init */
            NULL,           /* base_destroy */
            NULL,           /* class_init */
            NULL,           /* class_destroy */
            NULL,           /* class_data */
            0,              /* instance_size */
            0,              /* n_preallocs */
            NULL,           /* instance_init */
            & value_table,  /* value_table */
        };

        GTypeFundamentalInfo f_info = {0,};

        GType type_id = g_type_fundamental_next();
        if( ! type_id)
        {
            // TODO
        }
        type = g_type_register_fundamental(type_id, "MotoFloat_4", & info, & f_info, 0);
    }
    return type;
}

// Transformation functions

void __moto_transform_float_2_to_float_1(GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1]) / 2);
}

void __moto_transform_float_3_to_float_1(GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1] + src[2]) / 3);
}

void __moto_transform_float_4_to_float_1(GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1] + src[2] + src[3]) / 4);
}

void __moto_transform_float_1_to_float_2(GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_2);
    g_print("dest: %p\n", dest);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_float_3(GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_float_4(GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

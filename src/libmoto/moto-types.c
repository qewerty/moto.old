#include <string.h>
#include <math.h>

#include <gobject/gvaluecollector.h>

#include "moto-types.h"

static gboolean boolean_zero_array[4];
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

void __moto_transform_float_2_to_float_1(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_3_to_float_1(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_4_to_float_1(const GValue *src_value, GValue *dest_value);

// MotoFloat_1 -> *

void __moto_transform_float_1_to_boolean_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_boolean_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_boolean_4(const GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_int_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_int_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_int_4(const GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_uint_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_uint_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_uint_4(const GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_long_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_long_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_long_4(const GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_ulong_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_ulong_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_ulong_4(const GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_int64_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_int64_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_int64_4(const GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_uint64_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_uint64_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_uint64_4(const GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_float_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_float_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_float_4(const GValue *src_value, GValue *dest_value);

void __moto_transform_float_1_to_double_2(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_double_3(const GValue *src_value, GValue *dest_value);
void __moto_transform_float_1_to_double_4(const GValue *src_value, GValue *dest_value);

void moto_types_init(void)
{
    // 1 -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_BOOLEAN_2, __moto_transform_float_1_to_boolean_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_BOOLEAN_3, __moto_transform_float_1_to_boolean_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_BOOLEAN_4, __moto_transform_float_1_to_boolean_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT_2, __moto_transform_float_1_to_int_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT_3, __moto_transform_float_1_to_int_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT_4, __moto_transform_float_1_to_int_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_UINT_2, __moto_transform_float_1_to_uint_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_UINT_3, __moto_transform_float_1_to_uint_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_UINT_4, __moto_transform_float_1_to_uint_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_LONG_2, __moto_transform_float_1_to_long_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_LONG_3, __moto_transform_float_1_to_long_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_LONG_4, __moto_transform_float_1_to_long_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_ULONG_2, __moto_transform_float_1_to_ulong_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_ULONG_3, __moto_transform_float_1_to_ulong_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_ULONG_4, __moto_transform_float_1_to_ulong_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT64_2, __moto_transform_float_1_to_int64_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT64_3, __moto_transform_float_1_to_int64_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT64_4, __moto_transform_float_1_to_int64_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_UINT64_2, __moto_transform_float_1_to_uint64_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_UINT64_3, __moto_transform_float_1_to_uint64_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_UINT64_4, __moto_transform_float_1_to_uint64_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_FLOAT_2, __moto_transform_float_1_to_float_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_FLOAT_3, __moto_transform_float_1_to_float_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_FLOAT_4, __moto_transform_float_1_to_float_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_DOUBLE_2, __moto_transform_float_1_to_double_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_DOUBLE_3, __moto_transform_float_1_to_double_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_DOUBLE_4, __moto_transform_float_1_to_double_4);

    /*
    // 2 -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT_2, MOTO_TYPE_FLOAT_1, __moto_transform_float_2_to_float_1);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_2, MOTO_TYPE_FLOAT_3, __moto_transform_float_2_to_float_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_2, MOTO_TYPE_FLOAT_4, __moto_transform_float_2_to_float_4);

    // 3 -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT_3, MOTO_TYPE_FLOAT_1, __moto_transform_float_3_to_float_1);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_3, MOTO_TYPE_FLOAT_2, __moto_transform_float_3_to_float_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_3, MOTO_TYPE_FLOAT_4, __moto_transform_float_3_to_float_4);

    // 4 -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT_4, MOTO_TYPE_FLOAT_1, __moto_transform_float_4_to_float_1);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_4, MOTO_TYPE_FLOAT_2, __moto_transform_float_4_to_float_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_4, MOTO_TYPE_FLOAT_3, __moto_transform_float_4_to_float_3);
    */

}

// MOTO_TYPE_BOOLEAN_2

struct _MotoBoolean_2
{
    gboolean data[2];
};

static void __moto_boolean_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoBoolean_2);
    memcpy(value->data[0].v_pointer, boolean_zero_array, sizeof(MotoBoolean_2));
}

static void __moto_boolean_2_value_free(GValue *value)
{
    g_slice_free(MotoBoolean_2, value->data[0].v_pointer);
}

static void __moto_boolean_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoBoolean_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoBoolean_2));
}

static gpointer __moto_boolean_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_boolean_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_boolean_2_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_boolean_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_boolean_2_value_init,          /* value_init */
            __moto_boolean_2_value_free,          /* value_free */
            __moto_boolean_2_value_copy,          /* value_copy */
            __moto_boolean_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_boolean_2_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_boolean_2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoBoolean_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_BOOLEAN_3

struct _MotoBoolean_3
{
    gboolean data[3];
};

static void __moto_boolean_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoBoolean_3);
    memcpy(value->data[0].v_pointer, boolean_zero_array, sizeof(MotoBoolean_3));
}

static void __moto_boolean_3_value_free(GValue *value)
{
    g_slice_free(MotoBoolean_3, value->data[0].v_pointer);
}

static void __moto_boolean_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoBoolean_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoBoolean_3));
}

static gpointer __moto_boolean_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_boolean_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_boolean_3_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_boolean_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_boolean_3_value_init,          /* value_init */
            __moto_boolean_3_value_free,          /* value_free */
            __moto_boolean_3_value_copy,          /* value_copy */
            __moto_boolean_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_boolean_3_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_boolean_3_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoBoolean_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_BOOLEAN_4

struct _MotoBoolean_4
{
    gboolean data[4];
};

static void __moto_boolean_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoBoolean_4);
    memcpy(value->data[0].v_pointer, boolean_zero_array, sizeof(MotoBoolean_4));
}

static void __moto_boolean_4_value_free(GValue *value)
{
    g_slice_free(MotoBoolean_4, value->data[0].v_pointer);
}

static void __moto_boolean_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoBoolean_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoBoolean_4));
}

static gpointer __moto_boolean_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_boolean_4_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_boolean_4_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_boolean_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_boolean_4_value_init,          /* value_init */
            __moto_boolean_4_value_free,          /* value_free */
            __moto_boolean_4_value_copy,          /* value_copy */
            __moto_boolean_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_boolean_4_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_boolean_4_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoBoolean_4", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_INT_2

struct _MotoInt_2
{
    gint data[2];
};

static void __moto_int_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoInt_2);
    memcpy(value->data[0].v_pointer, int_zero_array, sizeof(MotoInt_2));
}

static void __moto_int_2_value_free(GValue *value)
{
    g_slice_free(MotoInt_2, value->data[0].v_pointer);
}

static void __moto_int_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoInt_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt_2));
}

static gpointer __moto_int_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_int_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_int_2_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_int_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_int_2_value_init,          /* value_init */
            __moto_int_2_value_free,          /* value_free */
            __moto_int_2_value_copy,          /* value_copy */
            __moto_int_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_int_2_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_int_2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoInt_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_INT_3

struct _MotoInt_3
{
    gint data[3];
};

static void __moto_int_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoInt_3);
    memcpy(value->data[0].v_pointer, int_zero_array, sizeof(MotoInt_3));
}

static void __moto_int_3_value_free(GValue *value)
{
    g_slice_free(MotoInt_3, value->data[0].v_pointer);
}

static void __moto_int_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoInt_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt_3));
}

static gpointer __moto_int_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_int_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_int_3_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_int_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_int_3_value_init,          /* value_init */
            __moto_int_3_value_free,          /* value_free */
            __moto_int_3_value_copy,          /* value_copy */
            __moto_int_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_int_3_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_int_3_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoInt_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_INT_4

struct _MotoInt_4
{
    gint data[4];
};

static void __moto_int_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoInt_4);
    memcpy(value->data[0].v_pointer, int_zero_array, sizeof(MotoInt_4));
}

static void __moto_int_4_value_free(GValue *value)
{
    g_slice_free(MotoInt_4, value->data[0].v_pointer);
}

static void __moto_int_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoInt_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt_4));
}

static gpointer __moto_int_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_int_4_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_int_4_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_int_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_int_4_value_init,          /* value_init */
            __moto_int_4_value_free,          /* value_free */
            __moto_int_4_value_copy,          /* value_copy */
            __moto_int_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_int_4_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_int_4_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoInt_4", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_UINT_2

struct _MotoUint_2
{
    guint data[2];
};

static void __moto_uint_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUint_2);
    memcpy(value->data[0].v_pointer, uint_zero_array, sizeof(MotoUint_2));
}

static void __moto_uint_2_value_free(GValue *value)
{
    g_slice_free(MotoUint_2, value->data[0].v_pointer);
}

static void __moto_uint_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUint_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUint_2));
}

static gpointer __moto_uint_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_uint_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_uint_2_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_uint_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_uint_2_value_init,          /* value_init */
            __moto_uint_2_value_free,          /* value_free */
            __moto_uint_2_value_copy,          /* value_copy */
            __moto_uint_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_uint_2_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_uint_2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUint_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_UINT_3

struct _MotoUint_3
{
    guint data[3];
};

static void __moto_uint_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUint_3);
    memcpy(value->data[0].v_pointer, uint_zero_array, sizeof(MotoUint_3));
}

static void __moto_uint_3_value_free(GValue *value)
{
    g_slice_free(MotoUint_3, value->data[0].v_pointer);
}

static void __moto_uint_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUint_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUint_3));
}

static gpointer __moto_uint_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_uint_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_uint_3_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_uint_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_uint_3_value_init,          /* value_init */
            __moto_uint_3_value_free,          /* value_free */
            __moto_uint_3_value_copy,          /* value_copy */
            __moto_uint_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_uint_3_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_uint_3_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUint_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_UINT_4

struct _MotoUint_4
{
    guint data[4];
};

static void __moto_uint_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUint_4);
    memcpy(value->data[0].v_pointer, uint_zero_array, sizeof(MotoUint_4));
}

static void __moto_uint_4_value_free(GValue *value)
{
    g_slice_free(MotoUint_4, value->data[0].v_pointer);
}

static void __moto_uint_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUint_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUint_4));
}

static gpointer __moto_uint_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_uint_4_collect_value(GValue       *value,
                                          guint         n_collect_values,
                                          GTypeCValue  *collect_values,
                                          guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_uint_4_lcopy_value(const GValue *value,
                                        guint         n_collect_values,
                                        GTypeCValue  *collect_values,
                                        guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_uint_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_uint_4_value_init,          /* value_init */
            __moto_uint_4_value_free,          /* value_free */
            __moto_uint_4_value_copy,          /* value_copy */
            __moto_uint_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_uint_4_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_uint_4_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUint_4", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_LONG_2

struct _MotoLong_2
{
    glong data[2];
};

static void __moto_long_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoLong_2);
    memcpy(value->data[0].v_pointer, long_zero_array, sizeof(MotoLong_2));
}

static void __moto_long_2_value_free(GValue *value)
{
    g_slice_free(MotoLong_2, value->data[0].v_pointer);
}

static void __moto_long_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoLong_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoLong_2));
}

static gpointer __moto_long_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_long_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_long_2_lcopy_value(const GValue       *value,
                                        guint         n_collect_values,
                                        GTypeCValue  *collect_values,
                                        guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_long_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_long_2_value_init,          /* value_init */
            __moto_long_2_value_free,          /* value_free */
            __moto_long_2_value_copy,          /* value_copy */
            __moto_long_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                               /* collect_format */
            __moto_long_2_collect_value,       /* collect_value */
            "p",                               /* lcopy_format */
            __moto_long_2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoLong_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_LONG_3

struct _MotoLong_3
{
    glong data[3];
};

static void __moto_long_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoLong_3);
    memcpy(value->data[0].v_pointer, long_zero_array, sizeof(MotoLong_3));
}

static void __moto_long_3_value_free(GValue *value)
{
    g_slice_free(MotoLong_3, value->data[0].v_pointer);
}

static void __moto_long_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoLong_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoLong_3));
}

static gpointer __moto_long_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_long_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_long_3_lcopy_value(const GValue       *value,
                                        guint         n_collect_values,
                                        GTypeCValue  *collect_values,
                                        guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_long_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_long_3_value_init,          /* value_init */
            __moto_long_3_value_free,          /* value_free */
            __moto_long_3_value_copy,          /* value_copy */
            __moto_long_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                               /* collect_format */
            __moto_long_3_collect_value,       /* collect_value */
            "p",                               /* lcopy_format */
            __moto_long_3_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoLong_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_LONG_4

struct _MotoLong_4
{
    glong data[4];
};

static void __moto_long_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoLong_4);
    memcpy(value->data[0].v_pointer, long_zero_array, sizeof(MotoLong_4));
}

static void __moto_long_4_value_free(GValue *value)
{
    g_slice_free(MotoInt_4, value->data[0].v_pointer);
}

static void __moto_long_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoLong_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoLong_4));
}

static gpointer __moto_long_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_long_4_collect_value(GValue       *value,
                                           guint        n_collect_values,
                                           GTypeCValue *collect_values,
                                           guint        collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_long_4_lcopy_value(const GValue       *value,
                                        guint         n_collect_values,
                                        GTypeCValue  *collect_values,
                                        guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_long_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_long_4_value_init,          /* value_init */
            __moto_long_4_value_free,          /* value_free */
            __moto_long_4_value_copy,          /* value_copy */
            __moto_long_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                               /* collect_format */
            __moto_long_4_collect_value,       /* collect_value */
            "p",                               /* lcopy_format */
            __moto_long_4_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoLong_4", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_ULONG_2

struct _MotoUlong_2
{
    gulong data[2];
};

static void __moto_ulong_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUlong_2);
    memcpy(value->data[0].v_pointer, ulong_zero_array, sizeof(MotoUlong_2));
}

static void __moto_ulong_2_value_free(GValue *value)
{
    g_slice_free(MotoUlong_2, value->data[0].v_pointer);
}

static void __moto_ulong_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUlong_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUlong_2));
}

static gpointer __moto_ulong_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_ulong_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_ulong_2_lcopy_value(const GValue       *value,
                                        guint         n_collect_values,
                                        GTypeCValue  *collect_values,
                                        guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_ulong_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_ulong_2_value_init,          /* value_init */
            __moto_ulong_2_value_free,          /* value_free */
            __moto_ulong_2_value_copy,          /* value_copy */
            __moto_ulong_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                               /* collect_format */
            __moto_ulong_2_collect_value,       /* collect_value */
            "p",                               /* lcopy_format */
            __moto_ulong_2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUlong_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_ULONG_3

struct _MotoUlong_3
{
    gulong data[3];
};

static void __moto_ulong_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUlong_3);
    memcpy(value->data[0].v_pointer, ulong_zero_array, sizeof(MotoUlong_3));
}

static void __moto_ulong_3_value_free(GValue *value)
{
    g_slice_free(MotoUlong_3, value->data[0].v_pointer);
}

static void __moto_ulong_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUlong_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUlong_3));
}

static gpointer __moto_ulong_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_ulong_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_ulong_3_lcopy_value(const GValue       *value,
                                        guint         n_collect_values,
                                        GTypeCValue  *collect_values,
                                        guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_ulong_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_ulong_3_value_init,          /* value_init */
            __moto_ulong_3_value_free,          /* value_free */
            __moto_ulong_3_value_copy,          /* value_copy */
            __moto_ulong_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                               /* collect_format */
            __moto_ulong_3_collect_value,       /* collect_value */
            "p",                               /* lcopy_format */
            __moto_ulong_3_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUlong_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_ULONG_4

struct _MotoUlong_4
{
    gulong data[4];
};

static void __moto_ulong_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUlong_4);
    memcpy(value->data[0].v_pointer, ulong_zero_array, sizeof(MotoUlong_4));
}

static void __moto_ulong_4_value_free(GValue *value)
{
    g_slice_free(MotoInt_4, value->data[0].v_pointer);
}

static void __moto_ulong_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUlong_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUlong_4));
}

static gpointer __moto_ulong_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_ulong_4_collect_value(GValue       *value,
                                           guint        n_collect_values,
                                           GTypeCValue *collect_values,
                                           guint        collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_ulong_4_lcopy_value(const GValue       *value,
                                        guint         n_collect_values,
                                        GTypeCValue  *collect_values,
                                        guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_ulong_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_ulong_4_value_init,          /* value_init */
            __moto_ulong_4_value_free,          /* value_free */
            __moto_ulong_4_value_copy,          /* value_copy */
            __moto_ulong_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                               /* collect_format */
            __moto_ulong_4_collect_value,       /* collect_value */
            "p",                               /* lcopy_format */
            __moto_ulong_4_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUlong_4", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_INT64_2

struct _MotoInt64_2
{
    gint64 data[2];
};

static void __moto_int64_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoInt64_2);
    memcpy(value->data[0].v_pointer, int64_zero_array, sizeof(MotoInt64_2));
}

static void __moto_int64_2_value_free(GValue *value)
{
    g_slice_free(MotoInt64_2, value->data[0].v_pointer);
}

static void __moto_int64_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoInt64_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt64_2));
}

static gpointer __moto_int64_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_int64_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_int64_2_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_int64_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_int64_2_value_init,          /* value_init */
            __moto_int64_2_value_free,          /* value_free */
            __moto_int64_2_value_copy,          /* value_copy */
            __moto_int64_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_int64_2_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_int64_2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoInt64_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_INT64_3

struct _MotoInt64_3
{
    gint64 data[3];
};

static void __moto_int64_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoInt64_3);
    memcpy(value->data[0].v_pointer, int64_zero_array, sizeof(MotoInt64_3));
}

static void __moto_int64_3_value_free(GValue *value)
{
    g_slice_free(MotoInt64_3, value->data[0].v_pointer);
}

static void __moto_int64_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoInt64_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt64_3));
}

static gpointer __moto_int64_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_int64_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_int64_3_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_int64_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_int64_3_value_init,          /* value_init */
            __moto_int64_3_value_free,          /* value_free */
            __moto_int64_3_value_copy,          /* value_copy */
            __moto_int64_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_int64_3_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_int64_3_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoInt64_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_INT64_4

struct _MotoInt64_4
{
    gint64 data[4];
};

static void __moto_int64_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoInt64_4);
    memcpy(value->data[0].v_pointer, int64_zero_array, sizeof(MotoInt64_4));
}

static void __moto_int64_4_value_free(GValue *value)
{
    g_slice_free(MotoInt64_4, value->data[0].v_pointer);
}

static void __moto_int64_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoInt64_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt64_4));
}

static gpointer __moto_int64_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_int64_4_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_int64_4_lcopy_value(const GValue  *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_int64_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_int64_4_value_init,          /* value_init */
            __moto_int64_4_value_free,          /* value_free */
            __moto_int64_4_value_copy,          /* value_copy */
            __moto_int64_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_int64_4_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_int64_4_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoInt64_4", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_UINT64_2

struct _MotoUint64_2
{
    guint64 data[2];
};

static void __moto_uint64_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUint64_2);
    memcpy(value->data[0].v_pointer, uint64_zero_array, sizeof(MotoUint64_2));
}

static void __moto_uint64_2_value_free(GValue *value)
{
    g_slice_free(MotoUint64_2, value->data[0].v_pointer);
}

static void __moto_uint64_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUint64_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUint64_2));
}

static gpointer __moto_uint64_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_uint64_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_uint64_2_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_uint64_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_uint64_2_value_init,          /* value_init */
            __moto_uint64_2_value_free,          /* value_free */
            __moto_uint64_2_value_copy,          /* value_copy */
            __moto_uint64_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_uint64_2_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_uint64_2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUint64_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_UINT64_3

struct _MotoUint64_3
{
    guint64 data[3];
};

static void __moto_uint64_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUint64_3);
    memcpy(value->data[0].v_pointer, uint64_zero_array, sizeof(MotoUint64_3));
}

static void __moto_uint64_3_value_free(GValue *value)
{
    g_slice_free(MotoUint64_3, value->data[0].v_pointer);
}

static void __moto_uint64_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUint64_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUint64_3));
}

static gpointer __moto_uint64_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_uint64_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_uint64_3_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_uint64_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_uint64_3_value_init,          /* value_init */
            __moto_uint64_3_value_free,          /* value_free */
            __moto_uint64_3_value_copy,          /* value_copy */
            __moto_uint64_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_uint64_3_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_uint64_3_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUint64_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_UINT64_4

struct _MotoUint64_4
{
    guint64 data[4];
};

static void __moto_uint64_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoUint64_4);
    memcpy(value->data[0].v_pointer, uint64_zero_array, sizeof(MotoUint64_4));
}

static void __moto_uint64_4_value_free(GValue *value)
{
    g_slice_free(MotoUint64_4, value->data[0].v_pointer);
}

static void __moto_uint64_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoUint64_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoUint64_4));
}

static gpointer __moto_uint64_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_uint64_4_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_uint64_4_lcopy_value(const GValue  *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_uint64_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_uint64_4_value_init,          /* value_init */
            __moto_uint64_4_value_free,          /* value_free */
            __moto_uint64_4_value_copy,          /* value_copy */
            __moto_uint64_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_uint64_4_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_uint64_4_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoUint64_4", & info, & f_info, 0);
    }
    return type;
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

static gpointer __moto_float_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_float_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_float_2_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
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

static gpointer __moto_float_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_float_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_float_3_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
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

static gpointer __moto_float_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_float_4_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_float_4_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
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

// MOTO_TYPE_DOUBLE_2

struct _MotoDouble_2
{
    gdouble data[2];
};

static void __moto_double_2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoDouble_2);
    memcpy(value->data[0].v_pointer, double_zero_array, sizeof(MotoDouble_2));
}

static void __moto_double_2_value_free(GValue *value)
{
    g_slice_free(MotoDouble_2, value->data[0].v_pointer);
}

static void __moto_double_2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoDouble_2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoDouble_2));
}

static gpointer __moto_double_2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_double_2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_double_2_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_double_2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_double_2_value_init,          /* value_init */
            __moto_double_2_value_free,          /* value_free */
            __moto_double_2_value_copy,          /* value_copy */
            __moto_double_2_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_double_2_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_double_2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoDouble_2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_DOUBLE_3

struct _MotoDouble_3
{
    gdouble data[3];
};

static void __moto_double_3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoDouble_3);
    memcpy(value->data[0].v_pointer, double_zero_array, sizeof(MotoDouble_3));
}

static void __moto_double_3_value_free(GValue *value)
{
    g_slice_free(MotoDouble_3, value->data[0].v_pointer);
}

static void __moto_double_3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoDouble_3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoDouble_3));
}

static gpointer __moto_double_3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_double_3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_double_3_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_double_3_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_double_3_value_init,          /* value_init */
            __moto_double_3_value_free,          /* value_free */
            __moto_double_3_value_copy,          /* value_copy */
            __moto_double_3_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_double_3_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_double_3_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoDouble_3", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_DOUBLE_4

struct _MotoDouble_4
{
    gdouble data[4];
};

static void __moto_double_4_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoDouble_4);
    memcpy(value->data[0].v_pointer, double_zero_array, sizeof(MotoDouble_4));
}

static void __moto_double_4_value_free(GValue *value)
{
    g_slice_free(MotoDouble_4, value->data[0].v_pointer);
}

static void __moto_double_4_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoDouble_4);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoDouble_4));
}

static gpointer __moto_double_4_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *__moto_double_4_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *__moto_double_4_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_double_4_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            __moto_double_4_value_init,          /* value_init */
            __moto_double_4_value_free,          /* value_free */
            __moto_double_4_value_copy,          /* value_copy */
            __moto_double_4_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            __moto_double_4_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            __moto_double_4_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoDouble_4", & info, & f_info, 0);
    }
    return type;
}

// Transformation functions

// 1 -> *

void __moto_transform_float_1_to_boolean_2(const GValue *src_value, GValue *dest_value)
{
    gboolean src  = (gboolean)(src_value->data[0].v_float);
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_boolean_3(const GValue *src_value, GValue *dest_value)
{
    gboolean src = (gboolean)( src_value->data[0].v_float);
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_boolean_4(const GValue *src_value, GValue *dest_value)
{
    gboolean src  = (gboolean)(src_value->data[0].v_float);
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

void __moto_transform_float_1_to_int_2(const GValue *src_value, GValue *dest_value)
{
    gint src  = (gint)(src_value->data[0].v_float);
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_int_3(const GValue *src_value, GValue *dest_value)
{
    gint src  = (gint)(src_value->data[0].v_float);
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_int_4(const GValue *src_value, GValue *dest_value)
{
    gint src  = (gint)(src_value->data[0].v_float);
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

void __moto_transform_float_1_to_uint_2(const GValue *src_value, GValue *dest_value)
{
    guint src  = (guint)fabs(src_value->data[0].v_float);
    guint *dest = dest_value->data[0].v_pointer = (guint *)g_slice_new(MotoUint_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_uint_3(const GValue *src_value, GValue *dest_value)
{
    guint src  = (guint)fabs(src_value->data[0].v_float);
    guint *dest = dest_value->data[0].v_pointer = (guint *)g_slice_new(MotoUint_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_uint_4(const GValue *src_value, GValue *dest_value)
{
    guint src  = (guint)fabs(src_value->data[0].v_float);
    guint *dest = dest_value->data[0].v_pointer = (guint *)g_slice_new(MotoUint_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

void __moto_transform_float_1_to_long_2(const GValue *src_value, GValue *dest_value)
{
    glong src  = (glong)(src_value->data[0].v_float);
    glong *dest = dest_value->data[0].v_pointer = (glong *)g_slice_new(MotoLong_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_long_3(const GValue *src_value, GValue *dest_value)
{
    glong src  = (glong)(src_value->data[0].v_float);
    glong *dest = dest_value->data[0].v_pointer = (glong *)g_slice_new(MotoLong_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_long_4(const GValue *src_value, GValue *dest_value)
{
    glong src  = (glong)(src_value->data[0].v_float);
    glong *dest = dest_value->data[0].v_pointer = (glong *)g_slice_new(MotoLong_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

void __moto_transform_float_1_to_ulong_2(const GValue *src_value, GValue *dest_value)
{
    gulong src  = (gulong)fabs(src_value->data[0].v_float);
    gulong *dest = dest_value->data[0].v_pointer = (gulong *)g_slice_new(MotoUlong_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_ulong_3(const GValue *src_value, GValue *dest_value)
{
    gulong src  = (gulong)fabs(src_value->data[0].v_float);
    gulong *dest = dest_value->data[0].v_pointer = (gulong *)g_slice_new(MotoUlong_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_ulong_4(const GValue *src_value, GValue *dest_value)
{
    gulong src  = (gulong)fabs(src_value->data[0].v_float);
    gulong *dest = dest_value->data[0].v_pointer = (gulong *)g_slice_new(MotoUlong_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

void __moto_transform_float_1_to_int64_2(const GValue *src_value, GValue *dest_value)
{
    gint64 src  = (gint64)(src_value->data[0].v_float);
    gint64 *dest = dest_value->data[0].v_pointer = (gint64 *)g_slice_new(MotoInt_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_int64_3(const GValue *src_value, GValue *dest_value)
{
    gint64 src  = (gint64)(src_value->data[0].v_float);
    gint64 *dest = dest_value->data[0].v_pointer = (gint64 *)g_slice_new(MotoInt_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_int64_4(const GValue *src_value, GValue *dest_value)
{
    gint64 src  = (gint64)(src_value->data[0].v_float);
    gint64 *dest = dest_value->data[0].v_pointer = (gint64 *)g_slice_new(MotoInt_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

void __moto_transform_float_1_to_uint64_2(const GValue *src_value, GValue *dest_value)
{
    guint64 src  = (guint64)fabs(src_value->data[0].v_float);
    guint64 *dest = dest_value->data[0].v_pointer = (guint64 *)g_slice_new(MotoUint64_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_uint64_3(const GValue *src_value, GValue *dest_value)
{
    guint64 src  = (guint64)fabs(src_value->data[0].v_float);
    guint64 *dest = dest_value->data[0].v_pointer = (guint64 *)g_slice_new(MotoUint64_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_uint64_4(const GValue *src_value, GValue *dest_value)
{
    guint64 src  = (guint64)fabs(src_value->data[0].v_float);
    guint64 *dest = dest_value->data[0].v_pointer = (guint64 *)g_slice_new(MotoUint64_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

void __moto_transform_float_1_to_float_2(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_float_3(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_float_4(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

void __moto_transform_float_1_to_double_2(const GValue *src_value, GValue *dest_value)
{
    gdouble src  = (gdouble)(src_value->data[0].v_float);
    gdouble *dest = dest_value->data[0].v_pointer = (gdouble *)g_slice_new(MotoDouble_2);
    dest[0] = src;
    dest[1] = src;
}

void __moto_transform_float_1_to_double_3(const GValue *src_value, GValue *dest_value)
{
    gdouble src  = (gdouble)(src_value->data[0].v_float);
    gdouble *dest = dest_value->data[0].v_pointer = (gdouble *)g_slice_new(MotoDouble_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

void __moto_transform_float_1_to_double_4(const GValue *src_value, GValue *dest_value)
{
    gdouble src  = (gdouble)(src_value->data[0].v_float);
    gdouble *dest = dest_value->data[0].v_pointer = (gdouble *)g_slice_new(MotoDouble_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

// 2 -> *

void __moto_transform_float_2_to_float_1(const GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1]) / 2);
}

// 3 -> *

void __moto_transform_float_3_to_float_1(const GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1] + src[2]) / 3);
}

// 4 -> *

void __moto_transform_float_4_to_float_1(const GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1] + src[2] + src[3]) / 4);
}

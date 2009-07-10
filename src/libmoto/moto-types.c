#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gobject/gvaluecollector.h>

#include "moto-types.h"

static gboolean boolean_zero_array[4];
static gint     int_zero_array[4];
static gfloat   float_zero_array[4];

typedef struct _MotoBoolean_2 MotoBoolean_2;
typedef struct _MotoBoolean_3 MotoBoolean_3;
typedef struct _MotoBoolean_4 MotoBoolean_4;

typedef struct _MotoInt_2 MotoInt_2;
typedef struct _MotoInt_3 MotoInt_3;
typedef struct _MotoInt_4 MotoInt_4;

typedef struct _MotoFloat_2 MotoFloat_2;
typedef struct _MotoFloat_3 MotoFloat_3;
typedef struct _MotoFloat_4 MotoFloat_4;

typedef struct _MotoBoolean_2x2 MotoBoolean_2x2;
typedef struct _MotoBoolean_3x3 MotoBoolean_3x3;
typedef struct _MotoBoolean_4x4 MotoBoolean_4x4;

typedef struct _MotoInt_2x2 MotoInt_2x2;
typedef struct _MotoInt_3x3 MotoInt_3x3;
typedef struct _MotoInt_4x4 MotoInt_4x4;

typedef struct _MotoFloat_2x2 MotoFloat_2x2;
typedef struct _MotoFloat_3x3 MotoFloat_3x3;
typedef struct _MotoFloat_4x4 MotoFloat_4x4;

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

// Transformation functions

// 1 -> *
//
// boolean -> *
static void __moto_transform_boolean_1_to_boolean_2(const GValue *src_value, GValue *dest_value)
{
    gboolean src  = (src_value->data[0].v_int) ? TRUE : FALSE;
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_boolean_1_to_boolean_3(const GValue *src_value, GValue *dest_value)
{
    gboolean src = (src_value->data[0].v_int) ? TRUE : FALSE;
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_boolean_1_to_boolean_4(const GValue *src_value, GValue *dest_value)
{
    gboolean src  = (src_value->data[0].v_int) ? TRUE : FALSE;
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

static void __moto_transform_boolean_1_to_int_2(const GValue *src_value, GValue *dest_value)
{
    gint src  = (src_value->data[0].v_int) ? 1 : 0;
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_boolean_1_to_int_3(const GValue *src_value, GValue *dest_value)
{
    gint src  = (src_value->data[0].v_int) ? 1 : 0;
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_boolean_1_to_int_4(const GValue *src_value, GValue *dest_value)
{
    gint src  = (src_value->data[0].v_int) ? 1 : 0;
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

static void __moto_transform_boolean_1_to_float_2(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = (src_value->data[0].v_int) ? 1.0f : 0.0f;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_boolean_1_to_float_3(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = (src_value->data[0].v_int) ? 1.0f : 0.0f;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_boolean_1_to_float_4(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = (src_value->data[0].v_int) ? 1.0f : 0.0f;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

// int -> *
static void __moto_transform_int_1_to_boolean_2(const GValue *src_value, GValue *dest_value)
{
    gboolean src  = (src_value->data[0].v_int) ? TRUE : FALSE;
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_int_1_to_boolean_3(const GValue *src_value, GValue *dest_value)
{
    gint src = (src_value->data[0].v_int) ? TRUE : FALSE;
    gint *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_int_1_to_boolean_4(const GValue *src_value, GValue *dest_value)
{
    gboolean src  = (src_value->data[0].v_int) ? TRUE : FALSE;
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

static void __moto_transform_int_1_to_int_2(const GValue *src_value, GValue *dest_value)
{
    gint src  = src_value->data[0].v_int;
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_int_1_to_int_3(const GValue *src_value, GValue *dest_value)
{
    gint src  = src_value->data[0].v_int;
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_int_1_to_int_4(const GValue *src_value, GValue *dest_value)
{
    gint src  = src_value->data[0].v_int;
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

static void __moto_transform_int_1_to_float_2(const GValue *src_value, GValue *dest_value)
{
    gfloat src = (gfloat)(src_value->data[0].v_int);
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_int_1_to_float_3(const GValue *src_value, GValue *dest_value)
{
    gfloat src = (gfloat)(src_value->data[0].v_int);
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_int_1_to_float_4(const GValue *src_value, GValue *dest_value)
{
    gfloat src = (gfloat)(src_value->data[0].v_int);
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

// float -> *

static void __moto_transform_float_1_to_boolean_2(const GValue *src_value, GValue *dest_value)
{
    gboolean src  = (gboolean)(src_value->data[0].v_float);
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_float_1_to_boolean_3(const GValue *src_value, GValue *dest_value)
{
    gboolean src = (gboolean)( src_value->data[0].v_float);
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_float_1_to_boolean_4(const GValue *src_value, GValue *dest_value)
{
    gboolean src  = (gboolean)(src_value->data[0].v_float);
    gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBoolean_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

static void __moto_transform_float_1_to_int_2(const GValue *src_value, GValue *dest_value)
{
    gint src  = (gint)(src_value->data[0].v_float);
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_float_1_to_int_3(const GValue *src_value, GValue *dest_value)
{
    gint src  = (gint)(src_value->data[0].v_float);
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_float_1_to_int_4(const GValue *src_value, GValue *dest_value)
{
    gint src  = (gint)(src_value->data[0].v_float);
    gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

static void __moto_transform_float_1_to_float_2(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_2);
    dest[0] = src;
    dest[1] = src;
}

static void __moto_transform_float_1_to_float_3(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_3);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
}

static void __moto_transform_float_1_to_float_4(const GValue *src_value, GValue *dest_value)
{
    gfloat src  = src_value->data[0].v_float;
    gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat_4);
    dest[0] = src;
    dest[1] = src;
    dest[2] = src;
    dest[3] = src;
}

// 2 -> *

static void __moto_transform_float_2_to_float_1(const GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1]) / 2);
}

// 3 -> *

static void __moto_transform_float_3_to_float_1(const GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1] + src[2]) / 3);
}

// 4 -> *

static void __moto_transform_float_4_to_float_1(const GValue *src_value, GValue *dest_value)
{
    gfloat *src = (gfloat *)src_value->data[0].v_pointer;
    g_value_set_float(dest_value, (src[0] + src[1] + src[2] + src[3]) / 4);
}

// Public functions

void moto_types_init(void)
{
    // 1 -> *

    // boolean -> *
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_BOOLEAN_2, __moto_transform_boolean_1_to_boolean_2);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_BOOLEAN_3, __moto_transform_boolean_1_to_boolean_3);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_BOOLEAN_4, __moto_transform_boolean_1_to_boolean_4);

    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_INT_2, __moto_transform_boolean_1_to_int_2);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_INT_3, __moto_transform_boolean_1_to_int_3);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_INT_4, __moto_transform_boolean_1_to_int_4);

    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_FLOAT_2, __moto_transform_boolean_1_to_float_2);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_FLOAT_3, __moto_transform_boolean_1_to_float_3);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_1, MOTO_TYPE_FLOAT_4, __moto_transform_boolean_1_to_float_4);

    // int -> *
    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_BOOLEAN_2, __moto_transform_int_1_to_boolean_2);
    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_BOOLEAN_3, __moto_transform_int_1_to_boolean_3);
    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_BOOLEAN_4, __moto_transform_int_1_to_boolean_4);

    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_INT_2, __moto_transform_int_1_to_int_2);
    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_INT_3, __moto_transform_int_1_to_int_3);
    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_INT_4, __moto_transform_int_1_to_int_4);

    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_FLOAT_2, __moto_transform_int_1_to_float_2);
    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_FLOAT_3, __moto_transform_int_1_to_float_3);
    g_value_register_transform_func(MOTO_TYPE_INT_1, MOTO_TYPE_FLOAT_4, __moto_transform_int_1_to_float_4);

    // float -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_BOOLEAN_2, __moto_transform_float_1_to_boolean_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_BOOLEAN_3, __moto_transform_float_1_to_boolean_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_BOOLEAN_4, __moto_transform_float_1_to_boolean_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT_2, __moto_transform_float_1_to_int_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT_3, __moto_transform_float_1_to_int_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_INT_4, __moto_transform_float_1_to_int_4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_FLOAT_2, __moto_transform_float_1_to_float_2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_FLOAT_3, __moto_transform_float_1_to_float_3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_1, MOTO_TYPE_FLOAT_4, __moto_transform_float_1_to_float_4);

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

    /*
    // Matrix types trasformations

    // 2x2 -> 2x2
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_2x2, MOTO_TYPE_INT_2x2,    __moto_transform_boolean_2x2_to_int_2x2);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_2x2, MOTO_TYPE_FLOAT_2x2,  __moto_transform_boolean_2x2_to_float_2x2);

    g_value_register_transform_func(MOTO_TYPE_INT_2x2, MOTO_TYPE_BOOLEAN_2x2, __moto_transform_int_2x2_to_boolean_2x2);
    g_value_register_transform_func(MOTO_TYPE_INT_2x2, MOTO_TYPE_FLOAT_2x2,   __moto_transform_int_2x2_to_float_2x2);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_2x2, MOTO_TYPE_BOOLEAN_2x2, __moto_transform_float_2x2_to_boolean_2x2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_2x2, MOTO_TYPE_INT_2x2,     __moto_transform_float_2x2_to_int_2x2);

    // 3x3 -> 3x3
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_3x3, MOTO_TYPE_INT_3x3,    __moto_transform_boolean_3x3_to_int_3x3);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_3x3, MOTO_TYPE_FLOAT_3x3,  __moto_transform_boolean_3x3_to_float_3x3);

    g_value_register_transform_func(MOTO_TYPE_INT_3x3, MOTO_TYPE_BOOLEAN_3x3, __moto_transform_int_3x3_to_boolean_3x3);
    g_value_register_transform_func(MOTO_TYPE_INT_3x3, MOTO_TYPE_FLOAT_3x3,   __moto_transform_int_3x3_to_float_3x3);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_3x3, MOTO_TYPE_BOOLEAN_3x3, __moto_transform_float_3x3_to_boolean_3x3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_3x3, MOTO_TYPE_INT_3x3,     __moto_transform_float_3x3_to_int_3x3);

    // 4x4 -> 4x4
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_4x4, MOTO_TYPE_INT_4x4,    __moto_transform_boolean_4x4_to_int_4x4);
    g_value_register_transform_func(MOTO_TYPE_BOOLEAN_4x4, MOTO_TYPE_FLOAT_4x4,  __moto_transform_boolean_4x4_to_float_4x4);

    g_value_register_transform_func(MOTO_TYPE_INT_4x4, MOTO_TYPE_BOOLEAN_4x4, __moto_transform_int_4x4_to_boolean_4x4);
    g_value_register_transform_func(MOTO_TYPE_INT_4x4, MOTO_TYPE_FLOAT_4x4,   __moto_transform_int_4x4_to_float_4x4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT_4x4, MOTO_TYPE_BOOLEAN_4x4, __moto_transform_float_4x4_to_boolean_4x4);
    g_value_register_transform_func(MOTO_TYPE_FLOAT_4x4, MOTO_TYPE_INT_4x4,     __moto_transform_float_4x4_to_int_4x4);

    */
}

// boolean

void moto_value_get_boolean_2(GValue *value, gboolean *v0, gboolean *v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
}

void moto_value_get_boolean_2_v(GValue *value, gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gboolean)*2);
}

void moto_value_get_boolean_3(GValue *value, gboolean *v0, gboolean *v1, gboolean *v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_3);
    gboolean *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
}

void moto_value_get_boolean_3_v(GValue *value, gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gboolean)*3);
}

void moto_value_get_boolean_4(GValue *value, gboolean *v0, gboolean *v1, gboolean *v2, gboolean *v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_4);
    gboolean *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
    *v3 = p[3];
}

void moto_value_get_boolean_4_v(GValue *value, gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gboolean)*4);
}

// int

void moto_value_get_int_2(GValue *value, gint *v0, gint *v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
}

void moto_value_get_int_2_v(GValue *value, gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gint)*2);
}

void moto_value_get_int_3(GValue *value, gint *v0, gint *v1, gint *v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_3);
    gint *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
}

void moto_value_get_int_3_v(GValue *value, gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gint)*3);
}

void moto_value_get_int_4(GValue *value, gint *v0, gint *v1, gint *v2, gint *v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_4);
    gint *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
    *v3 = p[3];
}

void moto_value_get_int_4_v(GValue *value, gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gint)*4);
}

// float

void moto_value_get_float_2(GValue *value, gfloat *v0, gfloat *v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
}

void moto_value_get_float_2_v(GValue *value, gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gfloat)*2);
}

void moto_value_get_float_3(GValue *value, gfloat *v0, gfloat *v1, gfloat *v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_3);
    gfloat *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
}

void moto_value_get_float_3_v(GValue *value, gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gfloat)*3);
}

void moto_value_get_float_4(GValue *value, gfloat *v0, gfloat *v1, gfloat *v2, gfloat *v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_4);
    gfloat *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
    *v3 = p[3];
}

void moto_value_get_float_4_v(GValue *value, gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gfloat)*4);
}

// boolean

void moto_value_set_boolean_2(GValue *value, gboolean v0, gboolean v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
}

void moto_value_set_boolean_2_v(GValue *value, const gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gboolean)*2);
}

void moto_value_set_boolean_3(GValue *value, gboolean v0, gboolean v1, gboolean v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
}

void moto_value_set_boolean_3_v(GValue *value, const gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gboolean)*3);
}

void moto_value_set_boolean_4(GValue *value, gboolean v0, gboolean v1, gboolean v2, gboolean v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
    p[3] = v3;
}

void moto_value_set_boolean_4_v(GValue *value, const gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOLEAN_2);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gboolean)*4);
}

// int

void moto_value_set_int_2(GValue *value, gint v0, gint v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
}

void moto_value_set_int_2_v(GValue *value, const gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gint)*2);
}

void moto_value_set_int_3(GValue *value, gint v0, gint v1, gint v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
}

void moto_value_set_int_3_v(GValue *value, const gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gint)*3);
}

void moto_value_set_int_4(GValue *value, gint v0, gint v1, gint v2, gint v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
    p[3] = v3;
}

void moto_value_set_int_4_v(GValue *value, const gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT_2);
    gint *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gint)*4);
}

// float

void moto_value_set_float_2(GValue *value, gfloat v0, gfloat v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
}

void moto_value_set_float_2_v(GValue *value, const gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gfloat)*2);
}

void moto_value_set_float_3(GValue *value, gfloat v0, gfloat v1, gfloat v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
}

void moto_value_set_float_3_v(GValue *value, const gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gfloat)*3);
}

void moto_value_set_float_4(GValue *value, gfloat v0, gfloat v1, gfloat v2, gfloat v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
    p[3] = v3;
}

void moto_value_set_float_4_v(GValue *value, const gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_2);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gfloat)*4);
}

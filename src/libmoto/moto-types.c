#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gobject/gvaluecollector.h>

#include "moto-types.h"

static gboolean boolzero_array[4];
static gint     intzero_array[4];
static gfloat   floatzero_array[4];

typedef struct _MotoBool2 MotoBool2;
typedef struct _MotoBool3 MotoBool3;
typedef struct _MotoBool4 MotoBool4;

typedef struct _MotoInt2 MotoInt2;
typedef struct _MotoInt3 MotoInt3;
typedef struct _MotoInt4 MotoInt4;

typedef struct _MotoFloat2 MotoFloat2;
typedef struct _MotoFloat3 MotoFloat3;
typedef struct _MotoFloat4 MotoFloat4;

typedef struct _MotoBoolArray MotoBoolArray;
typedef struct _MotoIntArray MotoIntArray;
typedef struct _MotoFloatArray MotoFloatArray;

typedef struct _MotoBool2x2 MotoBool2x2;
typedef struct _MotoBool3x3 MotoBool3x3;
typedef struct _MotoBool4x4 MotoBool4x4;

typedef struct _MotoInt2x2 MotoInt2x2;
typedef struct _MotoInt3x3 MotoInt3x3;
typedef struct _MotoInt4x4 MotoInt4x4;

typedef struct _MotoFloat2x2 MotoFloat2x2;
typedef struct _MotoFloat3x3 MotoFloat3x3;
typedef struct _MotoFloat4x4 MotoFloat4x4;

// MOTO_TYPE_BOOL2

struct _MotoBool2
{
    gboolean data[2];
};

static void moto_bool2_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoBool2);
    memcpy(value->data[0].v_pointer, boolzero_array, sizeof(MotoBool2));
}

static void moto_bool2_value_free(GValue *value)
{
    g_slice_free(MotoBool2, value->data[0].v_pointer);
}

static void moto_bool2_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoBool2);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoBool2));
}

static gpointer moto_bool2_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *moto_bool2_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

static gchar *moto_bool2_lcopy_value(const GValue       *value,
                                         guint         n_collect_values,
                                         GTypeCValue  *collect_values,
                                         guint         collect_flags)
{
    // TODO: Implement
    return NULL;
}

GType moto_bool2_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GTypeValueTable value_table = \
        {
            moto_bool2_value_init,          /* value_init */
            moto_bool2_value_free,          /* value_free */
            moto_bool2_value_copy,          /* value_copy */
            moto_bool2_value_peek_pointer,  /* value_peek_pointer */
            "p",                                /* collect_format */
            moto_bool2_collect_value,       /* collect_value */
            "p",                                /* lcopy_format */
            moto_bool2_lcopy_value,         /* lcopy_value */
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
        type = g_type_register_fundamental(type_id, "MotoBool2", & info, & f_info, 0);
    }
    return type;
}

// MOTO_TYPE_BOOL3

struct _MotoBool3
{
    gboolean data[3];
};

static void moto_bool3_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoBool3);
    memcpy(value->data[0].v_pointer, boolzero_array, sizeof(MotoBool3));
}

static void moto_bool3_value_free(GValue *value)
{
    g_slice_free(MotoBool3, value->data[0].v_pointer);
}

static void moto_bool3_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoBool3);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoBool3));
}

static gpointer moto_bool3_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *moto_bool3_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
    }

    static gchar *moto_bool3_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_bool3_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_bool3_value_init,          /* value_init */
                moto_bool3_value_free,          /* value_free */
                moto_bool3_value_copy,          /* value_copy */
                moto_bool3_value_peek_pointer,  /* value_peek_pointer */
                "p",                                /* collect_format */
                moto_bool3_collect_value,       /* collect_value */
                "p",                                /* lcopy_format */
                moto_bool3_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoBool3", & info, & f_info, 0);
        }
        return type;
    }

    // MOTO_TYPE_BOOL4

    struct _MotoBool4
    {
        gboolean data[4];
    };

    static void moto_bool4_value_init(GValue *value)
    {
        value->data[0].v_pointer = g_slice_new(MotoBool4);
        memcpy(value->data[0].v_pointer, boolzero_array, sizeof(MotoBool4));
    }

    static void moto_bool4_value_free(GValue *value)
    {
        g_slice_free(MotoBool4, value->data[0].v_pointer);
    }

    static void moto_bool4_value_copy(const GValue *src_value, GValue *dest_value)
    {
        dest_value->data[0].v_pointer = g_slice_new(MotoBool4);
        memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoBool4));
    }

    static gpointer moto_bool4_value_peek_pointer(const GValue *value)
    {
        return value->data[0].v_pointer;
    }

    static gchar *moto_bool4_collect_value(GValue       *value,
                                               guint         n_collect_values,
                                               GTypeCValue  *collect_values,
                                               guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    static gchar *moto_bool4_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_bool4_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_bool4_value_init,          /* value_init */
                moto_bool4_value_free,          /* value_free */
                moto_bool4_value_copy,          /* value_copy */
                moto_bool4_value_peek_pointer,  /* value_peek_pointer */
                "p",                                /* collect_format */
                moto_bool4_collect_value,       /* collect_value */
                "p",                                /* lcopy_format */
                moto_bool4_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoBool4", & info, & f_info, 0);
        }
        return type;
    }

    // MOTO_TYPE_INT2

    struct _MotoInt2
    {
        gint data[2];
    };

    static void moto_int2_value_init(GValue *value)
    {
        value->data[0].v_pointer = g_slice_new(MotoInt2);
        memcpy(value->data[0].v_pointer, intzero_array, sizeof(MotoInt2));
    }

    static void moto_int2_value_free(GValue *value)
    {
        g_slice_free(MotoInt2, value->data[0].v_pointer);
    }

    static void moto_int2_value_copy(const GValue *src_value, GValue *dest_value)
    {
        dest_value->data[0].v_pointer = g_slice_new(MotoInt2);
        memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt2));
    }

    static gpointer moto_int2_value_peek_pointer(const GValue *value)
    {
        return value->data[0].v_pointer;
    }

    static gchar *moto_int2_collect_value(GValue       *value,
                                               guint         n_collect_values,
                                               GTypeCValue  *collect_values,
                                               guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    static gchar *moto_int2_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_int2_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_int2_value_init,          /* value_init */
                moto_int2_value_free,          /* value_free */
                moto_int2_value_copy,          /* value_copy */
                moto_int2_value_peek_pointer,  /* value_peek_pointer */
                "p",                                /* collect_format */
                moto_int2_collect_value,       /* collect_value */
                "p",                                /* lcopy_format */
                moto_int2_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoInt2", & info, & f_info, 0);
        }
        return type;
    }

    // MOTO_TYPE_INT3

    struct _MotoInt3
    {
        gint data[3];
    };

    static void moto_int3_value_init(GValue *value)
    {
        value->data[0].v_pointer = g_slice_new(MotoInt3);
        memcpy(value->data[0].v_pointer, intzero_array, sizeof(MotoInt3));
    }

    static void moto_int3_value_free(GValue *value)
    {
        g_slice_free(MotoInt3, value->data[0].v_pointer);
    }

    static void moto_int3_value_copy(const GValue *src_value, GValue *dest_value)
    {
        dest_value->data[0].v_pointer = g_slice_new(MotoInt3);
        memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt3));
    }

    static gpointer moto_int3_value_peek_pointer(const GValue *value)
    {
        return value->data[0].v_pointer;
    }

    static gchar *moto_int3_collect_value(GValue       *value,
                                               guint         n_collect_values,
                                               GTypeCValue  *collect_values,
                                               guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    static gchar *moto_int3_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_int3_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_int3_value_init,          /* value_init */
                moto_int3_value_free,          /* value_free */
                moto_int3_value_copy,          /* value_copy */
                moto_int3_value_peek_pointer,  /* value_peek_pointer */
                "p",                                /* collect_format */
                moto_int3_collect_value,       /* collect_value */
                "p",                                /* lcopy_format */
                moto_int3_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoInt3", & info, & f_info, 0);
        }
        return type;
    }

    // MOTO_TYPE_INT4

    struct _MotoInt4
    {
        gint data[4];
    };

    static void moto_int4_value_init(GValue *value)
    {
        value->data[0].v_pointer = g_slice_new(MotoInt4);
        memcpy(value->data[0].v_pointer, intzero_array, sizeof(MotoInt4));
    }

    static void moto_int4_value_free(GValue *value)
    {
        g_slice_free(MotoInt4, value->data[0].v_pointer);
    }

    static void moto_int4_value_copy(const GValue *src_value, GValue *dest_value)
    {
        dest_value->data[0].v_pointer = g_slice_new(MotoInt4);
        memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoInt4));
    }

    static gpointer moto_int4_value_peek_pointer(const GValue *value)
    {
        return value->data[0].v_pointer;
    }

    static gchar *moto_int4_collect_value(GValue       *value,
                                               guint         n_collect_values,
                                               GTypeCValue  *collect_values,
                                               guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    static gchar *moto_int4_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_int4_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_int4_value_init,          /* value_init */
                moto_int4_value_free,          /* value_free */
                moto_int4_value_copy,          /* value_copy */
                moto_int4_value_peek_pointer,  /* value_peek_pointer */
                "p",                                /* collect_format */
                moto_int4_collect_value,       /* collect_value */
                "p",                                /* lcopy_format */
                moto_int4_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoInt4", & info, & f_info, 0);
        }
        return type;
    }

    // MOTO_TYPE_FLOAT2

    struct _MotoFloat2
    {
        gfloat data[2];
    };

    static void moto_float2_value_init(GValue *value)
    {
        value->data[0].v_pointer = g_slice_new(MotoFloat2);
        memcpy(value->data[0].v_pointer, floatzero_array, sizeof(MotoFloat2));
    }

    static void moto_float2_value_free(GValue *value)
    {
        g_slice_free(MotoFloat2, value->data[0].v_pointer);
    }

    static void moto_float2_value_copy(const GValue *src_value, GValue *dest_value)
    {
        dest_value->data[0].v_pointer = g_slice_new(MotoFloat2);
        memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoFloat2));
    }

    static gpointer moto_float2_value_peek_pointer(const GValue *value)
    {
        return value->data[0].v_pointer;
    }

    static gchar *moto_float2_collect_value(GValue       *value,
                                               guint         n_collect_values,
                                               GTypeCValue  *collect_values,
                                               guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    static gchar *moto_float2_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_float2_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_float2_value_init,          /* value_init */
                moto_float2_value_free,          /* value_free */
                moto_float2_value_copy,          /* value_copy */
                moto_float2_value_peek_pointer,  /* value_peek_pointer */
                "p",                                /* collect_format */
                moto_float2_collect_value,       /* collect_value */
                "p",                                /* lcopy_format */
                moto_float2_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoFloat2", & info, & f_info, 0);
        }
        return type;
    }

    // MOTO_TYPE_FLOAT3

    struct _MotoFloat3
    {
        gfloat data[3];
    };

    static void moto_float3_value_init(GValue *value)
    {
        value->data[0].v_pointer = g_slice_new(MotoFloat3);
        memcpy(value->data[0].v_pointer, floatzero_array, sizeof(MotoFloat3));
    }

    static void moto_float3_value_free(GValue *value)
    {
        g_slice_free(MotoFloat3, value->data[0].v_pointer);
    }

    static void moto_float3_value_copy(const GValue *src_value, GValue *dest_value)
    {
        dest_value->data[0].v_pointer = g_slice_new(MotoFloat3);
        memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoFloat3));
    }

    static gpointer moto_float3_value_peek_pointer(const GValue *value)
    {
        return value->data[0].v_pointer;
    }

    static gchar *moto_float3_collect_value(GValue       *value,
                                               guint         n_collect_values,
                                               GTypeCValue  *collect_values,
                                               guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    static gchar *moto_float3_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_float3_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_float3_value_init,          /* value_init */
                moto_float3_value_free,          /* value_free */
                moto_float3_value_copy,          /* value_copy */
                moto_float3_value_peek_pointer,  /* value_peek_pointer */
                "p",                                /* collect_format */
                moto_float3_collect_value,       /* collect_value */
                "p",                                /* lcopy_format */
                moto_float3_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoFloat3", & info, & f_info, 0);
        }
        return type;
    }

    // MOTO_TYPE_FLOAT4

    struct _MotoFloat4
    {
        gfloat data[4];
    };

    static void moto_float4_value_init(GValue *value)
    {
        value->data[0].v_pointer = g_slice_new(MotoFloat4);
        memcpy(value->data[0].v_pointer, floatzero_array, sizeof(MotoFloat4));
    }

    static void moto_float4_value_free(GValue *value)
    {
        g_slice_free(MotoFloat4, value->data[0].v_pointer);
    }

    static void moto_float4_value_copy(const GValue *src_value, GValue *dest_value)
    {
        dest_value->data[0].v_pointer = g_slice_new(MotoFloat4);
        memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoFloat4));
    }

    static gpointer moto_float4_value_peek_pointer(const GValue *value)
    {
        return value->data[0].v_pointer;
    }

    static gchar *moto_float4_collect_value(GValue       *value,
                                               guint         n_collect_values,
                                               GTypeCValue  *collect_values,
                                               guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    static gchar *moto_float4_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_float4_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_float4_value_init,          /* value_init */
                moto_float4_value_free,          /* value_free */
                moto_float4_value_copy,          /* value_copy */
                moto_float4_value_peek_pointer,  /* value_peek_pointer */
                "p",                                /* collect_format */
                moto_float4_collect_value,       /* collect_value */
                "p",                                /* lcopy_format */
                moto_float4_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoFloat4", & info, & f_info, 0);
        }
        return type;
    }

// MOTO_TYPE_BOOL_ARRAY

struct _MotoBoolArray
{
    gboolean* data;
    gsize _size;
};

static void moto_bool_array_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoBoolArray);
    memcpy(value->data[0].v_pointer, boolzero_array, sizeof(MotoBoolArray));
}

static void moto_bool_array_value_free(GValue *value)
{
    g_slice_free(MotoBoolArray, value->data[0].v_pointer);
}

static void moto_bool_array_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoBoolArray);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoBoolArray));
}

static gpointer moto_bool_array_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *moto_bool_array_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
    }

    static gchar *moto_bool_array_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_bool_array_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_bool_array_value_init,          /* value_init */
                moto_bool_array_value_free,          /* value_free */
                moto_bool_array_value_copy,          /* value_copy */
                moto_bool_array_value_peek_pointer,  /* value_peek_pointer */
                "p",                                      /* collect_format */
                moto_bool_array_collect_value,       /* collect_value */
                "p",                                      /* lcopy_format */
                moto_bool_array_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoBoolArray", & info, & f_info, 0);
        }
        return type;
    }

// MOTO_TYPE_INT_ARRAY

struct _MotoIntArray
{
    gint* data;
    gsize _size;
};

static void moto_int_array_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoIntArray);
    memcpy(value->data[0].v_pointer, boolzero_array, sizeof(MotoIntArray));
}

static void moto_int_array_value_free(GValue *value)
{
    g_slice_free(MotoIntArray, value->data[0].v_pointer);
}

static void moto_int_array_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoIntArray);
    memcpy(dest_value->data[0].v_pointer, src_value->data[0].v_pointer, sizeof(MotoIntArray));
}

static gpointer moto_int_array_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *moto_int_array_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
    }

    static gchar *moto_int_array_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_int_array_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_int_array_value_init,          /* value_init */
                moto_int_array_value_free,          /* value_free */
                moto_int_array_value_copy,          /* value_copy */
                moto_int_array_value_peek_pointer,  /* value_peek_pointer */
                "p",                                      /* collect_format */
                moto_int_array_collect_value,       /* collect_value */
                "p",                                      /* lcopy_format */
                moto_int_array_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoIntArray", & info, & f_info, 0);
        }
        return type;
    }

// MOTO_TYPE_FLOAT_ARRAY

struct _MotoFloatArray
{
    gfloat* data;
    gsize size;
};

static void moto_float_array_value_init(GValue *value)
{
    value->data[0].v_pointer = g_slice_new(MotoFloatArray);
    MotoFloatArray* a = value->data[0].v_pointer;
    a->data = NULL;
    a->size = 0;
}

static void moto_float_array_value_free(GValue *value)
{
    MotoFloatArray* a = (MotoFloatArray*)value->data[0].v_pointer;
    if(a->data)
        g_free(a->data);
    g_slice_free(MotoFloatArray, value->data[0].v_pointer);
}

static void moto_float_array_value_copy(const GValue *src_value, GValue *dest_value)
{
    dest_value->data[0].v_pointer = g_slice_new(MotoFloatArray);

    MotoFloatArray* dest = (MotoFloatArray*)dest_value->data[0].v_pointer;
    MotoFloatArray* src = (MotoFloatArray*)src_value->data[0].v_pointer;
    if(src->size > 0)
    {
        dest->data = g_try_malloc(sizeof(gfloat)*src->size);
        memcpy(dest->data, src->data, sizeof(gfloat)*src->size);
        dest->size = src->size;
    }
    else
    {
        dest->data = NULL;
        dest->size = 0;
    }
}

static gpointer moto_float_array_value_peek_pointer(const GValue *value)
{
    return value->data[0].v_pointer;
}

static gchar *moto_float_array_collect_value(GValue       *value,
                                           guint         n_collect_values,
                                           GTypeCValue  *collect_values,
                                           guint         collect_flags)
{
    // TODO: Implement
    return NULL;
    }

    static gchar *moto_float_array_lcopy_value(const GValue       *value,
                                             guint         n_collect_values,
                                             GTypeCValue  *collect_values,
                                             guint         collect_flags)
    {
        // TODO: Implement
        return NULL;
    }

    GType moto_float_array_get_type(void)
    {
        static GType type = 0;
        if(0 == type)
        {
            static GTypeValueTable value_table = \
            {
                moto_float_array_value_init,          /* value_init */
                moto_float_array_value_free,          /* value_free */
                moto_float_array_value_copy,          /* value_copy */
                moto_float_array_value_peek_pointer,  /* value_peek_pointer */
                "p",                                      /* collect_format */
                moto_float_array_collect_value,       /* collect_value */
                "p",                                      /* lcopy_format */
                moto_float_array_lcopy_value,         /* lcopy_value */
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
            type = g_type_register_fundamental(type_id, "MotoFloatArray", & info, & f_info, 0);
        }
        return type;
    }

    // Transformation functions

    // 1 -> *
    //
    // boolean -> *
    static void moto_transform_bool1_to_bool2(const GValue *src_value, GValue *dest_value)
    {
        gboolean src  = (src_value->data[0].v_int) ? TRUE : FALSE;
        gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_bool1_to_bool3(const GValue *src_value, GValue *dest_value)
    {
        gboolean src = (src_value->data[0].v_int) ? TRUE : FALSE;
        gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_bool1_to_bool4(const GValue *src_value, GValue *dest_value)
    {
        gboolean src  = (src_value->data[0].v_int) ? TRUE : FALSE;
        gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    static void moto_transform_bool1_to_int2(const GValue *src_value, GValue *dest_value)
    {
        gint src  = (src_value->data[0].v_int) ? 1 : 0;
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_bool1_to_int3(const GValue *src_value, GValue *dest_value)
    {
        gint src  = (src_value->data[0].v_int) ? 1 : 0;
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_bool1_to_int4(const GValue *src_value, GValue *dest_value)
    {
        gint src  = (src_value->data[0].v_int) ? 1 : 0;
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    static void moto_transform_bool1_to_float2(const GValue *src_value, GValue *dest_value)
    {
        gfloat src  = (src_value->data[0].v_int) ? 1.0f : 0.0f;
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_bool1_to_float3(const GValue *src_value, GValue *dest_value)
    {
        gfloat src  = (src_value->data[0].v_int) ? 1.0f : 0.0f;
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_bool1_to_float4(const GValue *src_value, GValue *dest_value)
    {
        gfloat src  = (src_value->data[0].v_int) ? 1.0f : 0.0f;
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    // int -> *
    static void moto_transform_int1_to_bool2(const GValue *src_value, GValue *dest_value)
    {
        gboolean src  = (src_value->data[0].v_int) ? TRUE : FALSE;
        gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_int1_to_bool3(const GValue *src_value, GValue *dest_value)
    {
        gint src = (src_value->data[0].v_int) ? TRUE : FALSE;
        gint *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_int1_to_bool4(const GValue *src_value, GValue *dest_value)
    {
        gboolean src  = (src_value->data[0].v_int) ? TRUE : FALSE;
        gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    static void moto_transform_int1_to_int2(const GValue *src_value, GValue *dest_value)
    {
        gint src  = src_value->data[0].v_int;
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_int1_to_int3(const GValue *src_value, GValue *dest_value)
    {
        gint src  = src_value->data[0].v_int;
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_int1_to_int4(const GValue *src_value, GValue *dest_value)
    {
        gint src  = src_value->data[0].v_int;
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    static void moto_transform_int1_to_float2(const GValue *src_value, GValue *dest_value)
    {
        gfloat src = (gfloat)(src_value->data[0].v_int);
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_int1_to_float3(const GValue *src_value, GValue *dest_value)
    {
        gfloat src = (gfloat)(src_value->data[0].v_int);
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_int1_to_float4(const GValue *src_value, GValue *dest_value)
    {
        gfloat src = (gfloat)(src_value->data[0].v_int);
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    // float -> *

    static void moto_transform_float1_to_bool2(const GValue *src_value, GValue *dest_value)
    {
        gboolean src  = (gboolean)(src_value->data[0].v_float);
        gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_float1_to_bool3(const GValue *src_value, GValue *dest_value)
    {
        gboolean src = (gboolean)( src_value->data[0].v_float);
        gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_float1_to_bool4(const GValue *src_value, GValue *dest_value)
    {
        gboolean src  = (gboolean)(src_value->data[0].v_float);
        gboolean *dest = dest_value->data[0].v_pointer = (gboolean *)g_slice_new(MotoBool4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    static void moto_transform_float1_to_int2(const GValue *src_value, GValue *dest_value)
    {
        gint src  = (gint)(src_value->data[0].v_float);
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_float1_to_int3(const GValue *src_value, GValue *dest_value)
    {
        gint src  = (gint)(src_value->data[0].v_float);
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_float1_to_int4(const GValue *src_value, GValue *dest_value)
    {
        gint src  = (gint)(src_value->data[0].v_float);
        gint *dest = dest_value->data[0].v_pointer = (gint *)g_slice_new(MotoInt4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    static void moto_transform_float1_to_float2(const GValue *src_value, GValue *dest_value)
    {
        gfloat src  = src_value->data[0].v_float;
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat2);
        dest[0] = src;
        dest[1] = src;
    }

    static void moto_transform_float1_to_float3(const GValue *src_value, GValue *dest_value)
    {
        gfloat src  = src_value->data[0].v_float;
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat3);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }

    static void moto_transform_float1_to_float4(const GValue *src_value, GValue *dest_value)
    {
        gfloat src  = src_value->data[0].v_float;
        gfloat *dest = dest_value->data[0].v_pointer = (gfloat *)g_slice_new(MotoFloat4);
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
        dest[3] = src;
    }

    // 2 -> *

    static void moto_transform_float2_to_float1(const GValue *src_value, GValue *dest_value)
    {
        gfloat *src = (gfloat *)src_value->data[0].v_pointer;
        g_value_set_float(dest_value, (src[0] + src[1]) / 2);
    }

    // 3 -> *

    static void moto_transform_float3_to_float1(const GValue *src_value, GValue *dest_value)
    {
        gfloat *src = (gfloat *)src_value->data[0].v_pointer;
        g_value_set_float(dest_value, (src[0] + src[1] + src[2]) / 3);
    }

    // 4 -> *

    static void moto_transform_float4_to_float1(const GValue *src_value, GValue *dest_value)
    {
        gfloat *src = (gfloat *)src_value->data[0].v_pointer;
        g_value_set_float(dest_value, (src[0] + src[1] + src[2] + src[3]) / 4);
    }

    // Public functions

    void moto_types_init(void)
    {
        // 1 -> *

        // boolean -> *
        g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_BOOL2, moto_transform_bool1_to_bool2);
        g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_BOOL3, moto_transform_bool1_to_bool3);
    g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_BOOL4, moto_transform_bool1_to_bool4);

    g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_INT2, moto_transform_bool1_to_int2);
    g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_INT3, moto_transform_bool1_to_int3);
    g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_INT4, moto_transform_bool1_to_int4);

    g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_FLOAT2, moto_transform_bool1_to_float2);
    g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_FLOAT3, moto_transform_bool1_to_float3);
    g_value_register_transform_func(MOTO_TYPE_BOOL1, MOTO_TYPE_FLOAT4, moto_transform_bool1_to_float4);

    // int -> *
    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_BOOL2, moto_transform_int1_to_bool2);
    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_BOOL3, moto_transform_int1_to_bool3);
    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_BOOL4, moto_transform_int1_to_bool4);

    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_INT2, moto_transform_int1_to_int2);
    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_INT3, moto_transform_int1_to_int3);
    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_INT4, moto_transform_int1_to_int4);

    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_FLOAT2, moto_transform_int1_to_float2);
    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_FLOAT3, moto_transform_int1_to_float3);
    g_value_register_transform_func(MOTO_TYPE_INT1, MOTO_TYPE_FLOAT4, moto_transform_int1_to_float4);

    // float -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_BOOL2, moto_transform_float1_to_bool2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_BOOL3, moto_transform_float1_to_bool3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_BOOL4, moto_transform_float1_to_bool4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_INT2, moto_transform_float1_to_int2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_INT3, moto_transform_float1_to_int3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_INT4, moto_transform_float1_to_int4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_FLOAT2, moto_transform_float1_to_float2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_FLOAT3, moto_transform_float1_to_float3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT1, MOTO_TYPE_FLOAT4, moto_transform_float1_to_float4);

    /*
    // 2 -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT2, MOTO_TYPE_FLOAT1, moto_transform_float2_to_float1);
    g_value_register_transform_func(MOTO_TYPE_FLOAT2, MOTO_TYPE_FLOAT3, moto_transform_float2_to_float3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT2, MOTO_TYPE_FLOAT4, moto_transform_float2_to_float4);

    // 3 -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT3, MOTO_TYPE_FLOAT1, moto_transform_float3_to_float1);
    g_value_register_transform_func(MOTO_TYPE_FLOAT3, MOTO_TYPE_FLOAT2, moto_transform_float3_to_float2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT3, MOTO_TYPE_FLOAT4, moto_transform_float3_to_float4);

    // 4 -> *
    g_value_register_transform_func(MOTO_TYPE_FLOAT4, MOTO_TYPE_FLOAT1, moto_transform_float4_to_float1);
    g_value_register_transform_func(MOTO_TYPE_FLOAT4, MOTO_TYPE_FLOAT2, moto_transform_float4_to_float2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT4, MOTO_TYPE_FLOAT3, moto_transform_float4_to_float3);
    */

    /*
    // Matrix types trasformations

    // 2x2 -> 2x2
    g_value_register_transform_func(MOTO_TYPE_BOOL2x2, MOTO_TYPE_INT2x2,    moto_transform_bool2x2_to_int2x2);
    g_value_register_transform_func(MOTO_TYPE_BOOL2x2, MOTO_TYPE_FLOAT2x2,  moto_transform_bool2x2_to_float2x2);

    g_value_register_transform_func(MOTO_TYPE_INT2x2, MOTO_TYPE_BOOL2x2, moto_transform_int2x2_to_bool2x2);
    g_value_register_transform_func(MOTO_TYPE_INT2x2, MOTO_TYPE_FLOAT2x2,   moto_transform_int2x2_to_float2x2);

    g_value_register_transform_func(MOTO_TYPE_FLOAT2x2, MOTO_TYPE_BOOL2x2, moto_transform_float2x2_to_bool2x2);
    g_value_register_transform_func(MOTO_TYPE_FLOAT2x2, MOTO_TYPE_INT2x2,     moto_transform_float2x2_to_int2x2);

    // 3x3 -> 3x3
    g_value_register_transform_func(MOTO_TYPE_BOOL3x3, MOTO_TYPE_INT3x3,    moto_transform_bool3x3_to_int3x3);
    g_value_register_transform_func(MOTO_TYPE_BOOL3x3, MOTO_TYPE_FLOAT3x3,  moto_transform_bool3x3_to_float3x3);

    g_value_register_transform_func(MOTO_TYPE_INT3x3, MOTO_TYPE_BOOL3x3, moto_transform_int3x3_to_bool3x3);
    g_value_register_transform_func(MOTO_TYPE_INT3x3, MOTO_TYPE_FLOAT3x3,   moto_transform_int3x3_to_float3x3);

    g_value_register_transform_func(MOTO_TYPE_FLOAT3x3, MOTO_TYPE_BOOL3x3, moto_transform_float3x3_to_bool3x3);
    g_value_register_transform_func(MOTO_TYPE_FLOAT3x3, MOTO_TYPE_INT3x3,     moto_transform_float3x3_to_int3x3);

    // 4x4 -> 4x4
    g_value_register_transform_func(MOTO_TYPE_BOOL4x4, MOTO_TYPE_INT4x4,    moto_transform_bool4x4_to_int4x4);
    g_value_register_transform_func(MOTO_TYPE_BOOL4x4, MOTO_TYPE_FLOAT4x4,  moto_transform_bool4x4_to_float4x4);

    g_value_register_transform_func(MOTO_TYPE_INT4x4, MOTO_TYPE_BOOL4x4, moto_transform_int4x4_to_bool4x4);
    g_value_register_transform_func(MOTO_TYPE_INT4x4, MOTO_TYPE_FLOAT4x4,   moto_transform_int4x4_to_float4x4);

    g_value_register_transform_func(MOTO_TYPE_FLOAT4x4, MOTO_TYPE_BOOL4x4, moto_transform_float4x4_to_bool4x4);
    g_value_register_transform_func(MOTO_TYPE_FLOAT4x4, MOTO_TYPE_INT4x4,     moto_transform_float4x4_to_int4x4);

    */
}

gsize moto_value_get_float_array_size(GValue *value)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_ARRAY);
    MotoFloatArray *p = g_value_peek_pointer(value);
    return p->size;
}

gfloat* moto_value_get_float_array(GValue *value, gsize *size)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_ARRAY);
    MotoFloatArray *p = g_value_peek_pointer(value);
    *size = p->size;
    return p->data;
}

void moto_value_set_float_array(GValue *value, const gfloat* data, gsize size)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT_ARRAY);
    MotoFloatArray *p = g_value_peek_pointer(value);
    if(p->data)
        g_free(p->data);
    p->data = NULL;
    p->size = 0;

    if(data && size > 0)
    {
        g_print("");
        p->data = g_try_malloc(sizeof(gfloat)*size);
        memcpy(p->data, data, sizeof(gfloat)*size);
        p->size = size;
    }
}

// boolean

void moto_value_get_bool2(GValue *value, gboolean *v0, gboolean *v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL2);
    gboolean *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
}

void moto_value_get_bool2_v(GValue *value, gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL2);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gboolean)*2);
}

void moto_value_get_bool3(GValue *value, gboolean *v0, gboolean *v1, gboolean *v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL3);
    gboolean *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
}

void moto_value_get_bool3_v(GValue *value, gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL3);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gboolean)*3);
}

void moto_value_get_bool4(GValue *value, gboolean *v0, gboolean *v1, gboolean *v2, gboolean *v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL4);
    gboolean *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
    *v3 = p[3];
}

void moto_value_get_bool4_v(GValue *value, gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL4);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gboolean)*4);
}

// int

void moto_value_get_int2(GValue *value, gint *v0, gint *v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT2);
    gint *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
}

void moto_value_get_int2_v(GValue *value, gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT2);
    gint *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gint)*2);
}

void moto_value_get_int3(GValue *value, gint *v0, gint *v1, gint *v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT3);
    gint *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
}

void moto_value_get_int3_v(GValue *value, gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT3);
    gint *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gint)*3);
}

void moto_value_get_int4(GValue *value, gint *v0, gint *v1, gint *v2, gint *v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT4);
    gint *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
    *v3 = p[3];
}

void moto_value_get_int4_v(GValue *value, gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT4);
    gint *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gint)*4);
}

// float

void moto_value_get_float2(GValue *value, gfloat *v0, gfloat *v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT2);
    gfloat *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
}

void moto_value_get_float2_v(GValue *value, gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT2);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gfloat)*2);
}

void moto_value_get_float3(GValue *value, gfloat *v0, gfloat *v1, gfloat *v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT3);
    gfloat *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
}

void moto_value_get_float3_v(GValue *value, gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT3);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gfloat)*3);
}

void moto_value_get_float4(GValue *value, gfloat *v0, gfloat *v1, gfloat *v2, gfloat *v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT4);
    gfloat *p = g_value_peek_pointer(value);
    *v0 = p[0];
    *v1 = p[1];
    *v2 = p[2];
    *v3 = p[3];
}

void moto_value_get_float4_v(GValue *value, gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT4);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(v, p, sizeof(gfloat)*4);
}

// boolean

void moto_value_set_bool2(GValue *value, gboolean v0, gboolean v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL2);
    gboolean *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
}

void moto_value_set_bool2_v(GValue *value, const gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL2);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gboolean)*2);
}

void moto_value_set_bool3(GValue *value, gboolean v0, gboolean v1, gboolean v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL3);
    gboolean *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
}

void moto_value_set_bool3_v(GValue *value, const gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL3);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gboolean)*3);
}

void moto_value_set_bool4(GValue *value, gboolean v0, gboolean v1, gboolean v2, gboolean v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL4);
    gboolean *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
    p[3] = v3;
}

void moto_value_set_bool4_v(GValue *value, const gboolean *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_BOOL4);
    gboolean *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gboolean)*4);
}

// int

void moto_value_set_int2(GValue *value, gint v0, gint v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT2);
    gint *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
}

void moto_value_set_int2_v(GValue *value, const gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT2);
    gint *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gint)*2);
}

void moto_value_set_int3(GValue *value, gint v0, gint v1, gint v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT3);
    gint *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
}

void moto_value_set_int3_v(GValue *value, const gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT3);
    gint *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gint)*3);
}

void moto_value_set_int4(GValue *value, gint v0, gint v1, gint v2, gint v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT4);
    gint *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
    p[3] = v3;
}

void moto_value_set_int4_v(GValue *value, const gint *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_INT4);
    gint *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gint)*4);
}

// float

void moto_value_set_float2(GValue *value, gfloat v0, gfloat v1)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT2);
    gfloat *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
}

void moto_value_set_float2_v(GValue *value, const gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT2);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gfloat)*2);
}

void moto_value_set_float3(GValue *value, gfloat v0, gfloat v1, gfloat v2)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT3);
    gfloat *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
}

void moto_value_set_float3_v(GValue *value, const gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT3);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gfloat)*3);
}

void moto_value_set_float4(GValue *value, gfloat v0, gfloat v1, gfloat v2, gfloat v3)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT4);
    gfloat *p = g_value_peek_pointer(value);
    p[0] = v0;
    p[1] = v1;
    p[2] = v2;
    p[3] = v3;
}

void moto_value_set_float4_v(GValue *value, const gfloat *v)
{
    g_assert(G_VALUE_TYPE(value) == MOTO_TYPE_FLOAT4);
    gfloat *p = g_value_peek_pointer(value);
    memcpy(p, v, sizeof(gfloat)*4);
}

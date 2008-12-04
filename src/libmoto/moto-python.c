#include "libmotoutil/numdef.h"
#include "moto-python.h"
#include "moto-types.h"

#define moto_TYPE_SIZE_from_PyObject(TYPE, SIZE, v, obj) \
    if(PyTuple_Check(obj)) \
    { \
        Py_ssize_t size = PyTuple_Size(obj); \
        size = min(SIZE, size); \
        g##TYPE tmp[SIZE]; \
\
        gint i; \
        for(i = 0; i < size; i++) \
        { \
            PyObject *o = PyTuple_GET_ITEM(obj, i); \
            if( ! moto_##TYPE##_from_PyObject(tmp + i, o)) \
                return FALSE; \
        } \
\
        for(i = 0; i < size; i++) \
            v[i] = tmp[i]; \
\
        return TRUE; \
    } \
    else \
    { \
        g##TYPE tmp; \
        if( ! moto_##TYPE##_from_PyObject(& tmp, obj)) \
            return FALSE; \
\
        gint i; \
        for(i = 0; i < SIZE; i++) \
            v[i] = tmp; \
\
        return TRUE;\
    } \
\
    return FALSE

gboolean moto_boolean_from_PyObject(gboolean *v, PyObject *obj)
{
    if(PyInt_Check(obj)) // Booleans in Python are implemented as a subclass of integers.
    {
        v[0] = (gboolean)PyInt_AS_LONG(obj);
        return TRUE;
    }
    else if(PyFloat_Check(obj))
    {
        v[0] = fabs(PyFloat_AS_DOUBLE(obj)) >= MICRO;
        return TRUE;
    }

    return FALSE;
}

gboolean moto_boolean_2_from_PyObject(gboolean *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(boolean, 2, v, obj);
}

gboolean moto_boolean_3_from_PyObject(gboolean *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(boolean, 3, v, obj);
}

gboolean moto_boolean_4_from_PyObject(gboolean *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(boolean, 4, v, obj);
}

gboolean moto_int_from_PyObject(gint *v, PyObject *obj)
{
    if(PyInt_Check(obj))
    {
        v[0] = (gint)PyInt_AS_LONG(obj);
        return TRUE;
    }
    else if(PyFloat_Check(obj))
    {
        v[0] = (gint)PyFloat_AS_DOUBLE(obj);
        return TRUE;
    }

    return FALSE;
}

gboolean moto_int_2_from_PyObject(gint *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(int, 2, v, obj);
}

gboolean moto_int_3_from_PyObject(gint *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(int, 3, v, obj);
}

gboolean moto_int_4_from_PyObject(gint *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(int, 4, v, obj);
}

gboolean moto_float_from_PyObject(gfloat *v, PyObject *obj)
{
    if(PyInt_Check(obj))
    {
        v[0] = (gfloat)PyInt_AS_LONG(obj);
        return TRUE;
    }
    else if(PyFloat_Check(obj))
    {
        v[0] = (gfloat)PyFloat_AS_DOUBLE(obj);
        return TRUE;
    }

    return FALSE;
}

gboolean moto_float_2_from_PyObject(gfloat *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(float, 2, v, obj);
}

gboolean moto_float_3_from_PyObject(gfloat *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(float, 3, v, obj);
}

gboolean moto_float_4_from_PyObject(gfloat *v, PyObject *obj)
{
    moto_TYPE_SIZE_from_PyObject(float, 4, v, obj);
}

/* PyObject -> GValue */

static GHashTable *htable = NULL;

static void destroy_key(gpointer data)
{
    g_slice_free(GType, data);
}

static void free_htable(void)
{
    if(htable)
        g_hash_table_destroy(htable);
}

#define MOTO_INSERT_CONVERTER(G_TYPE, TYPE) \
    key = g_slice_new(GType); \
    *key = G_TYPE; \
    g_hash_table_insert(htable, key, moto_##TYPE##_value_from_PyObject)

#define MOTO_INSERT_VECTOR_CONVERTER(TYPE_PREFIX, TYPE, SIZE) \
    key = g_slice_new(GType); \
    *key = TYPE_PREFIX##_##SIZE; \
    g_hash_table_insert(htable, key, moto_##TYPE##_##SIZE##_value_from_PyObject)

#define DEFINE_CONVERTER(TYPE, SIZE)\
    static gboolean moto_##TYPE##_##SIZE##_value_from_PyObject(GValue *v, PyObject *obj) \
    {\
        return moto_##TYPE##_##SIZE##_from_PyObject((g##TYPE *)v->data[0].v_pointer, obj); \
    }

// Converters

static gboolean moto_int_value_from_PyObject(GValue *v, PyObject *obj)
{
    return moto_int_from_PyObject( & v->data[0].v_int, obj);
}

static gboolean moto_float_value_from_PyObject(GValue *v, PyObject *obj)
{
    return moto_float_from_PyObject( & v->data[0].v_float, obj);
}

static gboolean moto_boolean_value_from_PyObject(GValue *v, PyObject *obj)
{
    return moto_boolean_from_PyObject( & v->data[0].v_int, obj);
}

DEFINE_CONVERTER(boolean, 2)
DEFINE_CONVERTER(boolean, 3)
DEFINE_CONVERTER(boolean, 4)
DEFINE_CONVERTER(int, 2)
DEFINE_CONVERTER(int, 3)
DEFINE_CONVERTER(int, 4)
DEFINE_CONVERTER(float, 2)
DEFINE_CONVERTER(float, 3)
DEFINE_CONVERTER(float, 4)

/*
static gboolean moto_string_value_from_PyObject(GValue *v, PyObject *obj)
{
    return moto_string_from_PyObject( & v->data[0].v_int, obj);
}
*/

gboolean moto_GValue_from_PyObject(GValue *v, PyObject *obj)
{
    if( ! htable)
    {
        htable = g_hash_table_new_full(g_int_hash, g_int_equal,
                                       destroy_key, NULL);
        GType *key;

        MOTO_INSERT_CONVERTER(G_TYPE_BOOLEAN, boolean);
        MOTO_INSERT_CONVERTER(G_TYPE_INT, int);
        MOTO_INSERT_CONVERTER(G_TYPE_FLOAT, float);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_BOOLEAN, boolean, 2);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_BOOLEAN, boolean, 3);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_BOOLEAN, boolean, 4);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_INT, int, 2);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_INT, int, 3);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_INT, int, 4);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_FLOAT, float, 2);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_FLOAT, float, 3);
        MOTO_INSERT_VECTOR_CONVERTER(MOTO_TYPE_FLOAT, float, 4);

        atexit(free_htable);
    }

    GType type = G_VALUE_TYPE(v);
    MotoGValueFromPyObjectFunc func = g_hash_table_lookup(htable, & type);
    if(func)
        return func(v, obj);

    return FALSE;
}

#undef MOTO_INSERT_CONVERTER
#undef MOTO_INSERT_VECTOR_CONVERTER
#undef moto_TYPE_SIZE_from_PyObject

#include "moto-python.h"

#include "libmotoutil/numdef.h"
#include "moto-types.h"
#include "moto-filename.h"

// Py -> moto

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
    else if(PySequence_Check(obj)) \
    { \
        Py_ssize_t size = PySequence_Size(obj); \
        size = min(SIZE, size); \
        g##TYPE tmp[SIZE]; \
\
        gint i; \
        for(i = 0; i < size; i++) \
        { \
            PyObject *o = PySequence_GetItem(obj, i); \
            if( ! moto_##TYPE##_from_PyObject(tmp + i, o)) \
                return FALSE; \
            Py_DECREF(o);\
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
    else if(PySequence_Check(obj))
    {
        v[0] = PySequence_Size(obj) > 0;
        return TRUE;
    }
    else
    {
        v[0] = obj != Py_None;
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

gboolean moto_string_from_PyObject(gchar **v, PyObject *obj)
{
    if(PyString_Check(obj))
    {
        *v = PyString_AS_STRING(obj);

        return TRUE;
    }

    return FALSE;
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
        g_hash_table_unref(htable);
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

static gboolean moto_string_value_from_PyObject(GValue *v, PyObject *obj)
{
    return moto_string_from_PyObject((gchar **)(& v->data[0].v_pointer), obj);
}

typedef struct _HTableData
{
    GType type;
    MotoGValueFromPyObjectFunc func;
} HTableData;

static void convert_value(gpointer key, gpointer value, gpointer user_data)
{
    HTableData *data = (HTableData *)user_data;
    if(data->func)
        return;

    if(g_type_is_a(data->type, *((GType *)key)))
        data->func = g_hash_table_lookup(htable, key);
}

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
        MOTO_INSERT_CONVERTER(G_TYPE_STRING, string);

        atexit(free_htable);
    }

    GType type = G_VALUE_TYPE(v);
    MotoGValueFromPyObjectFunc func = g_hash_table_lookup(htable, & type);
    if(func)
    {
        return func(v, obj);
    }
    else
    {
        HTableData data;
        data.type = type;
        data.func = NULL;
        g_hash_table_foreach(htable, convert_value, & data);

        if(data.func)
            return data.func(v, obj);
    }

    return FALSE;
}

#undef MOTO_INSERT_CONVERTER
#undef MOTO_INSERT_VECTOR_CONVERTER
#undef moto_TYPE_SIZE_from_PyObject

// Creating function for expression

PyObject *moto_PyFunction_from_args_and_body(const gchar *argsdef, const gchar *body)
{
    GString *code = g_string_new("");

    const gchar *indent = "    ";

    const gchar *end = g_utf8_strchr(body, -1, g_utf8_get_char("\n"));
    if( ! end)
    {
        // Single-line expression.
        g_string_printf(code, "def func%s:\n    return %s\n", argsdef, body);
    }
    else
    {
        // Multi-line expression.
        g_string_printf(code, "def func%s:\n", argsdef);

        const gchar *line = body;

        g_string_append(code, indent);
        g_string_append_len(code, line, g_utf8_pointer_to_offset(line, end));

        line = g_utf8_next_char(end);
        end = g_utf8_strchr(line, -1, g_utf8_get_char("\n"));

        while(end)
        {
            g_string_append(code, indent);
            g_string_append_len(code, line, g_utf8_pointer_to_offset(line, end));

            line = g_utf8_next_char(end);
            end = g_utf8_strchr(line, -1, g_utf8_get_char("\n"));
        }
        g_string_append(code, "\n");
    }

    PyObject *module   = NULL;
    PyObject *func     = NULL;
    PyObject *compiled = Py_CompileString(code->str, "__moto__", Py_file_input);
    if(compiled)
    {
        module = PyImport_ExecCodeModule("moto.__moto__", compiled);
        if(module)
            func = PyObject_GetAttrString(module, "func");
    }
    else
    {
        g_print("PyErr_Occured(): %d\n", PyErr_Occurred());
        PyErr_Clear();
    }

error:
    Py_XDECREF(module);
    Py_XDECREF(compiled);

    g_string_free(code, TRUE);
    return func;
}

// moto -> Py

PyObject *moto_PyObject_from_GValue(GValue *v)
{
    PyObject *obj = NULL;

    GType type = G_VALUE_TYPE(v);
    if(g_type_is_a(type, G_TYPE_BOOLEAN))
    {
        obj = PyBool_FromLong(v->data[0].v_int);
    }
    if(g_type_is_a(type, G_TYPE_INT))
    {
        obj = PyInt_FromLong(v->data[0].v_int);
    }
    else if(g_type_is_a(type, G_TYPE_FLOAT))
    {
        obj = PyFloat_FromDouble(v->data[0].v_float);
    }
    else if(g_type_is_a(type, MOTO_TYPE_BOOLEAN_2))
    {
        gboolean *tmp = (gboolean *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(bb)", tmp[0], tmp[1]);
    }
    else if(g_type_is_a(type, MOTO_TYPE_INT_2))
    {
        gint *tmp = (gint *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(ii)", tmp[0], tmp[1]);
    }
    else if(g_type_is_a(type, MOTO_TYPE_FLOAT_2))
    {
        gfloat *tmp = (gfloat *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(ff)", tmp[0], tmp[1]);
    }
    else if(g_type_is_a(type, MOTO_TYPE_BOOLEAN_3))
    {
        gboolean *tmp = (gboolean *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(bbb)", tmp[0], tmp[1], tmp[2]);
    }
    else if(g_type_is_a(type, MOTO_TYPE_INT_3))
    {
        gint *tmp = (gint *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(iii)", tmp[0], tmp[1], tmp[2]);
    }
    else if(g_type_is_a(type, MOTO_TYPE_FLOAT_3))
    {
        gfloat *tmp = (gfloat *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(fff)", tmp[0], tmp[1], tmp[2]);
    }
    else if(g_type_is_a(type, MOTO_TYPE_BOOLEAN_4))
    {
        gboolean *tmp = (gboolean *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(bbbb)", tmp[0], tmp[1], tmp[2], tmp[3]);
    }
    else if(g_type_is_a(type, MOTO_TYPE_INT_4))
    {
        gint *tmp = (gint *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(iiii)", tmp[0], tmp[1], tmp[2], tmp[3]);
    }
    else if(g_type_is_a(type, MOTO_TYPE_FLOAT_4))
    {
        gfloat *tmp = (gfloat *)g_value_peek_pointer(v);
        obj = Py_BuildValue("(ffff)", tmp[0], tmp[1], tmp[2], tmp[3]);
    }
    else if(g_type_is_a(type, G_TYPE_STRING))
    {
        obj = PyString_FromString(g_value_get_string(v));
    }
    else if(g_type_is_a(type, G_TYPE_OBJECT))
    {
        obj = NULL; // TODO
    }

    if( ! obj)
        Py_RETURN_NONE;

    return obj;
}

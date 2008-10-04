#include "libmotoutil/numdef.h"

#include "moto-ipo-segment.h"
#include "moto-messager.h"

/* IpoFunction */
/* TODO: Must be reimplemented. */

MotoIpoFunction *moto_ipo_function_new(const gchar *name, MotoIpoFunctionGetValueFunc func)
{
    MotoIpoFunction *self = (MotoIpoFunction *)g_try_malloc(sizeof(MotoIpoFunction));
    self->name = g_string_new(name);
    self->get_value = func;

    return self;
}

void moto_ipo_function_free(MotoIpoFunction *self)
{
    g_string_free(self->name, TRUE);
    g_free(self);
}

gfloat linear_ipo_get_value(MotoIpoSegment *ipo_segment, gfloat arg)
{
    gfloat length = ipo_segment->_end_x - ipo_segment->_start_x;
    if(length < MICRO)
        return ipo_segment->_start_y;

    return ipo_segment->_start_y*(arg - ipo_segment->_start_x)/length + \
           ipo_segment->_end_y*(ipo_segment->_end_x - arg)/length;
}

gfloat bezier_ipo_get_value(MotoIpoSegment *ipo_segment, gfloat arg)
{
    /* TODO: Temporary using of linear interpolation. */
    return linear_ipo_get_value(ipo_segment, arg);
}

static GSList *_ipo_functions = NULL;
static gboolean _ipo_functions_initialized = FALSE;

static MotoIpoFunction *get_ipo_function(const gchar *func_name)
{
    if( ! _ipo_functions_initialized)
    {
        _ipo_functions = g_slist_append(_ipo_functions,
                moto_ipo_function_new("linear", linear_ipo_get_value));
        _ipo_functions = g_slist_append(_ipo_functions,
                moto_ipo_function_new("bezier", bezier_ipo_get_value));

        _ipo_functions_initialized = TRUE;
    }

    GSList *l = _ipo_functions;
    for(; l; l = g_slist_next(l))
    {
        if(g_utf8_collate(func_name, ((MotoIpoFunction *)l->data)->name->str) == 0)
            return (MotoIpoFunction *)l->data;
    }

    return NULL;
}

/* class IpoSegment */

static GObjectClass *ipo_segment_parent_class = NULL;

static void
moto_ipo_segment_dispose(GObject *obj)
{
    ipo_segment_parent_class->dispose(obj);
}

static void
moto_ipo_segment_finalize(GObject *obj)
{
    ipo_segment_parent_class->finalize(obj);
}

static void
moto_ipo_segment_init(MotoIpoSegment *self)
{
    self->_start_x = 0;
    self->_end_x = 0;
    self->_start_y = 0;
    self->_end_y = 0;

    self->func_object = NULL;
    self->func = NULL;
}

static void
moto_ipo_segment_class_init(MotoIpoSegmentClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    ipo_segment_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_ipo_segment_dispose;
    goclass->finalize   = moto_ipo_segment_finalize;
}

G_DEFINE_TYPE(MotoIpoSegment, moto_ipo_segment, G_TYPE_OBJECT);

/* methods of class IpoSegment */

MotoIpoSegment *moto_ipo_segment_new(const gchar *func_name)
{
    MotoIpoFunction *func_object = get_ipo_function(func_name);
    if( ! func_object)
    {
        GString *msg = g_string_new("");
        g_string_printf(msg, "I can't create interpolation segment. Interpolation function  \"%s\" does not exist.", func_name);
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return NULL;
    }

    MotoIpoSegment *self = (MotoIpoSegment *)g_object_new(MOTO_TYPE_IPO_SEGMENT, NULL);
    self->func_object = func_object;
    self->func = func_object->get_value;

    return self;
}

gfloat moto_ipo_segment_get_value(MotoIpoSegment *self, gfloat arg)
{
    return self->func(self, arg);
}

void moto_ipo_segment_set_function(MotoIpoSegment *self, const gchar *func_name)
{
    MotoIpoFunction *func_object = get_ipo_function(func_name);
    if( ! func_object)
    {
        GString *msg = g_string_new("");
        g_string_printf(msg, "I can't set function for interpolation segment. Interpolation function \"%s\" does not exist.", func_name);
        moto_error(msg->str);
        g_string_free(msg, TRUE);
    }

    self->func_object = func_object;
    self->func = func_object->get_value;
}

gchar *moto_ipo_segment_get_function_name(MotoIpoSegment *self)
{
    if( ! self->func_object)
        return NULL;
    return self->func_object->name->str;
}


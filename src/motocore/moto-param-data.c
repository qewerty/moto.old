#include "moto-param-data.h"

/* class ParamData */

static GObjectClass *param_data_parent_class = NULL;

static void
moto_param_data_dispose(GObject *obj)
{
    param_data_parent_class->dispose(obj);
}

static void
moto_param_data_finalize(GObject *obj)
{
    param_data_parent_class->finalize(obj);
}

static void
moto_param_data_init(MotoParamData *self)
{
    self->param_func     = NULL;
    self->point_func     = NULL;
    self->update_func    = NULL;
    self->get_func       = NULL;
    self->set_func       = NULL;
}

static void
moto_param_data_class_init(MotoParamDataClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_param_data_dispose;
    goclass->finalize   = moto_param_data_finalize;
}

G_DEFINE_ABSTRACT_TYPE(MotoParamData, moto_param_data, G_TYPE_OBJECT);

/* methods of class ParamData */

void moto_param_data_point(MotoParamData *self, gpointer p)
{
    if(self->point_func)
        self->point_func(self->param, p);
}

void moto_param_data_update(MotoParamData *self)
{
    if(self->update_func)
        self->update_func(self->param);
}

gpointer moto_param_data_get(MotoParamData *self)
{
    if(self->get_func)
        return self->get_func(self->param);
    return NULL;
}

void moto_param_data_set(MotoParamData *self, gpointer p)
{
    if(self->set_func)
        self->set_func(self->param, p);
}

void moto_param_data_set_cbs(MotoParamData *self,
        MotoParamDataPointFunc point, MotoParamDataUpdateFunc update,
        MotoParamDataGetFunc get, MotoParamDataSetFunc set)
{
    self->point_func     = point;
    self->update_func    = update;
    self->get_func       = get;
    self->set_func       = set;
}

#include "moto-param-data.h"

/* class ParamData */

static GObjectClass *param_data_parent_class = NULL;

static void
moto_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(param_data_parent_class)->dispose(obj);
}

static void
moto_param_data_finalize(GObject *obj)
{
    param_data_parent_class->finalize(obj);
}

static void
moto_param_data_init(MotoParamData *self)
{
    self->param     = NULL;
    self->point     = NULL;
    self->update    = NULL;
    self->get       = NULL;
    self->set       = NULL;
}

static void
moto_param_data_class_init(MotoParamDataClass *klass)
{
    param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    param_data_parent_class->dispose = moto_param_data_dispose;
    param_data_parent_class->finalize = moto_param_data_finalize;
}

G_DEFINE_ABSTRACT_TYPE(MotoParamData, moto_param_data, G_TYPE_OBJECT);

/* methods of class ParamData */

void moto_param_data_update(MotoParamData *self)
{
    if(self->point)
        self->point(self->param);
}

void moto_param_data_point(MotoParamData *self, gpointer p)
{
    if(self->update)
        self->update(self->param, p);
}

gpointer moto_param_data_get(MotoParamData *self)
{
    if(self->get)
        return self->get(self->param);
}

void moto_param_data_set(MotoParamData *self, gpointer p)
{
    if(self->set)
        self->set(self->param, p);
}


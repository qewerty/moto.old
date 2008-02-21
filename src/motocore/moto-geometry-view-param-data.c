#include "moto-geometry-view-param-data.h"

/* class GeometryViewParamData */

static GObjectClass *geometry_view_param_data_parent_class = NULL;

static void
moto_geometry_view_param_data_dispose(GObject *obj)
{
    G_OBJECT_CLASS(geometry_view_param_data_parent_class)->dispose(obj);
}

static void
moto_geometry_view_param_data_finalize(GObject *obj)
{
    geometry_view_param_data_parent_class->finalize(obj);
}

static void
moto_geometry_view_param_data_init(MotoGeometryViewParamData *self)
{
    self->value = self->default_value = FALSE;
}

static void
moto_geometry_view_param_data_class_init(MotoGeometryViewParamDataClass *klass)
{
    geometry_view_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    geometry_view_param_data_parent_class->dispose = moto_geometry_view_param_data_dispose;
    geometry_view_param_data_parent_class->finalize = moto_geometry_view_param_data_finalize;
}

G_DEFINE_TYPE(MotoGeometryViewParamData, moto_geometry_view_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class GeometryViewParamData */

MotoParamData *
moto_geometry_view_param_data_new(MotoParamDataGetFunc get,
        MotoParamDataSetFunc set, MotoGeometryViewNode *default_value)
{
    MotoGeometryViewParamData *self = \
        (MotoGeometryViewParamData *)g_object_new(MOTO_TYPE_GEOMETRY_VIEW_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    data->get = get;
    data->set = set;

    self->value = self->default_value = default_value;

    return data;
}

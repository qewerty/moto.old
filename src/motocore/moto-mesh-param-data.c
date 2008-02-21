#include "moto-mesh-param-data.h"

/* class MeshParamData */

static GObjectClass *mesh_param_data_parent_class = NULL;

static void
moto_mesh_param_data_dispose(GObject *obj)
{
    mesh_param_data_parent_class->dispose(obj);
}

static void
moto_mesh_param_data_finalize(GObject *obj)
{
    mesh_param_data_parent_class->finalize(obj);
}

static void
moto_mesh_param_data_init(MotoMeshParamData *self)
{
    self->value = self->default_value = NULL;
}

static void
moto_mesh_param_data_class_init(MotoMeshParamDataClass *klass)
{
    mesh_param_data_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    mesh_param_data_parent_class->dispose = moto_mesh_param_data_dispose;
    mesh_param_data_parent_class->finalize = moto_mesh_param_data_finalize;
}

G_DEFINE_TYPE(MotoMeshParamData, moto_mesh_param_data, MOTO_TYPE_PARAM_DATA);

/* methods of class MeshParamData */

MotoParamData *
moto_mesh_param_data_new(MotoParamDataGetFunc get,
        MotoParamDataSetFunc set, MotoMesh *default_value)
{
    MotoMeshParamData *self = \
        (MotoMeshParamData *)g_object_new(MOTO_TYPE_MESH_PARAM_DATA, NULL);
    MotoParamData *data = (MotoParamData *)self;

    data->get = get;
    data->set = set;

    self->value = self->default_value = default_value;

    return data;
}

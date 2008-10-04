#include "moto-transform-info.h"

void moto_transform_info_copy(MotoTransformInfo *self, MotoTransformInfo *other)
{
    int i;
    for(i = 0; i < 16; i++)
        self->model[i] = other->model[i];
    for(i = 0; i < 16; i++)
        self->proj[i] = other->proj[i];
    for(i = 0; i < 4; i++)
        self->view[i] = other->view[i];
}

void moto_transform_info_set_model(MotoTransformInfo *self, GLdouble model[16])
{
    int i;
    for(i = 0; i < 16; i++)
        self->model[i] = model[i];
}

void moto_transform_info_set_proj(MotoTransformInfo *self, GLdouble proj[16])
{
    int i;
    for(i = 0; i < 16; i++)
        self->proj[i] = proj[i];
}

void moto_transform_info_set_view(MotoTransformInfo *self, GLint view[4])
{
    int i;
    for(i = 0; i < 4; i++)
        self->view[i] = view[i];
}

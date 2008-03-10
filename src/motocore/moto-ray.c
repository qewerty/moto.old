#include "moto-ray.h"
#include "common/matrix.h"

void moto_ray_copy(MotoRay *self, MotoRay *other)
{
    vector3_copy(self->pos, other->pos);
    vector3_copy(self->dir, other->dir);
}

void moto_ray_transform(MotoRay *self, float m[16])
{
    float p[3], d[3];
    point3_transform(p, m, ray->pos);
    vector3_transform(d, m, ray->dir);
    vector3_copy(ray->pos, p);
    vector3_copy(ray->dir, d);
}

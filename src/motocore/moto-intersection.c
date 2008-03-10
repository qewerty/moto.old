#include "moto-intersection.h"

void moto_intersection_reset(MotoIntersection *self)
{
    self->hit = 0;
    self->dist = 0;
}

void moto_intersection_copy(MotoIntersection *self, MotoIntersection *other)
{
    self->hit = other->hit;
    self->dist = other->dist;
}

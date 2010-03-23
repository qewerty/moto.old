#include <assert.h>

#include "moto-intersection.h"
#include "libmotoutil/xform.h"

void moto_intersection_reset(MotoIntersection *self)
{
    self->hits_num = 0;
}

void moto_intersection_copy(MotoIntersection *self, MotoIntersection *other)
{
    self->hits_num = other->hits_num;
    assert(self->hits_num <= MOTO_INTERSECTION_MAX_HITS);

    unsigned int i;
    for(i = 0; i < self->hits_num; i++)
    {
        self->hits[i].dist = other->hits[i].dist;
        vector3_copy(self->hits[i].point, other->hits[i].point);
        vector3_copy(self->hits[i].normal, other->hits[i].normal);
        self->hits[i].is_entering = other->hits[i].is_entering;
    }
}

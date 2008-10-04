#include "moto-bound.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* class Bound */

static GObjectClass *bound_parent_class = NULL;

/*
static void
moto_bound_dispose(GObject *obj)
{
    MotoBound *self = (MotoBound *)obj;

    G_OBJECT_CLASS(bound_parent_class)->dispose(obj);
}

static void
moto_bound_finalize(GObject *obj)
{
    bound_parent_class->finalize(obj);
}
*/

static void
moto_bound_init(MotoBound *self)
{
    self->bound[0] = 0;
    self->bound[1] = 0;
    self->bound[2] = 0;
    self->bound[3] = 0;
    self->bound[4] = 0;
    self->bound[5] = 0;
}

static void
moto_bound_class_init(MotoBoundClass *klass)
{
    // GObjectClass *goclass = (GObjectClass *)klass;

    bound_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    /*
    goclass->dispose    = moto_bound_dispose;
    goclass->finalize   = moto_bound_finalize;
    */
}

G_DEFINE_TYPE(MotoBound, moto_bound, G_TYPE_OBJECT);

/* methods of class Bound */

MotoBound *moto_bound_new(gfloat min_x, gfloat max_x,
                          gfloat min_y, gfloat max_y,
                          gfloat min_z, gfloat max_z)
{
    MotoBound *self = (MotoBound *)g_object_new(MOTO_TYPE_BOUND, NULL);

    self->bound[0] = min_x;
    self->bound[1] = max_x;
    self->bound[2] = min_y;
    self->bound[3] = max_y;
    self->bound[4] = min_z;
    self->bound[5] = max_z;

    return self;
}

MotoBound *moto_bound_new_from_array(gfloat array[6])
{
    MotoBound *self = (MotoBound *)g_object_new(MOTO_TYPE_BOUND, NULL);

    self->bound[0] = array[0];
    self->bound[1] = array[1];
    self->bound[2] = array[2];
    self->bound[3] = array[3];
    self->bound[4] = array[4];
    self->bound[5] = array[5];

    return self;
}

void moto_bound_copy(MotoBound *self, MotoBound *other)
{
    self->bound[0] = other->bound[0];
    self->bound[1] = other->bound[1];
    self->bound[2] = other->bound[2];
    self->bound[3] = other->bound[3];
    self->bound[4] = other->bound[4];
    self->bound[5] = other->bound[5];
}

void moto_bound_set_extended(MotoBound *self, MotoBound *other, gfloat extent)
{
    self->bound[0] = other->bound[0] - extent;
    self->bound[1] = other->bound[1] + extent;
    self->bound[2] = other->bound[2] - extent;
    self->bound[3] = other->bound[3] + extent;
    self->bound[4] = other->bound[4] - extent;
    self->bound[5] = other->bound[5] + extent;
}

void moto_bound_print(MotoBound *self)
{
    g_print("min_x: %f; max_x: %f\n", self->bound[0], self->bound[1]);
    g_print("min_y: %f; max_y: %f\n", self->bound[2], self->bound[3]);
    g_print("min_z: %f; max_z: %f\n", self->bound[4], self->bound[5]);
}

gboolean moto_bound_intersect_ray(MotoBound *self, MotoRay *ray,
        MotoIntersection *intersection)
{
    return moto_ray_intersect_bound(ray, intersection, self->bound);
}

gboolean moto_bound_is_valid(MotoBound *self)
{
    if(self->bound[0] >= self->bound[1])
        return FALSE;

    if(self->bound[2] >= self->bound[3])
        return FALSE;

    if(self->bound[4] >= self->bound[5])
        return FALSE;

    return TRUE;
}

void moto_bound_draw(MotoBound *self)
{
    // TODO
}

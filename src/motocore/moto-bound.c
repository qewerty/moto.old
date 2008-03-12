#include "moto-bound.h"
#include "common/matrix.h"
#include "common/numdef.h"

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

gboolean moto_bound_intersect_ray(MotoBound *self, MotoRay *ray,
        MotoIntersection *intersection)
{
    return moto_ray_intersect(ray, intersection, self->bound);
}

gboolean moto_bound_is_valid(MotoBound *self)
{
    if(self->min_x >= self->max_x)
        return FALSE;

    if(self->min_y >= self->max_y)
        return FALSE;

    if(self->min_z >= self->max_z)
        return FALSE;

    return TRUE;
}

void moto_bound_draw(MotoBound *self)
{
    // TODO
}

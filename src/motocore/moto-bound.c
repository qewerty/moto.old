#include "moto-bound.h"
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
    self->min_x = 0;
    self->max_x = 0;
    self->min_y = 0;
    self->max_y = 0;
    self->min_z = 0;
    self->max_z = 0;
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

    self->min_x = min_x;
    self->max_x = max_x;
    self->min_y = min_y;
    self->max_y = max_y;
    self->min_z = min_z;
    self->max_z = max_z;

    return self;
}

MotoBound *moto_bound_new_from_array(gfloat array[6])
{
    return moto_bound_new(array[0], array[1], array[2],
                          array[3], array[4], array[5]);
}

gboolean moto_bound_intersect(MotoBound *self, MotoRay *ray,
        MotoIntersection *intersection)
{
    gfloat t_hit, numer, denom;

    gfloat t_in  = -100000;
    gfloat t_out = 100000;

    gint in_surf, out_surf;

    gint i;
    for(i = 0; i < 6; i++)
    {
        switch(i)
        {
            case 0:
                numer = fabs(self->max_y) - ray->pos[1];
                denom = ray->dir[1];
            break;
            case 1:
                numer = fabs(self->min_y) + ray->pos[1];
                denom = -ray->dir[1];
            break;
            case 2:
                numer = fabs(self->max_x) - ray->pos[0];
                denom = ray->dir[0];
            break;
            case 3:
                numer = fabs(self->min_x) + ray->pos[0];
                denom = -ray->dir[0];
            break;
            case 4:
                numer = fabs(self->max_z) - ray->pos[2];
                denom = ray->dir[2];
            break;
            case 5:
                numer = fabs(self->min_z) + ray->pos[2];
                denom = -ray->dir[2];
            break;
        }

        if(fabs(denom) < MICRO)
        {
            if(numer < 0) return FALSE;
        }
        else
        {
            t_hit = numer / denom;
            if(denom > 0)
            {
                if(t_hit < t_out)
                {
                    t_out = t_hit;
                    out_surf = i;
                }
            }
            else
            {
                if(t_hit > t_in)
                {
                    t_in = t_hit;
                    in_surf = i;
                }
            }
        }

        if(t_in >= t_out)
            return FALSE;
    }

    gint num = 0;



    return num > 0;
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

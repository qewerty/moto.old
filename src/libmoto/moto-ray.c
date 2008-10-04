#include <stdio.h>

#include "moto-ray.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* macros */

#define cube_normal(n, i)\
    switch(i)\
    {\
        case 0: vector3_set(n, 0, 1, 0); break;\
        case 1: vector3_set(n, 0, -1, 0); break;\
        case 2: vector3_set(n, 1, 0, 0); break;\
        case 3: vector3_set(n, -1, 0, 0); break;\
        case 4: vector3_set(n, 0, 0, 1); break;\
        case 5: vector3_set(n, 0, 0, -1);\
    }

#define min_x bound[0]
#define max_x bound[1]
#define min_y bound[2]
#define max_y bound[3]
#define min_z bound[4]
#define max_z bound[5]

/* MotoRay */

void moto_ray_copy(MotoRay *self, MotoRay *other)
{
    vector3_copy(self->pos, other->pos);
    vector3_copy(self->dir, other->dir);
}

void moto_ray_transform(MotoRay *self, float m[16])
{
    float p[3], d[3];
    point3_transform(p, m, self->pos);
    vector3_transform(d, m, self->dir);
    vector3_copy(self->pos, p);
    vector3_copy(self->dir, d);
}

void moto_ray_set_transformed(MotoRay *self, MotoRay *other, float m[16])
{
    point3_transform(self->pos, m, other->pos);
    vector3_transform(self->dir, m, other->dir);
}

void moto_ray_normalize(MotoRay *self)
{
    float lenbuf;
    vector3_normalize(self->dir, lenbuf);
}

int moto_ray_intersect_plane(MotoRay *self,
        MotoIntersection *intersection,
        float point[3], float normal[3])
{
    float dif[3];
    vector3_dif(dif, point, self->pos);
    float numer = vector3_dot(normal, dif);
    float denom = vector3_dot(normal, self->dir);

    if(fabs(denom) < MICRO) /* parallel */
    {
        return 0;
    }

    float t = numer/denom;

    if(t < MICRO)
    {
        return 0;
    }

    intersection->hits[0].dist = t;
    intersection->hits[0].is_entering = 1;

    vector3_copy(intersection->hits[0].point, self->pos);
    point3_move(intersection->hits[0].point, self->dir, t);
    vector3_copy(intersection->hits[0].normal, normal);

    intersection->hits_num = 1;

    return 1;
}

int moto_ray_intersect_plane_check(MotoRay *self,
        float point[3], float normal[3])
{
    float dif[3];
    vector3_dif(dif, point, self->pos);
    float numer = vector3_dot(normal, dif);
    float denom = vector3_dot(normal, self->dir);

    if(fabs(denom) < MICRO) /* parallel */
    {
        return 0;
    }

    if(numer/denom < MICRO)
    {
        return 0;
    }

    return 1;
}

int moto_ray_intersect_plane_dist(MotoRay *self,
        float *dist,
        float point[3],
        float normal[3])
{
    float dif[3];
    vector3_dif(dif, point, self->pos);
    float numer = vector3_dot(normal, dif);
    float denom = vector3_dot(normal, self->dir);

    if(fabs(denom) < MICRO) /* parallel */
    {
        return 0;
    }

    *dist = numer/denom;

    if(*dist < MICRO)
    {
        return 0;
    }

    return 1;
}

/* TODO: Needs to be optimized? */
int moto_ray_intersect_triangle(MotoRay *self,
        MotoIntersection *intersection,
        float A[3], float B[3], float C[3])
{
    float tmp;
    float normal[3], v1[3], v2[3];
    vector3_dif(v1, B, A);
    vector3_dif(v2, C, A);

    vector3_cross(normal, v1, v2);
    vector3_normalize(normal, tmp);

    if( ! moto_ray_intersect_plane(self, intersection, A, normal))
        return 0;

    float n1[3], n2[3], n3[3], oA[3], oB[3], oC[3];
    vector3_dif(oA, A, self->pos);
    vector3_dif(oB, B, self->pos);
    vector3_dif(oC, C, self->pos);

    if(vector3_dot(self->dir, normal) < 0) /* faceforward */
    {
        vector3_cross(n1, oC, oA);
        vector3_normalize(n1, tmp);
        if(vector3_dot(n1, self->dir) > 0)
            return 0;

        vector3_cross(n2, oB, oC);
        vector3_normalize(n2, tmp);
        if(vector3_dot(n2, self->dir) > 0)
            return 0;

        vector3_cross(n3, oA, oB);
        vector3_normalize(n3, tmp);
        if(vector3_dot(n3, self->dir) > 0)
            return 0;
    }
    else
    {
        vector3_cross(n1, oA, oC);
        vector3_normalize(n1, tmp);
        if(vector3_dot(n1, self->dir) > 0)
            return 0;

        vector3_cross(n2, oC, oB);
        vector3_normalize(n2, tmp);
        if(vector3_dot(n2, self->dir) > 0)
            return 0;

        vector3_cross(n3, oB, oA);
        vector3_normalize(n3, tmp);
        if(vector3_dot(n3, self->dir) > 0)
            return 0;
    }

    return 1;
}

int moto_ray_intersect_triangle_check(MotoRay *self,
        float A[3], float B[3], float C[3])
{
    float tmp;
    float normal[3], v1[3], v2[3];
    vector3_dif(v1, B, A);
    vector3_dif(v2, C, A);

    vector3_cross(normal, v1, v2);
    vector3_normalize(normal, tmp);

    if( ! moto_ray_intersect_plane_check(self, A, normal))
        return 0;

    float n1[3], n2[3], n3[3], oA[3], oB[3], oC[3];
    vector3_dif(oA, A, self->pos);
    vector3_dif(oB, B, self->pos);
    vector3_dif(oC, C, self->pos);

    if(vector3_dot(self->dir, normal) < 0) /* faceforward */
    {
        vector3_cross(n1, oC, oA);
        vector3_normalize(n1, tmp);
        if(vector3_dot(n1, self->dir) > 0)
            return 0;

        vector3_cross(n2, oB, oC);
        vector3_normalize(n2, tmp);
        if(vector3_dot(n2, self->dir) > 0)
            return 0;

        vector3_cross(n3, oA, oB);
        vector3_normalize(n3, tmp);
        if(vector3_dot(n3, self->dir) > 0)
            return 0;
    }
    else
    {
        vector3_cross(n1, oA, oC);
        vector3_normalize(n1, tmp);
        if(vector3_dot(n1, self->dir) > 0)
            return 0;

        vector3_cross(n2, oC, oB);
        vector3_normalize(n2, tmp);
        if(vector3_dot(n2, self->dir) > 0)
            return 0;

        vector3_cross(n3, oB, oA);
        vector3_normalize(n3, tmp);
        if(vector3_dot(n3, self->dir) > 0)
            return 0;
    }

    return 1;
}

int moto_ray_intersect_triangle_dist(MotoRay *self,
        float *dist, float A[3], float B[3], float C[3])
{
    float tmp;
    float normal[3], v1[3], v2[3];
    vector3_dif(v1, B, A);
    vector3_dif(v2, C, A);

    vector3_cross(normal, v1, v2);
    vector3_normalize(normal, tmp);

    if( ! moto_ray_intersect_plane_dist(self, dist, A, normal))
        return 0;

    float n1[3], n2[3], n3[3], oA[3], oB[3], oC[3];
    vector3_dif(oA, A, self->pos);
    vector3_dif(oB, B, self->pos);
    vector3_dif(oC, C, self->pos);

    if(vector3_dot(self->dir, normal) < 0) /* faceforward */
    {
        vector3_cross(n1, oC, oA);
        vector3_normalize(n1, tmp);
        if(vector3_dot(n1, self->dir) > 0)
            return 0;

        vector3_cross(n2, oB, oC);
        vector3_normalize(n2, tmp);
        if(vector3_dot(n2, self->dir) > 0)
            return 0;

        vector3_cross(n3, oA, oB);
        vector3_normalize(n3, tmp);
        if(vector3_dot(n3, self->dir) > 0)
            return 0;
    }
    else
    {
        vector3_cross(n1, oA, oC);
        vector3_normalize(n1, tmp);
        if(vector3_dot(n1, self->dir) > 0)
            return 0;

        vector3_cross(n2, oC, oB);
        vector3_normalize(n2, tmp);
        if(vector3_dot(n2, self->dir) > 0)
            return 0;

        vector3_cross(n3, oB, oA);
        vector3_normalize(n3, tmp);
        if(vector3_dot(n3, self->dir) > 0)
            return 0;
    }

    return 1;
}

int moto_ray_intersect_cube(MotoRay *self,
        MotoIntersection *intersection,
        float origin[3], float width)
{
    float hw = width/2;
    float bound[] = {origin[0] - hw, origin[0] + hw,
                     origin[1] - hw, origin[1] + hw,
                     origin[2] - hw, origin[2] + hw};

    return moto_ray_intersect_bound(self, intersection, bound);
}

int moto_ray_intersect_cube_check(MotoRay *self,
        float origin[3], float width)
{
    float hw = width/2;
    float bound[] = {origin[0] - hw, origin[0] + hw,
                     origin[1] - hw, origin[1] + hw,
                     origin[2] - hw, origin[2] + hw};

    return moto_ray_intersect_bound_check(self, bound);
}

int moto_ray_intersect_cube_dist(MotoRay *self,
        float *dist,
        float origin[3],
        float width)
{
    float hw = width/2;
    float bound[] = {origin[0] - hw, origin[0] + hw,
                     origin[1] - hw, origin[1] + hw,
                     origin[2] - hw, origin[2] + hw};

    return moto_ray_intersect_bound_dist(self, dist, bound);
}

int moto_ray_intersect_sphere(MotoRay *self,
        MotoIntersection *intersection,
        float origin[3], float radius)
{
    return moto_ray_intersect_sphere_2(self, intersection, origin, radius*radius);
}

int moto_ray_intersect_sphere_check(MotoRay *self,
        float origin[3], float radius)
{
    return moto_ray_intersect_sphere_2_check(self, origin, radius*radius);
}

int moto_ray_intersect_sphere_dist(MotoRay *self,
        float *dist, float origin[3], float radius)
{
    return moto_ray_intersect_sphere_2_dist(self, dist, origin, radius*radius);
}

int moto_ray_intersect_sphere_2(MotoRay *self,
        MotoIntersection *intersection,
        float origin[3], float square_radius)
{
    float tmp, dst[3];
    vector3_dif(dst, self->pos, origin);

    float B = vector3_dot(dst, self->dir);
    float C = vector3_dot(dst, dst) - square_radius;
    float D = B*B - C;

    if(D > MICRO)
    {
        float sqrtD = sqrt(D);

        intersection->hits_num = 0;

        intersection->hits[0].dist = sqrtD - B;
        if(intersection->hits[0].dist > MICRO)
        {
            vector3_copy(intersection->hits[0].point, self->pos);
            point3_move(intersection->hits[0].point, self->dir, intersection->hits[0].dist);
            vector3_dif(intersection->hits[0].normal, intersection->hits[0].point, origin);
            vector3_normalize(intersection->hits[0].normal, tmp);

            intersection->hits[0].is_entering = \
                (vector3_dot(intersection->hits[0].normal, self->dir) < 0);

            intersection->hits_num++;
        }

        intersection->hits[intersection->hits_num].dist = -sqrtD - B;
        if(intersection->hits[intersection->hits_num].dist > MICRO)
        {
            vector3_copy(intersection->hits[intersection->hits_num].point, self->pos);
            point3_move(intersection->hits[intersection->hits_num].point, self->dir,
                    intersection->hits[intersection->hits_num].dist);
            vector3_dif(intersection->hits[intersection->hits_num].normal,
                    intersection->hits[intersection->hits_num].point, origin);
            vector3_normalize(intersection->hits[intersection->hits_num].normal, tmp);

            intersection->hits[intersection->hits_num].is_entering = \
                (vector3_dot(intersection->hits[intersection->hits_num].normal, self->dir) < 0);

            intersection->hits_num++;
        }

        return intersection->hits_num > 0;
    }
    else if(D >= 0 && D < MICRO)
    {
        intersection->hits_num = 0;
        intersection->hits[0].dist = -B;
        if(-B < MICRO)
        {
            vector3_copy(intersection->hits[0].point, self->pos);
            point3_move(intersection->hits[0].point, self->dir,
                    intersection->hits[0].dist);
            vector3_dif(intersection->hits[0].normal, intersection->hits[0].point, origin);
            vector3_normalize(intersection->hits[0].normal, tmp);

            intersection->hits[0].is_entering = 0;

            intersection->hits_num = 1;
        }

        return intersection->hits_num > 0;
    }

    return 0;
}

int moto_ray_intersect_sphere_2_check(MotoRay *self,
        float origin[3], float square_radius)
{
    float dst[3];
    vector3_dif(dst, self->pos, origin);

    float B = vector3_dot(dst, self->dir);
    float C = vector3_dot(dst, dst) - square_radius;
    float D = B*B - C;

    if(D > MICRO)
    {
        float sqrtD = sqrt(D);
        if((sqrtD - B) > MICRO)
            return 1;

        if((-sqrtD - B) > MICRO)
            return 1;
    }
    else if(D >= 0 && D < MICRO)
    {
        if(-B < MICRO)
            return 1;
    }

    return 0;
}

int moto_ray_intersect_sphere_2_dist(MotoRay *self,
        float *dist, float origin[3], float square_radius)
{
    float dst[3], dd[2];
    vector3_dif(dst, self->pos, origin);

    float B = vector3_dot(dst, self->dir);
    float C = vector3_dot(dst, dst) - square_radius;
    float D = B*B - C;

    if(D > MICRO)
    {
        float sqrtD = sqrt(D);
        int num = 0;

        dd[0] = sqrtD - B;
        if(dd[0] > MICRO)
        {
            *dist = dd[0];
            num++;
        }

        dd[1] = -sqrtD - B;
        if(dd[1] > MICRO)
        {
            *dist = dd[1];
            num++;
        }

        if( ! num)
            return 0;
        if(num == 2)
            *dist = min(dd[0], dd[1]);
        return 1;
    }
    else if(D >= 0 && D < MICRO)
    {
        *dist = -B;
        if(*dist < MICRO)
            return 1;
    }

    return 0;
}

int moto_ray_intersect_bound(MotoRay *self,
        MotoIntersection *intersection,
        float bound[6])
{
    float t_hit, numer, denom;

    float t_in  = -100000;
    float t_out = 100000;

    int in_surf = 0, out_surf = 1;

    int i;
    for(i = 0; i < 6; i++)
    {
        switch(i)
        {
            case 0:
                numer = max_y - self->pos[1];
                denom = self->dir[1];
            break;
            case 1:
                numer = -min_y + self->pos[1];
                denom = -self->dir[1];
            break;
            case 2:
                numer = max_x - self->pos[0];
                denom = self->dir[0];
            break;
            case 3:
                numer = -min_x + self->pos[0];
                denom = -self->dir[0];
            break;
            case 4:
                numer = max_z - self->pos[2];
                denom = self->dir[2];
            break;
            case 5:
                numer = -min_z + self->pos[2];
                denom = -self->dir[2];
            break;
        }

        if(fabs(denom) < MICRO) /* parallel */
        {
            if(numer < 0) return 0;
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
            return 0;
    }

    intersection->hits_num = 0;
    if(t_in > MICRO)
    {
        intersection->hits[0].dist = t_in;
        intersection->hits[0].is_entering = 1;

        vector3_copy(intersection->hits[0].point, self->pos);
        point3_move(intersection->hits[0].point, self->dir, t_in);
        cube_normal(intersection->hits[0].normal, in_surf);

        intersection->hits_num++;
    }
    if(t_out > MICRO)
    {
        intersection->hits[intersection->hits_num].dist = t_out;
        intersection->hits[intersection->hits_num].is_entering = 0;

        vector3_copy(intersection->hits[intersection->hits_num].point, self->pos);
        point3_move(intersection->hits[intersection->hits_num].point, self->dir, t_out);
        cube_normal(intersection->hits[intersection->hits_num].normal, out_surf);

        intersection->hits_num++;
    }

    return (intersection->hits_num > 0);
}

int moto_ray_intersect_bound_check(MotoRay *self, float bound[6])
{
    float t_hit, numer, denom;

    float t_in  = -100000;
    float t_out = 100000;

    int in_surf, out_surf;

    int i;
    for(i = 0; i < 6; i++)
    {
        switch(i)
        {
            case 0:
                numer = max_y - self->pos[1];
                denom = self->dir[1];
            break;
            case 1:
                numer = -min_y + self->pos[1];
                denom = -self->dir[1];
            break;
            case 2:
                numer = max_x - self->pos[0];
                denom = self->dir[0];
            break;
            case 3:
                numer = -min_x + self->pos[0];
                denom = -self->dir[0];
            break;
            case 4:
                numer = max_z - self->pos[2];
                denom = self->dir[2];
            break;
            case 5:
                numer = -min_z + self->pos[2];
                denom = -self->dir[2];
            break;
        }

        if(fabs(denom) < MICRO) /* parallel */
        {
            if(numer < 0) return 0;
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
            return 0;
    }

    if(t_in > MICRO)
        return 1;
    if(t_out > MICRO)
        return 1;

    return 0;
}

int moto_ray_intersect_bound_dist(MotoRay *self,
        float *dist, float bound[6])
{
    float t_hit, numer, denom;

    float t_in  = -100000;
    float t_out = 100000;

    int i;
    for(i = 0; i < 6; i++)
    {
        switch(i)
        {
            case 0:
                numer = max_y - self->pos[1];
                denom = self->dir[1];
            break;
            case 1:
                numer = -min_y + self->pos[1];
                denom = -self->dir[1];
            break;
            case 2:
                numer = max_x - self->pos[0];
                denom = self->dir[0];
            break;
            case 3:
                numer = -min_x + self->pos[0];
                denom = -self->dir[0];
            break;
            case 4:
                numer = max_z - self->pos[2];
                denom = self->dir[2];
            break;
            case 5:
                numer = -min_z + self->pos[2];
                denom = -self->dir[2];
            break;
        }

        if(fabs(denom) < MICRO) /* parallel */
        {
            if(numer < 0)
                return 0;
        }
        else
        {
            t_hit = numer / denom;
            if(denom > 0)
            {
                if(t_hit < t_out)
                {
                    t_out = t_hit;
                }
            }
            else
            {
                if(t_hit > t_in)
                {
                    t_in = t_hit;
                }
            }
        }

        if(t_in > t_out)
        {
            return 0;
        }
    }

    int num = 0;
    if(t_in > MICRO)
    {
        *dist = t_in;
        num++;
    }
    if(t_out > MICRO)
    {
        *dist = t_out;
        num++;
    }

    if( ! num)
        return 0;
    if(num == 2)
        *dist = t_in;

    return 1;
}

int moto_ray_intersect_cylinder(MotoRay *self,
        MotoIntersection *intersection,
        float a[3], float b[3], float radius)
{}

int moto_ray_intersect_cylinder_check(MotoRay *self,
        float a[3], float b[3], float radius)
{}

int moto_ray_intersect_cylinder_dist(MotoRay *self,
        float *dist, float a[3], float b[3], float radius)
{}

#include <stdio.h>
#include <assert.h>

#include "common/matrix.h"
#include "common/print-array.h"
#include "motocore/moto-ray.h"

void test_intersect_plane()
{
    MotoRay ray;
    MotoIntersection inter;
    float dist;

    vector3_set(ray.pos, 0, 0, -2);
    vector3_set(ray.dir, 0, 0, 1);

    float point[3] = {0, 0, 0};
    float normal[3] = {0, 0, 1};

    float p[3], n[3];

    float r = \
        moto_ray_intersect_plane_check(&ray, point, normal);
    assert(r);
    r = moto_ray_intersect_plane_dist(&ray, &dist, point, normal);
    assert(r);
    assert(dist == 2);
    r = moto_ray_intersect_plane(&ray, &inter, point, normal);
    assert(r);
    assert(inter.hits_num == 1);
    assert(inter.hits[0].dist = 2);
    vector3_set(p, 0, 0, 0);
    assert(vector3_equal_dif(inter.hits[0].point, p, MICRO));
    vector3_set(n, 0, 0, 1);
    assert(vector3_equal_dif(inter.hits[0].normal, n, MICRO));

    vector3_set(point, 1, 1, 1);
    vector3_set(normal, 0, 1, 0);

    r = moto_ray_intersect_plane_check(&ray, point, normal);
    assert( ! r);
    r = moto_ray_intersect_plane_dist(&ray, &dist, point, normal);
    assert( ! r);
    r = moto_ray_intersect_plane(&ray, &inter, point, normal);
    assert( ! r);

    vector3_set(point, 0, 0, -2);
    vector3_set(normal, 0, 0, 1);

    r = moto_ray_intersect_plane_check(&ray, point, normal);
    assert( ! r);
    r = moto_ray_intersect_plane_dist(&ray, &dist, point, normal);
    assert( ! r);
    r = moto_ray_intersect_plane(&ray, &inter, point, normal);
    assert( ! r);
}

void test_intersect_triangle()
{
    MotoRay ray;
    MotoIntersection inter;
    float dist, tmp, r;
    float p[3], n[3];

    /* 1 */

    vector3_set(ray.pos, 0, 0, 0);
    vector3_set(ray.dir, 1, 1, 1);
    vector3_normalize(ray.dir, tmp);

    float A[] = {1, 0, 0};
    float B[] = {0, 1, 0};
    float C[] = {0, 0, 1};

    r = moto_ray_intersect_triangle(&ray, &inter, A, B, C);
    assert(r);
    assert(fabs(inter.hits[0].dist - 0.577350) < MICRO);
    vector3_set(p, 0.333333, 0.333333, 0.333333);
    assert(vector3_equal_dif(p, inter.hits[0].point, MICRO));
    vector3_set(n, 0.577350, 0.577350, 0.577350);
    assert(vector3_equal_dif(n, inter.hits[0].normal, MICRO));

    r = moto_ray_intersect_triangle_check(&ray, A, B, C);
    assert(r);

    r = moto_ray_intersect_triangle_dist(&ray, &dist, A, B, C);
    assert(r);
    assert(fabs(dist - 0.577350) < MICRO);

    /* 2 */

    vector3_set(ray.pos, 2, 2, 2);
    vector3_set(ray.dir, -1, -1, -1);
    vector3_normalize(ray.dir, tmp);

    r = moto_ray_intersect_triangle(&ray, &inter, A, B, C);
    assert(r);
    vector3_set(p, 0.333333, 0.333333, 0.333333);
    assert(vector3_equal_dif(p, inter.hits[0].point, MICRO));
    vector3_set(n, 0.577350, 0.577350, 0.577350);
    assert(vector3_equal_dif(n, inter.hits[0].normal, MICRO));

    r = moto_ray_intersect_triangle_check(&ray, A, B, C);
    assert(r);

    r = moto_ray_intersect_triangle_dist(&ray, &dist, A, B, C);
    assert(r);

    /* 3 */

    vector3_set(ray.pos, 0, 0, 0);
    vector3_set(ray.dir, -0.1, 1, 1);
    vector3_normalize(ray.dir, tmp);

    r = moto_ray_intersect_triangle(&ray, &inter, A, B, C);
    assert( ! r);

    r = moto_ray_intersect_triangle_check(&ray, A, B, C);
    assert( ! r);

    r = moto_ray_intersect_triangle_dist(&ray, &dist, A, B, C);
    assert( ! r);
}

void test_intersect_sphere()
{
    MotoRay ray;
    MotoIntersection inter;
    float dist, tmp, r, radius, origin[3];
    float p[3];//, n[3];

    /* 1 */
    vector3_set(ray.pos, 0, 0, -2);
    vector3_set(ray.dir, 0, 0, 1);
    vector3_normalize(ray.dir, tmp);

    vector3_set(origin, 0, 0, 0);
    radius = 0.2;

    r = moto_ray_intersect_sphere(&ray, &inter, origin, radius);
    assert(r);
    assert(inter.hits_num == 2);
    if(inter.hits[0].is_entering)
    {
        vector3_set(p, 0, 0, -0.2);
        assert(fabs(inter.hits[0].dist - 1.8) < MICRO);
    }
    else
    {
        vector3_set(p, 0, 0, 0.2);
        assert(fabs(inter.hits[0].dist - 2.2) < MICRO);
    }
    assert(vector3_equal_dif(p, inter.hits[0].point, MICRO));
    if(inter.hits[1].is_entering)
    {
        vector3_set(p, 0, 0, -0.2);
    }
    else
    {
        vector3_set(p, 0, 0, 0.2);
    }
    assert(vector3_equal_dif(p, inter.hits[1].point, MICRO));

    r = moto_ray_intersect_sphere_check(&ray, origin, radius);
    assert(r);

    r = moto_ray_intersect_sphere_dist(&ray, &dist, origin, radius);
    assert(r);
    assert(fabs(dist - 1.8) < MICRO);

    /* 2 */
    vector3_set(ray.pos, 0, 0, -2);
    vector3_set(ray.dir, 1, 1, 1);
    vector3_normalize(ray.dir, tmp);

    vector3_set(origin, 0, 0, 0);
    radius = 0.1;

    r = moto_ray_intersect_sphere(&ray, &inter, origin, radius);
    assert( ! r);

    r = moto_ray_intersect_sphere_check(&ray, origin, radius);
    assert( ! r);

    r = moto_ray_intersect_sphere_dist(&ray, &dist, origin, radius);
    assert( ! r);
}

#define bound_set(b, min_x, max_x, min_y, max_y, min_z, max_z)\
    (b)[0] = (min_x);(b)[1] = (max_x);\
    (b)[2] = (min_y);(b)[3] = (max_y);\
    (b)[4] = (min_z);(b)[5] = (max_z)

void test_intersect_bound()
{
    MotoRay ray;
    MotoIntersection inter;
    float dist, tmp, r, bound[6];
    float p[3], n[3];

    /* 1 */

    vector3_set(ray.pos, 0, 0, -2.345);
    vector3_set(ray.dir, 0, 0, 1);
    vector3_normalize(ray.dir, tmp);

    bound_set(bound, -1, 1, -1, 1, -1, 1);

    r = moto_ray_intersect_bound(&ray, &inter, bound);
    assert(r);
    assert(inter.hits_num == 2);
    if(inter.hits[0].is_entering)
    {
        vector3_set(p, 0, 0, -1);
        vector3_set(n, 0, 0, -1);
        assert(fabs(inter.hits[0].dist - 1.345) < MICRO);
    }
    else
    {
         vector3_set(p, 0, 0, 1);
         vector3_set(n, 0, 0, 1);
        assert(fabs(inter.hits[0].dist - 3.345) < MICRO);
    }
    assert(vector3_equal_dif(p, inter.hits[0].point, MICRO));
    assert(vector3_equal_dif(n, inter.hits[0].normal, MICRO));
    if(inter.hits[1].is_entering)
    {
        vector3_set(p, 0, 0, -1);
        vector3_set(n, 0, 0, -1);
        assert(fabs(inter.hits[1].dist - 1.345) < MICRO);
    }
    else
    {
         vector3_set(p, 0, 0, 1);
         vector3_set(n, 0, 0, 1);
        assert(fabs(inter.hits[1].dist - 3.345) < MICRO);
    }
    assert(vector3_equal_dif(p, inter.hits[1].point, MICRO));
    assert(vector3_equal_dif(n, inter.hits[1].normal, MICRO));

    r = moto_ray_intersect_bound_check(&ray, bound);
    assert(r);

    r = moto_ray_intersect_bound_dist(&ray, &dist, bound);
    assert(r);
    assert(fabs(dist - 1.345) < MICRO);

    /* 2 */

    vector3_set(ray.pos, 0, 0, 2.345);
    vector3_set(ray.dir, 0, 0, 1);
    vector3_normalize(ray.dir, tmp);

    bound_set(bound, -1, 1, -1, 1, -1, 1);

    r = moto_ray_intersect_bound(&ray, &inter, bound);
    assert( ! r);

    r = moto_ray_intersect_bound_check(&ray, bound);
    assert( ! r);

    r = moto_ray_intersect_bound_dist(&ray, &dist, bound);
    assert( ! r);

    /* 3 */

    vector3_set(ray.pos, 0, 0, -2.345);
    vector3_set(ray.dir, 1, 1, 1);
    vector3_normalize(ray.dir, tmp);

    bound_set(bound, -0.1, 0.1, -0.1, 0.1, -0.1, 0.1);

    r = moto_ray_intersect_bound(&ray, &inter, bound);
    assert( ! r);

    r = moto_ray_intersect_bound_check(&ray, bound);
    assert( ! r);

    r = moto_ray_intersect_bound_dist(&ray, &dist, bound);
    assert( ! r);
}

void test_intersect_cube()
{
    MotoRay ray;
    MotoIntersection inter;
    float dist, tmp, r, origin[3];
    float p[3], n[3];

    /* 1 */

    vector3_set(ray.pos, 0, 0, -2.345);
    vector3_set(ray.dir, 0, 0, 1);
    vector3_normalize(ray.dir, tmp);

    vector3_set(origin, 0, 0, 0);

    r = moto_ray_intersect_cube(&ray, &inter, origin, 2);
    assert(r);
    assert(inter.hits_num == 2);
    if(inter.hits[0].is_entering)
    {
        vector3_set(p, 0, 0, -1);
        vector3_set(n, 0, 0, -1);
        assert(fabs(inter.hits[0].dist - 1.345) < MICRO);
    }
    else
    {
         vector3_set(p, 0, 0, 1);
         vector3_set(n, 0, 0, 1);
        assert(fabs(inter.hits[0].dist - 3.345) < MICRO);
    }
    assert(vector3_equal_dif(p, inter.hits[0].point, MICRO));
    assert(vector3_equal_dif(n, inter.hits[0].normal, MICRO));
    if(inter.hits[1].is_entering)
    {
        vector3_set(p, 0, 0, -1);
        vector3_set(n, 0, 0, -1);
        assert(fabs(inter.hits[1].dist - 1.345) < MICRO);
    }
    else
    {
         vector3_set(p, 0, 0, 1);
         vector3_set(n, 0, 0, 1);
        assert(fabs(inter.hits[1].dist - 3.345) < MICRO);
    }
    assert(vector3_equal_dif(p, inter.hits[1].point, MICRO));
    assert(vector3_equal_dif(n, inter.hits[1].normal, MICRO));

    r = moto_ray_intersect_cube_check(&ray, origin, 2);
    assert(r);

    r = moto_ray_intersect_cube_dist(&ray, &dist, origin, 2);
    assert(r);
    assert(fabs(dist - 1.345) < MICRO);

    /* 2 */

    vector3_set(ray.pos, 0, 0, 2.345);
    vector3_set(ray.dir, 0, 0, 1);
    vector3_normalize(ray.dir, tmp);

    vector3_set(origin, 0, 0, 0);

    r = moto_ray_intersect_cube(&ray, &inter, origin, 2);
    assert( ! r);

    r = moto_ray_intersect_cube_check(&ray, origin, 2);
    assert( ! r);

    r = moto_ray_intersect_cube_dist(&ray, &dist, origin, 2);
    assert( ! r);

    /* 3 */

    vector3_set(ray.pos, 0, 0, -2.345);
    vector3_set(ray.dir, 1, 1, 1);
    vector3_normalize(ray.dir, tmp);

    vector3_set(origin, 0, 0, 0);

    r = moto_ray_intersect_cube(&ray, &inter, origin, 0.2);
    assert( ! r);

    r = moto_ray_intersect_cube_check(&ray, origin, 0.2);
    assert( ! r);

    r = moto_ray_intersect_cube_dist(&ray, &dist, origin, 0.2);
    assert( ! r);
}

int main(int argc, char *argv[])
{
    printf("Testing \"moto-ray.h\" ... ");

    test_intersect_plane();
    test_intersect_triangle();
    test_intersect_sphere();
    test_intersect_bound();
    test_intersect_cube();

    printf("OK\n");

    return 0;
}

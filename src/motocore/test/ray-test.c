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

    /* 1 */
    vector3_set(ray.pos, 0, 0, -2);
    vector3_set(ray.dir, 0, 0, 1);
    // vector3_normalize(ray.dir, tmp);

    vector3_set(origin, 0, 0, 0);
    radius = 0.2;

    r = moto_ray_intersect_sphere(&ray, &inter, origin, radius);
    assert(r);

    /* 2 */
    vector3_set(ray.pos, 0, 0, -2);
    vector3_set(ray.dir, 1, 1, 1);
    vector3_normalize(ray.dir, tmp);

    vector3_set(origin, 0, 0, 0);
    radius = 0.1;

    r = moto_ray_intersect_sphere(&ray, &inter, origin, radius);
    assert( ! r);
}

int main(int argc, char *argv[])
{
    printf("Testing \"moto-ray.h\" ... ");

    test_intersect_plane();
    test_intersect_triangle();
    test_intersect_sphere();

    printf("OK\n");

    return 0;
}

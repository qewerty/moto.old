/* ##################################################################################
#
#  Moto Animation System (http://motoanim.sf.net)
#  Copyleft (C) 2008 Konstantin Evdokimenko a.k.a Qew[erty] (qewerty@gmail.com)
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
################################################################################## */

#ifndef _MOTO_RAY_H_
#define _MOTO_RAY_H_

#include <math.h>

#include "moto-intersection.h"

typedef struct _MotoRay MotoRay;

struct _MotoRay
{
    float pos[3];
    float dir[3];
};

void moto_ray_copy(MotoRay *self, MotoRay *other);
void moto_ray_transform(MotoRay *self, float m[16]);
void moto_ray_set_transformed(MotoRay *self, MotoRay *other, float m[16]);

/* intersections */

int moto_ray_intersect_plane(MotoRay *self,
        MotoIntersection *intersection,
        float point[3], float normal[3]);

int moto_ray_intersect_plane_check(MotoRay *self,
        float point[3], float normal[3]);

int moto_ray_intersect_plane_dist(MotoRay *self,
        float *dist,
        float point[3],
        float normal[3]);

int moto_ray_intersect_triangle(MotoRay *self,
        MotoIntersection *intersection,
        float A[3], float B[3], float C[3]);

int moto_ray_intersect_triangle_check(MotoRay *self,
        float A[3], float B[3], float C[3]);

int moto_ray_intersect_triangle_dist(MotoRay *self,
        float *dist, float A[3], float B[3], float C[3]);

int moto_ray_intersect_cube(MotoRay *self,
        MotoIntersection *intersection,
        float origin[3], float width);

int moto_ray_intersect_cube_check(MotoRay *self,
        float origin[3], float width);

int moto_ray_intersect_cube_dist(MotoRay *self,
        float *dist,
        float origin[3],
        float width);

int moto_ray_intersect_sphere(MotoRay *self,
        MotoIntersection *intersection,
        float origin[3], float radius);

int moto_ray_intersect_sphere_check(MotoRay *self,
        float origin[3], float radius);

int moto_ray_intersect_sphere_dist(MotoRay *self,
        float *dist,
        float origin[3],
        float radius);

int moto_ray_intersect_sphere_2(MotoRay *self,
        MotoIntersection *intersection,
        float origin[3], float square_radius);

int moto_ray_intersect_sphere_2_check(MotoRay *self,
        float origin[3], float square_radius);

int moto_ray_intersect_sphere_2_dist(MotoRay *self,
        float *dist,
        float origin[3],
        float square_radius);

/**
 * moto_ray_intersect_bound:
 * @self: a #MotoRay.
 * @intersection: a #MotoIntersection.
 * @bound: array of six values (min_x, max_x, min_y, max_y, min_z, max_z).
 *
 * Finds intersection with axis-aligned bounding box.
 *
 * Returns: 0 if no intersections found else returns not 0.
 */
int moto_ray_intersect_bound(MotoRay *self,
        MotoIntersection *intersection,
        float bound[6]);

#endif /* _MOTO_RAY_H_ */

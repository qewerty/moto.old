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

#ifndef _MOTO_INTERSECTION_H_
#define _MOTO_INTERSECTION_H_

#define MOTO_INTERSECTION_MAX_HITS 8

typedef struct _MotoIntersection MotoIntersection;
typedef struct _MotoRayHit MotoRayHit;

struct _MotoRayHit
{
    float dist;
    float point[3];
    float normal[3];
    char is_entering;
};

struct _MotoIntersection
{
     unsigned int hits_num;
     MotoRayHit hits[MOTO_INTERSECTION_MAX_HITS];
};

void moto_intersection_reset(MotoIntersection *self);
void moto_intersection_copy(MotoIntersection *self, MotoIntersection *other);

#endif /* _MOTO_INTERSECTION_H_ */

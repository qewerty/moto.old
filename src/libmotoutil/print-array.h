/*
*  Print some kind of arrays. Matricies, vectors, etc.
*  Macros for processing transformation matricies.
*  Copyleft (C) 2006 Konstantin Evdokimenko a.k.a Qew[erty] (qewerty@gmail.com)
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

#ifndef __MOTO_PRINT_ARRAY_H__
#define __MOTO_PRINT_ARRAY_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_matrix44(float m[16])
{
    int i, j;
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            printf("%f ", m[j*4 + i]);
        }
        printf("\n");
    }
}

void print_vector3(float v[3])
{
    printf("(%f %f %f)\n", v[0], v[1], v[2]);
}

#ifdef __cplusplus
}
#endif

#endif /* __MOTO_PRINT_ARRAY_H__ */

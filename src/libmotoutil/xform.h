/*
*
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

#ifndef __QEWERTY_XFORM_MACROS_H__
#define __QEWERTY_XFORM_MACROS_H__

#include <math.h>

#include "numdef.h"

/*

Column order like in OpenGL, vectors are columns.

*** math_row.math_column(C array index)

4x4

1.1(0)  1.2(4)  1.3(8)   1.4(12)
2.1(1)  2.2(5)  2.3(9)   2.4(13)
3.1(2)  3.2(6)  3.3(10)  3.4(14)
4.1(3)  4.2(7)  4.3(11)  4.4(15)

3x3

1.1(0)  1.2(3)  1.3(6)
2.1(1)  2.2(4)  2.3(7)
3.1(2)  3.2(5)  3.3(8)

Example of translation point (0, 1, 5) by (1, 2, 3):

1 0 0 1     0
0 1 0 2  *  1
0 0 1 3     5
0 0 0 1     1

This is not like in RenderMan where row-major order is used and
vectors are rows.

            1 0 0 0
0 1 5 1  *  0 1 0 0
            0 0 1 0
            1 2 3 1

But the cause is that in RenderMan matricies are assumed to be transposed
to OpenGL (and math) ones not row/column order difference.

Examples:

??? TODO

*/

/* matrix*_zero */

#define matrix44_zero(m)\
    (m)[0]  = 0; (m)[1]  = 0; (m)[2]  = 0; (m)[3]  = 0;\
    (m)[4]  = 0; (m)[5]  = 0; (m)[6]  = 0; (m)[7]  = 0;\
    (m)[8]  = 0; (m)[9]  = 0; (m)[10] = 0; (m)[11] = 0;\
    (m)[12] = 0; (m)[13] = 0; (m)[14] = 0; (m)[15] = 0

#define matrix33_zero(m)\
    (m)[0] = 0; (m)[1] = 0; (m)[2] = 0;\
    (m)[3] = 0; (m)[4] = 0; (m)[5] = 0;\
    (m)[6] = 0; (m)[7] = 0; (m)[8] = 0

/* matrix*_identity */

#define matrix44_identity(m)\
    (m)[0]  = 1; (m)[1]  = 0; (m)[2] = 0;  (m)[3]  = 0;\
    (m)[4]  = 0; (m)[5]  = 1; (m)[6] = 0;  (m)[7]  = 0;\
    (m)[8]  = 0; (m)[9]  = 0; (m)[10] = 1; (m)[11] = 0;\
    (m)[12] = 0; (m)[13] = 0; (m)[14] = 0; (m)[15] = 1

#define matrix33_identity(m)\
    (m)[0] = 1; (m)[1] = 0; (m)[2] = 0;\
    (m)[3] = 0; (m)[4] = 1; (m)[5] = 0;\
    (m)[6] = 0; (m)[7] = 0; (m)[8] = 1

/* matrix*_copy */

#define matrix44_copy(m, m2)\
    (m)[0]  = (m2)[0];  (m)[1]  = (m2)[1];  (m)[2]  = (m2)[2];  (m)[3]  = (m2)[3];\
    (m)[4]  = (m2)[4];  (m)[5]  = (m2)[5];  (m)[6]  = (m2)[6];  (m)[7]  = (m2)[7];\
    (m)[8]  = (m2)[8];  (m)[9]  = (m2)[9];  (m)[10] = (m2)[10]; (m)[11] = (m2)[11];\
    (m)[12] = (m2)[12]; (m)[13] = (m2)[13]; (m)[14] = (m2)[14]; (m)[15] = (m2)[15]

#define matrix33_copy(m, m2)\
    (m)[0] = (m2)[0]; (m)[1] = (m2)[1]; (m)[2] = (m2)[2];\
    (m)[3] = (m2)[3]; (m)[4] = (m2)[4]; (m)[5] = (m2)[5];\
    (m)[6] = (m2)[6]; (m)[7] = (m2)[7]; (m)[8] = (m2)[8]

/* matrix*_equal* */

#define matrix44_equal(m, m2)\
    ((m)[0]  == (m2)[0] &&\
     (m)[1]  == (m2)[1] &&\
     (m)[2]  == (m2)[2] &&\
     (m)[3]  == (m2)[3] &&\
     (m)[4]  == (m2)[4] &&\
     (m)[5]  == (m2)[5] &&\
     (m)[6]  == (m2)[6] &&\
     (m)[7]  == (m2)[7] &&\
     (m)[8]  == (m2)[8] &&\
     (m)[9]  == (m2)[9] &&\
     (m)[10] == (m2)[10] &&\
     (m)[11] == (m2)[11] &&\
     (m)[12] == (m2)[12] &&\
     (m)[13] == (m2)[13] &&\
     (m)[14] == (m2)[14] &&\
     (m)[15] == (m2)[15])

#define matrix44_equal_dif(m, m2, dif)\
    (fabs((m)[0]  - (m2)[0])  < (dif) &&\
     fabs((m)[1]  - (m2)[1])  < (dif) &&\
     fabs((m)[2]  - (m2)[2])  < (dif) &&\
     fabs((m)[3]  - (m2)[3])  < (dif) &&\
     fabs((m)[4]  - (m2)[4])  < (dif) &&\
     fabs((m)[5]  - (m2)[5])  < (dif) &&\
     fabs((m)[6]  - (m2)[6])  < (dif) &&\
     fabs((m)[7]  - (m2)[7])  < (dif) &&\
     fabs((m)[8]  - (m2)[8])  < (dif) &&\
     fabs((m)[9]  - (m2)[9])  < (dif) &&\
     fabs((m)[10] - (m2)[10]) < (dif) &&\
     fabs((m)[11] - (m2)[11]) < (dif) &&\
     fabs((m)[12] - (m2)[12]) < (dif) &&\
     fabs((m)[13] - (m2)[13]) < (dif) &&\
     fabs((m)[14] - (m2)[14]) < (dif) &&\
     fabs((m)[15] - (m2)[15]) < (dif))

#define matrix33_equal(m, m2)\
    ((m)[0] == (m2)[0] &&\
     (m)[1] == (m2)[1] &&\
     (m)[2] == (m2)[2] &&\
     (m)[3] == (m2)[3] &&\
     (m)[4] == (m2)[4] &&\
     (m)[5] == (m2)[5] &&\
     (m)[6] == (m2)[6] &&\
     (m)[7] == (m2)[7] &&\
     (m)[8] == (m2)[8])

#define matrix33_equal_dif(m, m2, dif)\
    (fabs((m)[0] - (m2)[0]) < (dif) &&\
     fabs((m)[1] - (m2)[1]) < (dif) &&\
     fabs((m)[2] - (m2)[2]) < (dif) &&\
     fabs((m)[3] - (m2)[3]) < (dif) &&\
     fabs((m)[4] - (m2)[4]) < (dif) &&\
     fabs((m)[5] - (m2)[5]) < (dif) &&\
     fabs((m)[6] - (m2)[6]) < (dif) &&\
     fabs((m)[7] - (m2)[7]) < (dif) &&\
     fabs((m)[8] - (m2)[8]) < (dif))

/* matrix*_sum */

#define matrix44_sum(r, m, m2)\
    (r)[0]  = (m)[0]  + (m2)[0];\
    (r)[1]  = (m)[1]  + (m2)[1];\
    (r)[2]  = (m)[2]  + (m2)[2];\
    (r)[3]  = (m)[3]  + (m2)[3];\
    (r)[4]  = (m)[4]  + (m2)[4];\
    (r)[5]  = (m)[5]  + (m2)[5];\
    (r)[6]  = (m)[6]  + (m2)[6];\
    (r)[7]  = (m)[7]  + (m2)[7];\
    (r)[8]  = (m)[8]  + (m2)[8];\
    (r)[9]  = (m)[9]  + (m2)[9];\
    (r)[10] = (m)[10] + (m2)[10];\
    (r)[11] = (m)[11] + (m2)[11];\
    (r)[12] = (m)[12] + (m2)[12];\
    (r)[13] = (m)[13] + (m2)[13];\
    (r)[14] = (m)[14] + (m2)[14];\
    (r)[15] = (m)[15] + (m2)[15]

#define matrix33_sum(r, m, m2)\
    (r)[0] = (m)[0] + (m2)[0];\
    (r)[1] = (m)[1] + (m2)[1];\
    (r)[2] = (m)[2] + (m2)[2];\
    (r)[3] = (m)[3] + (m2)[3];\
    (r)[4] = (m)[4] + (m2)[4];\
    (r)[5] = (m)[5] + (m2)[5];\
    (r)[6] = (m)[6] + (m2)[6];\
    (r)[7] = (m)[7] + (m2)[7];\
    (r)[8] = (m)[8] + (m2)[8]

/* matrix*_add */

#define matrix44_add(m, m2) matrix44_sum(m, m, m2)
#define matrix33_add(m, m2) matrix33_sum(m, m, m2)

/* matrix*_dif */

#define matrix44_dif(r, m, m2)\
    (r)[0]  = (m)[0]  - (m2)[0];\
    (r)[1]  = (m)[1]  - (m2)[1];\
    (r)[2]  = (m)[2]  - (m2)[2];\
    (r)[3]  = (m)[3]  - (m2)[3];\
    (r)[4]  = (m)[4]  - (m2)[4];\
    (r)[5]  = (m)[5]  - (m2)[5];\
    (r)[6]  = (m)[6]  - (m2)[6];\
    (r)[7]  = (m)[7]  - (m2)[7];\
    (r)[8]  = (m)[8]  - (m2)[8];\
    (r)[9]  = (m)[9]  - (m2)[9];\
    (r)[10] = (m)[10] - (m2)[10];\
    (r)[11] = (m)[11] - (m2)[11];\
    (r)[12] = (m)[12] - (m2)[12];\
    (r)[13] = (m)[13] - (m2)[13];\
    (r)[14] = (m)[14] - (m2)[14];\
    (r)[15] = (m)[15] - (m2)[15]

#define matrix33_dif(r, m, m2)\
    (r)[0] = (m)[0] - (m2)[0];\
    (r)[1] = (m)[1] - (m2)[1];\
    (r)[2] = (m)[2] - (m2)[2];\
    (r)[3] = (m)[3] - (m2)[3];\
    (r)[4] = (m)[4] - (m2)[4];\
    (r)[5] = (m)[5] - (m2)[5];\
    (r)[6] = (m)[6] - (m2)[6];\
    (r)[7] = (m)[7] - (m2)[7];\
    (r)[8] = (m)[8] - (m2)[8]

/* matrix*_sub */

#define matrix44_sub(m, m2) matrix44_dif(m, m, m2)
#define matrix33_sub(m, m2) matrix33_dif(m, m, m2)

/* matrix*_translate* */

#define matrix44_translate(m, x, y, z)\
    (m)[0]  = 1;    (m)[1]  = 0;   (m)[2]  = 0;   (m)[3]  = 0;\
    (m)[4]  = 0;    (m)[5]  = 1;   (m)[6]  = 0;   (m)[7]  = 0;\
    (m)[8]  = 0;    (m)[9]  = 0;   (m)[10] = 1;   (m)[11] = 0;\
    (m)[12] = (x);  (m)[13] = (y); (m)[14] = (z); (m)[15] = 1

#define matrix44_translate_lazy(m, x, y, z)\
    (m)[12] = (x);  (m)[13] = (y); (m)[14] = (z)

#define matrix33_translate(m, x, y)\
    (m)[0] = 1;   (m)[1] = 0;   (m)[2] = 0;\
    (m)[3] = 0;   (m)[4] = 1;   (m)[5] = 0;\
    (m)[6] = (x); (m)[7] = (y); (m)[8] = 1

#define matrix33_translate_lazy(m, x, y)\
    (m)[6] = (x);  (m)[7] = (y)

/* matrix*_scale* */

#define matrix44_scale(m, x, y, z)\
    (m)[0]  = (x);  (m)[1]  = 0;   (m)[2]  = 0;   (m)[3]  = 0;\
    (m)[4]  = 0;    (m)[5]  = (y); (m)[6]  = 0;   (m)[7]  = 0;\
    (m)[8]  = 0;    (m)[9]  = 0;   (m)[10] = (z); (m)[11] = 0;\
    (m)[12] = 0;    (m)[13] = 0;   (m)[14] = 0;   (m)[15] = 1

#define matrix44_scale_lazy(m, x, y, z)\
    (m)[0] = (x);  (m)[5] = (y); (m)[10] = (z)

#define matrix33_scale(m, x, y)\
    (m)[0] = (x); (m)[1] = 0;   (m)[2] = 0;\
    (m)[3] = 0;   (m)[4] = (y); (m)[5] = 0;\
    (m)[6] = 0;   (m)[7] = 0;   (m)[8] = 1

#define matrix33_scale_lazy(m, x, y, z)\
    (m)[0] = (x);  (m)[4] = (y);

/* matrix*_rotate* */

#define matrix44_rotate_x(m, a)\
    (m)[0]  = 1;  (m)[1]  = 0;       (m)[2]  = 0;      (m)[3]  = 0;\
    (m)[4]  = 0;  (m)[5]  = cos(a);  (m)[6]  = sin(a); (m)[7]  = 0;\
    (m)[8]  = 0;  (m)[9]  = -(m)[6]; (m)[10] = (m)[5]; (m)[11] = 0;\
    (m)[12] = 0;  (m)[13] = 0;       (m)[14] = 0;      (m)[15] = 1

#define matrix44_rotate_y(m, a)\
    (m)[0]  = cos(a);  (m)[1]  = 0; (m)[2]  = -sin(a); (m)[3]  = 0;\
    (m)[4]  = 0;       (m)[5]  = 1; (m)[6]  = 0;       (m)[7]  = 0;\
    (m)[8]  = -(m)[2]; (m)[9]  = 0; (m)[10] = (m)[0];  (m)[11] = 0;\
    (m)[12] = 0;       (m)[13] = 0; (m)[14] = 0;       (m)[15] = 1

#define matrix44_rotate_z(m, a)\
    (m)[0]  = cos(a);  (m)[1]  = sin(a); (m)[2]  = 0; (m)[3]  = 0;\
    (m)[4]  = -(m)[1]; (m)[5]  = (m)[0]; (m)[6]  = 0; (m)[7]  = 0;\
    (m)[8]  = 0;       (m)[9]  = 0;      (m)[10] = 1; (m)[11] = 0;\
    (m)[12] = 0;       (m)[13] = 0;      (m)[14] = 0; (m)[15] = 1

#define matrix33_rotate(m, a)\
    (m)[0] = cos(a);  (m)[1] = sin(a); (m)[2] = 0;\
    (m)[3] = -(m)[1]; (m)[4] = (m)[0]; (m)[5] = 0;\
    (m)[6] = 0;       (m)[7] = 0;      (m)[8] = 1

#define matrix44_rotate_from_axis(m, a, x, y, z)\
    (m)[0]  = (1-cos(a))*(x)*(x) + cos(a);\
    (m)[1]  = (1-cos(a))*(x)*(y) + sin(a)*(z);\
    (m)[2]  = ((1-cos(a))*(x)*(z) - sin(a)*(y)); \
    (m)[3]  = 0;\
    (m)[4]  = (1-cos(a))*(x)*(y) - sin(a)*(z);\
    (m)[5]  = (1-cos(a))*(y)*(y) + cos(a);\
    (m)[6]  = (1-cos(a))*(y)*(z) + sin(a)*(x);\
    (m)[7]  = 0;\
    (m)[8]  = (1-cos(a))*(x)*(z) + sin(a)*(y);\
    (m)[9]  = (1-cos(a))*(y)*(z) - sin(a)*(x);\
    (m)[10] = (1-cos(a))*(z)*(z) + cos(a);\
    (m)[11] = 0;\
    (m)[12] = 0;\
    (m)[13] = 0;\
    (m)[14] = 0;\
    (m)[15] = 1

#define matrix44_rotate_from_axis_sincos(m, s, c, x, y, z)\
    (m)[0]  = (1-(c))*(x)*(x) + (c);\
    (m)[1]  = (1-(c))*(x)*(y) + (s)*(z);\
    (m)[2]  = ((1-(c))*(x)*(z) - (s)*(y)); \
    (m)[3]  = 0;\
    (m)[4]  = (1-(c))*(x)*(y) - (s)*(z);\
    (m)[5]  = (1-(c))*(y)*(y) + (c);\
    (m)[6]  = (1-(c))*(y)*(z) + (s)*(x);\
    (m)[7]  = 0;\
    (m)[8]  = (1-(c))*(x)*(z) + (s)*(y);\
    (m)[9]  = (1-(c))*(y)*(z) - (s)*(x);\
    (m)[10] = (1-(c))*(z)*(z) + (c);\
    (m)[11] = 0;\
    (m)[12] = 0;\
    (m)[13] = 0;\
    (m)[14] = 0;\
    (m)[15] = 1

#define matrix44_rotate_from_quaternion(m, q)\
    (m)[0]  = 1 - 2*(q)[1]*(q)[1] - 2*(q)[2]*(q)[2];\
    (m)[1]  = 2*(q)[0]*(q)[1] + 2*(q)[3]*(q)[2];\
    (m)[2]  = 2*(q)[0]*(q)[2] - 2*(q)[3]*(q)[1];\
    (m)[3]  = 0;\
    (m)[4]  = 2*(q)[0]*(q)[1] - 2*(q)[3]*(q)[2];\
    (m)[5]  = 1 - 2*(q)[0]*(q)[0] - 2*(q)[2]*(q)[2];\
    (m)[6]  = 2*(q)[1]*(q)[2] + 2*(q)[3]*(q)[0];\
    (m)[7]  = 0;\
    (m)[8]  = 2*(q)[0]*(q)[2] + 2*(q)[3]*(q)[1];\
    (m)[9]  = 2*(q)[1]*(q)[2] - 2*(q)[3]*(q)[0];\
    (m)[10] = 1 - 2*(q)[0]*(q)[0] - 2*(q)[1]*(q)[1];\
    (m)[11] = 0;\
    (m)[12] = 0;\
    (m)[13] = 0;\
    (m)[14] = 0;\
    (m)[15] = 1

/* matrix44_perspective */

#define matrix44_perspective(m, fovy, aspect, z_near, z_far)\
    (m)[1]  = 0;\
    (m)[2]  = 0;\
    (m)[3]  = 0;\
    (m)[4]  = 0;\
    (m)[5]  = (1/tan(fovy/2));\
    (m)[0]  = (m)[5]/(aspect);\
    (m)[6]  = 0;\
    (m)[7]  = 0;\
    (m)[8]  = 0;\
    (m)[9]  = 0;\
    (m)[10] = (z_far+z_near)/(z_near-z_far);\
    (m)[11] = -1;\
    (m)[12] = 0;\
    (m)[13] = 0;\
    (m)[14] = (2*z_far*z_near)/(z_near-z_far);\
    (m)[15] = 0

/* matrix*_transpose */

#define matrix44_transpose(r, m)\
    (r)[0]  = (m)[0];\
    (r)[1]  = (m)[4];\
    (r)[2]  = (m)[8];\
    (r)[3]  = (m)[12];\
    (r)[4]  = (m)[1];\
    (r)[5]  = (m)[5];\
    (r)[6]  = (m)[9];\
    (r)[7]  = (m)[13];\
    (r)[8]  = (m)[2];\
    (r)[9]  = (m)[6];\
    (r)[10] = (m)[10];\
    (r)[11] = (m)[14];\
    (r)[12] = (m)[3];\
    (r)[13] = (m)[7];\
    (r)[14] = (m)[11];\
    (r)[15] = (m)[15]

#define matrix33_transpose(r, m)\
    (r)[0]  = (m)[0];\
    (r)[1]  = (m)[3];\
    (r)[2]  = (m)[6];\
    (r)[3]  = (m)[1];\
    (r)[4]  = (m)[4];\
    (r)[5]  = (m)[7];\
    (r)[6]  = (m)[2];\
    (r)[7]  = (m)[5];\
    (r)[8]  = (m)[8]

/* matrix*_mult | m*m2 */

#define matrix44_mult(r, m, m2)\
    (r)[0]  = (m)[0]*(m2)[0]  + (m)[4]*(m2)[1]  + (m)[8]*(m2)[2]   + (m)[12]*(m2)[3];\
    (r)[1]  = (m)[1]*(m2)[0]  + (m)[5]*(m2)[1]  + (m)[9]*(m2)[2]   + (m)[13]*(m2)[3];\
    (r)[2]  = (m)[2]*(m2)[0]  + (m)[6]*(m2)[1]  + (m)[10]*(m2)[2]  + (m)[14]*(m2)[3];\
    (r)[3]  = (m)[3]*(m2)[0]  + (m)[7]*(m2)[1]  + (m)[11]*(m2)[2]  + (m)[15]*(m2)[3];\
    (r)[4]  = (m)[0]*(m2)[4]  + (m)[4]*(m2)[5]  + (m)[8]*(m2)[6]   + (m)[12]*(m2)[7];\
    (r)[5]  = (m)[1]*(m2)[4]  + (m)[5]*(m2)[5]  + (m)[9]*(m2)[6]   + (m)[13]*(m2)[7];\
    (r)[6]  = (m)[2]*(m2)[4]  + (m)[6]*(m2)[5]  + (m)[10]*(m2)[6]  + (m)[14]*(m2)[7];\
    (r)[7]  = (m)[3]*(m2)[4]  + (m)[7]*(m2)[5]  + (m)[11]*(m2)[6]  + (m)[15]*(m2)[7];\
    (r)[8]  = (m)[0]*(m2)[8]  + (m)[4]*(m2)[9]  + (m)[8]*(m2)[10]  + (m)[12]*(m2)[11];\
    (r)[9]  = (m)[1]*(m2)[8]  + (m)[5]*(m2)[9]  + (m)[9]*(m2)[10]  + (m)[13]*(m2)[11];\
    (r)[10] = (m)[2]*(m2)[8]  + (m)[6]*(m2)[9]  + (m)[10]*(m2)[10] + (m)[14]*(m2)[11];\
    (r)[11] = (m)[3]*(m2)[8]  + (m)[7]*(m2)[9]  + (m)[11]*(m2)[10] + (m)[15]*(m2)[11];\
    (r)[12] = (m)[0]*(m2)[12] + (m)[4]*(m2)[13] + (m)[8]*(m2)[14]  + (m)[12]*(m2)[15];\
    (r)[13] = (m)[1]*(m2)[12] + (m)[5]*(m2)[13] + (m)[9]*(m2)[14]  + (m)[13]*(m2)[15];\
    (r)[14] = (m)[2]*(m2)[12] + (m)[6]*(m2)[13] + (m)[10]*(m2)[14] + (m)[14]*(m2)[15];\
    (r)[15] = (m)[3]*(m2)[12] + (m)[7]*(m2)[13] + (m)[11]*(m2)[14] + (m)[15]*(m2)[15]

#define matrix33_mult(r, m, m2)\
    (r)[0] = (m)[0]*(m2)[0] + (m)[3]*(m2)[1] + (m)[6]*(m2)[2];\
    (r)[1] = (m)[1]*(m2)[0] + (m)[4]*(m2)[1] + (m)[7]*(m2)[2];\
    (r)[2] = (m)[2]*(m2)[0] + (m)[5]*(m2)[1] + (m)[8]*(m2)[2];\
    (r)[3] = (m)[0]*(m2)[3] + (m)[3]*(m2)[4] + (m)[6]*(m2)[5];\
    (r)[4] = (m)[1]*(m2)[3] + (m)[4]*(m2)[4] + (m)[7]*(m2)[5];\
    (r)[5] = (m)[2]*(m2)[3] + (m)[5]*(m2)[4] + (m)[8]*(m2)[5];\
    (r)[6] = (m)[0]*(m2)[6] + (m)[3]*(m2)[7] + (m)[6]*(m2)[8];\
    (r)[7] = (m)[1]*(m2)[6] + (m)[4]*(m2)[7] + (m)[7]*(m2)[8];\
    (r)[8] = (m)[2]*(m2)[6] + (m)[5]*(m2)[7] + (m)[8]*(m2)[8]

/* matrix*_det */

#define matrix44_det(m)\
    ((m)[0]*((m)[5]*((m)[10]*(m)[15] - (m)[14]*(m)[11]) - (m)[9]*((m)[6]*(m)[15] -\
        (m)[14]*(m)[7]) + (m)[13]*((m)[6]*(m)[11] - (m)[10]*(m)[7])) -\
     (m)[4]*-((m)[1]*((m)[10]*(m)[15] - (m)[14]*(m)[11]) - (m)[9]*((m)[2]*(m)[15] -\
        (m)[14]*(m)[3]) + (m)[13]*((m)[2]*(m)[11] - (m)[10]*(m)[3])) +\
     (m)[8]*((m)[1]*((m)[6]*(m)[15] - (m)[14]*(m)[7]) - (m)[5]*((m)[2]*(m)[15] -\
        (m)[14]*(m)[3]) + (m)[13]*((m)[2]*(m)[7] - (m)[6]*(m)[3])) -\
     (m)[12]*-((m)[1]*((m)[6]*(m)[11] - (m)[10]*(m)[7]) - (m)[5]*((m)[2]*(m)[11] -\
        (m)[10]*(m)[3]) + (m)[9]*((m)[2]*(m)[7] - (m)[6]*(m)[3])))

#define matrix33_det(m)\
    ((m)[0]*((m)[4]*(m)[8] - (m)[7]*(m)[5]) -\
     (m)[3]*((m)[1]*(m)[8] - (m)[7]*(m)[2]) +\
     (m)[6]*((m)[1]*(m)[5] - (m)[4]*(m)[2]))

/* matrix*_inverse */

#define matrix44_inverse(r, m, ambuf, detbuf)\
    (ambuf)[0]   =  ((m)[5]*((m)[10]*(m)[15] - (m)[14]*(m)[11]) - (m)[9]*((m)[6]*(m)[15] - (m)[14]*(m)[7]) + (m)[13]*((m)[6]*(m)[11] - (m)[10]*(m)[7]));\
    (ambuf)[1]   = -((m)[4]*((m)[10]*(m)[15] - (m)[14]*(m)[11]) - (m)[8]*((m)[6]*(m)[15] - (m)[14]*(m)[7]) + (m)[12]*((m)[6]*(m)[11] - (m)[10]*(m)[7]));\
    (ambuf)[2]   =  ((m)[4]*((m)[9]*(m)[15]  - (m)[13]*(m)[11]) - (m)[8]*((m)[5]*(m)[15] - (m)[13]*(m)[7]) + (m)[12]*((m)[5]*(m)[11] - (m)[9]*(m)[7]));\
    (ambuf)[3]   = -((m)[4]*((m)[9]*(m)[14]  - (m)[13]*(m)[10]) - (m)[8]*((m)[5]*(m)[14] - (m)[13]*(m)[6]) + (m)[12]*((m)[5]*(m)[10] - (m)[9]*(m)[6]));\
    (ambuf)[4]   = -((m)[1]*((m)[10]*(m)[15] - (m)[14]*(m)[11]) - (m)[9]*((m)[2]*(m)[15] - (m)[14]*(m)[3]) + (m)[13]*((m)[2]*(m)[11] - (m)[10]*(m)[3]));\
    (ambuf)[5]   =  ((m)[0]*((m)[10]*(m)[15] - (m)[14]*(m)[11]) - (m)[8]*((m)[2]*(m)[15] - (m)[14]*(m)[3]) + (m)[12]*((m)[2]*(m)[11] - (m)[10]*(m)[3]));\
    (ambuf)[6]   = -((m)[0]*((m)[9]*(m)[15]  - (m)[13]*(m)[11]) - (m)[8]*((m)[1]*(m)[15] - (m)[13]*(m)[3]) + (m)[12]*((m)[1]*(m)[11] - (m)[9]*(m)[3]));\
    (ambuf)[7]   =  ((m)[0]*((m)[9]*(m)[14]  - (m)[13]*(m)[10]) - (m)[8]*((m)[1]*(m)[14] - (m)[13]*(m)[2]) + (m)[12]*((m)[1]*(m)[10] - (m)[9]*(m)[2]));\
    (ambuf)[8]   =  ((m)[1]*((m)[6]*(m)[15]  - (m)[14]*(m)[7])  - (m)[5]*((m)[2]*(m)[15] - (m)[14]*(m)[3]) + (m)[13]*((m)[2]*(m)[7]  - (m)[6]*(m)[3]));\
    (ambuf)[9]   = -((m)[0]*((m)[6]*(m)[15]  - (m)[14]*(m)[7])  - (m)[4]*((m)[2]*(m)[15] - (m)[14]*(m)[3]) + (m)[12]*((m)[2]*(m)[7]  - (m)[6]*(m)[3]));\
    (ambuf)[10]  =  ((m)[0]*((m)[5]*(m)[15]  - (m)[13]*(m)[7])  - (m)[4]*((m)[1]*(m)[15] - (m)[13]*(m)[3]) + (m)[12]*((m)[1]*(m)[7]  - (m)[5]*(m)[3]));\
    (ambuf)[11]  = -((m)[0]*((m)[5]*(m)[14]  - (m)[13]*(m)[6])  - (m)[4]*((m)[1]*(m)[14] - (m)[13]*(m)[2]) + (m)[12]*((m)[1]*(m)[6]  - (m)[5]*(m)[2]));\
    (ambuf)[12]  = -((m)[1]*((m)[6]*(m)[11]  - (m)[10]*(m)[7])  - (m)[5]*((m)[2]*(m)[11] - (m)[10]*(m)[3]) + (m)[9]*((m)[2]*(m)[7]   - (m)[6]*(m)[3]));\
    (ambuf)[13]  =  ((m)[0]*((m)[6]*(m)[11]  - (m)[10]*(m)[7])  - (m)[4]*((m)[2]*(m)[11] - (m)[10]*(m)[3]) + (m)[8]*((m)[2]*(m)[7]   - (m)[6]*(m)[3]));\
    (ambuf)[14]  = -((m)[0]*((m)[5]*(m)[11]  - (m)[9]*(m)[7])   - (m)[4]*((m)[1]*(m)[11] - (m)[9]*(m)[3])  + (m)[8]*((m)[1]*(m)[7]   - (m)[5]*(m)[3]));\
    (ambuf)[15]  =  ((m)[0]*((m)[5]*(m)[10]  - (m)[9]*(m)[6])   - (m)[4]*((m)[1]*(m)[10] - (m)[9]*(m)[2])  + (m)[8]*((m)[1]*(m)[6]   - (m)[5]*(m)[2]));\
    (detbuf) = (ambuf)[0]*(m)[0]+(ambuf)[4]*(m)[4]+(ambuf)[8]*(m)[8]+(ambuf)[12]*(m)[12];\
    if(detbuf)\
    {\
        (r)[0]  = (ambuf)[0]  / (detbuf);\
        (r)[1]  = (ambuf)[4]  / (detbuf);\
        (r)[2]  = (ambuf)[8]  / (detbuf);\
        (r)[3]  = (ambuf)[12] / (detbuf);\
        (r)[4]  = (ambuf)[1]  / (detbuf);\
        (r)[5]  = (ambuf)[5]  / (detbuf);\
        (r)[6]  = (ambuf)[9]  / (detbuf);\
        (r)[7]  = (ambuf)[13] / (detbuf);\
        (r)[8]  = (ambuf)[2]  / (detbuf);\
        (r)[9]  = (ambuf)[6]  / (detbuf);\
        (r)[10] = (ambuf)[10] / (detbuf);\
        (r)[11] = (ambuf)[14] / (detbuf);\
        (r)[12] = (ambuf)[3]  / (detbuf);\
        (r)[13] = (ambuf)[7]  / (detbuf);\
        (r)[14] = (ambuf)[11] / (detbuf);\
        (r)[15] = (ambuf)[15] / (detbuf);\
    }

#define matrix44_inverse2(r, m, ambuf, detbuf, cache) /* cache requires array of 17 elements | Ex: float cache[17]; */\
    (ambuf)[0]   =  ((m)[5]*(cache[0] = ((m)[10]*(m)[15] - (m)[14]*(m)[11])) - (m)[9]*(cache[1] = ((m)[6]*(m)[15] - (m)[14]*(m)[7])) + (m)[13]*(cache[2] = ((m)[6]*(m)[11] - (m)[10]*(m)[7])));\
    (ambuf)[1]   = -((m)[4]*(cache[0]) - (m)[8]*(cache[1]) + (m)[12]*(cache[2]));\
    (ambuf)[2]   =  ((m)[4]*(cache[3] = ((m)[9]*(m)[15] - (m)[13]*(m)[11])) - (m)[8]*(cache[4] = ((m)[5]*(m)[15] - (m)[13]*(m)[7])) + (m)[12]*(cache[5] = ((m)[5]*(m)[11] - (m)[9]*(m)[7])));\
    (ambuf)[3]   = -((m)[4]*(cache[6] = ((m)[9]*(m)[14] - (m)[13]*(m)[10])) - (m)[8]*(cache[7] = ((m)[5]*(m)[14] - (m)[13]*(m)[6])) + (m)[12]*(cache[8] = ((m)[5]*(m)[10] - (m)[9]*(m)[6])));\
    (ambuf)[4]   = -((m)[1]*(cache[0]) - (m)[9]*(cache[9] = ((m)[2]*(m)[15] - (m)[14]*(m)[3])) + (m)[13]*(cache[10] = ((m)[2]*(m)[11] - (m)[10]*(m)[3])));\
    (ambuf)[5]   =  ((m)[0]*(cache[0]) - (m)[8]*(cache[9]) + (m)[12]*(cache[10]));\
    (ambuf)[6]   = -((m)[0]*(cache[3]) - (m)[8]*(cache[11] = ((m)[1]*(m)[15] - (m)[13]*(m)[3])) + (m)[12]*(cache[12] = ((m)[1]*(m)[11] - (m)[9]*(m)[3])));\
    (ambuf)[7]   =  ((m)[0]*(cache[6]) - (m)[8]*(cache[13] = ((m)[1]*(m)[14] - (m)[13]*(m)[2])) + (m)[12]*(cache[14] = ((m)[1]*(m)[10] - (m)[9]*(m)[2])));\
    (ambuf)[8]   =  ((m)[1]*(cache[1]) - (m)[5]*(cache[9]) + (m)[13]*(cache[15] = ((m)[2]*(m)[7] - (m)[6]*(m)[3])));\
    (ambuf)[9]   = -((m)[0]*(cache[1]) - (m)[4]*(cache[9]) + (m)[12]*(cache[15]));\
    (ambuf)[10]  =  ((m)[0]*(cache[4]) - (m)[4]*(cache[11]) + (m)[12]*((m)[1]*(m)[7] - (m)[5]*(m)[3]));\
    (ambuf)[11]  = -((m)[0]*(cache[7]) - (m)[4]*(cache[13]) + (m)[12]*((m)[1]*(m)[6] - (m)[5]*(m)[2]));\
    (ambuf)[12]  = -((m)[1]*(cache[2]) - (m)[5]*(cache[10]) + (m)[9]*(cache[16] = ((m)[2]*(m)[7]  - (m)[6]*(m)[3])));\
    (ambuf)[13]  =  ((m)[0]*(cache[2]) - (m)[4]*(cache[10]) + (m)[8]*(cache[16]));\
    (ambuf)[14]  = -((m)[0]*(cache[5]) - (m)[4]*(cache[12])  + (m)[8]*((m)[1]*(m)[7]  - (m)[5]*(m)[3]));\
    (ambuf)[15]  =  ((m)[0]*(cache[8]) - (m)[4]*(cache[14])  + (m)[8]*((m)[1]*(m)[6]  - (m)[5]*(m)[2]));\
    (detbuf) = (ambuf)[0]*(m)[0]+(ambuf)[4]*(m)[4]+(ambuf)[8]*(m)[8]+(ambuf)[12]*(m)[12];\
    if(detbuf)\
    {\
        (r)[0]  = (ambuf)[0]  / (detbuf);\
        (r)[1]  = (ambuf)[4]  / (detbuf);\
        (r)[2]  = (ambuf)[8]  / (detbuf);\
        (r)[3]  = (ambuf)[12] / (detbuf);\
        (r)[4]  = (ambuf)[1]  / (detbuf);\
        (r)[5]  = (ambuf)[5]  / (detbuf);\
        (r)[6]  = (ambuf)[9]  / (detbuf);\
        (r)[7]  = (ambuf)[13] / (detbuf);\
        (r)[8]  = (ambuf)[2]  / (detbuf);\
        (r)[9]  = (ambuf)[6]  / (detbuf);\
        (r)[10] = (ambuf)[10] / (detbuf);\
        (r)[11] = (ambuf)[14] / (detbuf);\
        (r)[12] = (ambuf)[3]  / (detbuf);\
        (r)[13] = (ambuf)[7]  / (detbuf);\
        (r)[14] = (ambuf)[11] / (detbuf);\
        (r)[15] = (ambuf)[15] / (detbuf);\
    }

#define matrix33_inverse(r, m, ambuf, detbuf)\
    (ambuf)[0] =  ((m)[4]*(m)[8]-(m)[7]*(m)[5]);\
    (ambuf)[1] = -((m)[3]*(m)[8]-(m)[6]*(m)[5]);\
    (ambuf)[2] =  ((m)[3]*(m)[7]-(m)[6]*(m)[4]);\
    (ambuf)[3] = -((m)[1]*(m)[8]-(m)[7]*(m)[2]);\
    (ambuf)[4] =  ((m)[0]*(m)[8]-(m)[6]*(m)[2]);\
    (ambuf)[5] = -((m)[0]*(m)[7]-(m)[6]*(m)[1]);\
    (ambuf)[6] =  ((m)[1]*(m)[5]-(m)[4]*(m)[2]);\
    (ambuf)[7] = -((m)[0]*(m)[5]-(m)[3]*(m)[2]);\
    (ambuf)[8] =  ((m)[0]*(m)[4]-(m)[3]*(m)[1]);\
    (detbuf) = (ambuf)[0]*(m)[0]+(ambuf)[3]*(m)[3]+(ambuf)[6]*(m)[6];\
    if(detbuf)\
    {\
        (r)[0] = (ambuf)[0] / (detbuf);\
        (r)[1] = (ambuf)[3] / (detbuf);\
        (r)[2] = (ambuf)[6] / (detbuf);\
        (r)[3] = (ambuf)[1] / (detbuf);\
        (r)[4] = (ambuf)[4] / (detbuf);\
        (r)[5] = (ambuf)[7] / (detbuf);\
        (r)[6] = (ambuf)[2] / (detbuf);\
        (r)[7] = (ambuf)[5] / (detbuf);\
        (r)[8] = (ambuf)[8] / (detbuf);\
    }

/* vector*_zero */

#define vector3_zero(v) (v)[0] = 0; (v)[1] = 0; (v)[2] = 0
#define vector2_zero(v) (v)[0] = 0; (v)[1] = 0

/* vector*_copy */

#define vector3_copy(v, v2)\
    (v)[0] = (v2)[0]; (v)[1] = (v2)[1]; (v)[2] = (v2)[2]

#define vector2_copy(v, v2)\
    (v)[0] = (v2)[0]; (v)[1] = (v2)[1]

/* vector*_set */

#define vector3_set(v, x, y, z)\
    (v)[0] = (x); (v)[1] = (y); (v)[2] = (z)

#define vector2_set(v, x, y)\
    (v)[0] = (x); (v)[1] = (y)

/* vector*_equal */

#define vector4_equal(v, v2)\
    ((v)[0] == (v2)[0] &&\
     (v)[1] == (v2)[1] &&\
     (v)[2] == (v2)[2] &&\
     (v)[3] == (v2)[3])

#define vector4_equal_dif(v, v2, dif)\
    (fabs((v)[0] - (v2)[0]) < (dif) &&\
     fabs((v)[1] - (v2)[1]) < (dif) &&\
     fabs((v)[2] - (v2)[2]) < (dif) &&\
     fabs((v)[3] - (v2)[3]) < (dif))

#define vector3_equal(v, v2)\
    ((v)[0] == (v2)[0] &&\
     (v)[1] == (v2)[1] &&\
     (v)[2] == (v2)[2])

#define vector3_equal_dif(v, v2, dif)\
    (fabs((v)[0] - (v2)[0]) < (dif) &&\
     fabs((v)[1] - (v2)[1]) < (dif) &&\
     fabs((v)[2] - (v2)[2]) < (dif))

#define vector2_equal(v, v2)\
    ((v)[0] == (v2)[0] &&\
     (v)[1] == (v2)[1])

#define vector2_equal_dif(v, v2, dif)\
    (fabs((v)[0] - (v2)[0]) < (dif) &&\
     fabs((v)[1] - (v2)[1]) < (dif))

/* vector*_sum */

#define vector4_sum(r, v, v2)\
    (r)[0] = (v)[0] + (v2)[0];\
    (r)[1] = (v)[1] + (v2)[1];\
    (r)[2] = (v)[2] + (v2)[2];\
    (r)[3] = (v)[3] + (v2)[3]

// You must before include <xmmintrin.h>
#define vector4_sum_INTRIN(r, v, v2)\
    _mm_store_ps(r, _mm_add_ps(_mm_load_ps(v), _mm_load_ps(v2)));

#define vector4_mul_INTRIN(r, v, v2)\
    _mm_store_ps(r, _mm_mul_ps(_mm_load_ps(v), _mm_load_ps(v2)));

#define vector3_sum(r, v, v2)\
    (r)[0] = (v)[0] + (v2)[0];\
    (r)[1] = (v)[1] + (v2)[1];\
    (r)[2] = (v)[2] + (v2)[2]

#define vector2_sum(r, v, v2)\
    (r)[0] = (v)[0] + (v2)[0];\
    (r)[1] = (v)[1] + (v2)[1]

/* vector*_add */

#define vector4_add(v, v2) vector4_sum(v, v, v2)
#define vector3_add(v, v2) vector3_sum(v, v, v2)
#define vector2_add(v, v2) vector2_sum(v, v, v2)

/* vector*_dif */

#define vector4_dif(r, v, v2)\
    (r)[0] = (v)[0] - (v2)[0];\
    (r)[1] = (v)[1] - (v2)[1];\
    (r)[2] = (v)[2] - (v2)[2];\
    (r)[3] = (v)[3] - (v2)[3]

#define vector3_dif(r, v, v2)\
    (r)[0] = (v)[0] - (v2)[0];\
    (r)[1] = (v)[1] - (v2)[1];\
    (r)[2] = (v)[2] - (v2)[2]

#define vector2_dif(r, v, v2)\
    (r)[0] = (v)[0] - (v2)[0];\
    (r)[1] = (v)[1] - (v2)[1]

/* vector*_sub */

#define vector4_sub(v, v2) vector4_dif(v, v, v2)
#define vector3_sub(v, v2) vector3_dif(v, v, v2)
#define vector2_sub(v, v2) vector2_dif(v, v, v2)

/* vector*_mult */

#define vector3_mult(r, v, s)\
    (r)[0] = (v)[0]*(s); (r)[1] = (v)[1]*(s); (r)[2] = (v)[2]*(s)

#define vector2_mult(r, v, s)\
    (r)[0] = (v)[0]*(s); (r)[1] = (v)[1]*(s)

/* vector*_dot */

#define vector4_dot(v, v2)\
    ((v)[0]*(v2)[0]+(v)[1]*(v2)[1]+(v)[2]*(v2)[2]+(v)[3]*(v2)[3])

#define vector3_dot(v, v2)\
    ((v)[0]*(v2)[0]+(v)[1]*(v2)[1]+(v)[2]*(v2)[2])

#define vector2_dot(v, v2)\
    ((v)[0]*(v2)[0]+(v)[1]*(v2)[1])

/* vector3_cross */

#define vector3_cross(c, a, b)\
    (c)[0] = (a)[1]*(b)[2] - (a)[2]*(b)[1];\
    (c)[1] = (a)[2]*(b)[0] - (a)[0]*(b)[2];\
    (c)[2] = (a)[0]*(b)[1] - (a)[1]*(b)[0]

/* vector*_length */

#define vector4_length(v) sqrt(vector4_dot((v), (v)))
#define vector3_length(v) sqrt(vector3_dot((v), (v)))
#define vector2_length(v) sqrt(vector2_dot((v), (v)))

/* vector*_normalize */

#define vector4_normalize(v, lenbuf)\
    (lenbuf) = vector4_length(v);\
    (v)[0] /= (lenbuf);\
    (v)[1] /= (lenbuf);\
    (v)[2] /= (lenbuf);\
    (v)[3] /= (lenbuf)

#define vector3_normalize(v, lenbuf)\
    (lenbuf) = vector3_length(v);\
    (v)[0] /= (lenbuf);\
    (v)[1] /= (lenbuf);\
    (v)[2] /= (lenbuf)

#define vector2_normalize(v, lenbuf)\
    (lenbuf) = vector2_length(v);\
    (v)[0] /= (lenbuf);\
    (v)[1] /= (lenbuf)

/* [hvector|hpoint]*compute */

#define hvector4_compute(hv)\
    if((hv)[3]){\
    (hv)[0] /= (hv)[3];\
    (hv)[1] /= (hv)[3];\
    (hv)[2] /= (hv)[3];\
    (hv)[3] = 0;}

#define hvector3_compute(hv)\
    if((hv)[2]){\
    (hv)[0] /= (hv)[2];\
    (hv)[1] /= (hv)[2];\
    (hv)[2] = 0;}

#define hpoint4_compute(hp)\
    if((hp)[3] != 1){\
    (hp)[0] /= (hp)[3];\
    (hp)[1] /= (hp)[3];\
    (hp)[2] /= (hp)[3];\
    (hp)[3] = 1;}

#define hpoint3_compute(hp)\
    if((hp)[2] != 1){\
    (hp)[0] /= (hp)[2];\
    (hp)[1] /= (hp)[2];\
    (hp)[2] = 1;}

/* *[vector|point|normal]*_transform | m*v */

#define vector3_transform(r, m, v)\
    (r)[0] = (m)[0]*(v)[0] + (m)[4]*(v)[1] + (m)[8]*(v)[2];\
    (r)[1] = (m)[1]*(v)[0] + (m)[5]*(v)[1] + (m)[9]*(v)[2];\
    (r)[2] = (m)[2]*(v)[0] + (m)[6]*(v)[1] + (m)[10]*(v)[2]

#define vector2_transform(r, m, v)\
    (r)[0] = (m)[0]*(v)[0] + (m)[3]*(v)[1];\
    (r)[1] = (m)[1]*(v)[0] + (m)[4]*(v)[1]

#define point3_transform(r, m, v)\
    (r)[0] = (m)[0]*(v)[0] + (m)[4]*(v)[1] + (m)[8]*(v)[2]  + (m)[12];\
    (r)[1] = (m)[1]*(v)[0] + (m)[5]*(v)[1] + (m)[9]*(v)[2]  + (m)[13];\
    (r)[2] = (m)[2]*(v)[0] + (m)[6]*(v)[1] + (m)[10]*(v)[2] + (m)[14]

#define point2_transform(r, m, v)\
    (r)[0] = (m)[0]*(v)[0] + (m)[3]*(v)[1] + (m)[6];\
    (r)[1] = (m)[1]*(v)[0] + (m)[4]*(v)[1] + (m)[7]

#define normal3_transform(r, m, v, transbuf, revbuf)\
    matrix44_inverse(revbuf, m, transbuf/* ambuf */, (r)[0]/* detbuf */);\
    matrix44_transpose(transbuf, revbuf);\
    (r)[0] = (transbuf)[0]*(v)[0] + (transbuf)[4]*(v)[1] + (transbuf)[8]*(v)[2];\
    (r)[1] = (transbuf)[1]*(v)[0] + (transbuf)[5]*(v)[1] + (transbuf)[9]*(v)[2];\
    (r)[2] = (transbuf)[2]*(v)[0] + (transbuf)[6]*(v)[1] + (transbuf)[10]*(v)[2]

/* quat_* */

#define quat_identity(q)\
    (q)[0] = 0;\
    (q)[1] = 0;\
    (q)[2] = 0;\
    (q)[3] = 1

#define quat_sum(r, q, q2) vector4_sum(r, q, q2)
#define quat_dif(r, q, q2) vector4_dif(r, q, q2)
#define quat_add(q, q2) vector4_add(q, q2)
#define quat_sub(q, q2) vector4_sub(q, q2)

#define quat_equal(q, q2) vector4_equal(q, q2)
#define quat_equal_dif(q, q2, dif) vector4_equal_dif(q, q2, dif)

#define quat_normalize(q, lenbuf) vector4_normalize(q, lenbuf)

#define quat_mult(r, q, q2, crossbuf)\
    vector3_cross(crossbuf, q, q2);\
    (r)[0] = (q)[3]*(q2)[0] + (q2)[3]*(q)[0] + (crossbuf)[0];\
    (r)[1] = (q)[3]*(q2)[1] + (q2)[3]*(q)[1] + (crossbuf)[1];\
    (r)[2] = (q)[3]*(q2)[2] + (q2)[3]*(q)[2] + (crossbuf)[2];\
    (r)[3] = (q)[3]*(q2)[3] - vector3_dot(q, q2)

#define quat_inverse(r, q, dotbuf)\
    (dotbuf) = vector4_dot(q, q);\
    (r)[0] = -(q)[0] / (dotbuf);\
    (r)[1] = -(q)[1] / (dotbuf);\
    (r)[2] = -(q)[2] / (dotbuf);\
    (r)[3] =  (q)[3] / (dotbuf)

/* quat_from* */

#define quat_from_axis(q, a, x, y, z)\
    (q)[0] = sin(a/2)*x;\
    (q)[1] = sin(a/2)*y;\
    (q)[2] = sin(a/2)*z;\
    (q)[3] = cos(a/2)

#define quat_form_matrix(q, m)\
    (q)[3] = sqrt((m)[0]+(m)[5]+(n)[10]+1)/2;\
    (q)[0] = sqrt((m[0]+1-2*(q)[3]*(q)[3])/2);\
    (q)[1] = sqrt((m[5]+1-2*(q)[3]*(q)[3])/2);\
    (q)[2] = sqrt((m[10]+1-2*(q)[3]*(q)[3])/2)

/*  quat_from_euler* */

#define quat_from_euler_xyz(q, x, y, z, buf, buf2, crossbuf)\
    (buf)[0] = sin(x/2);\
    (buf)[1] = 0;\
    (buf)[2] = 0;\
    (buf)[3] = cos(x/2);\
    (q)[0] = 0;\
    (q)[1] = sin(y/2);\
    (q)[2] = 0;\
    (q)[3] = cos(y/2);\
    quat_mult(buf2, buf, q, crossbuf);\
    (buf)[0] = 0;\
    (buf)[1] = 0;\
    (buf)[2] = sin(z/2);\
    (buf)[3] = cos(z/2);\
    quat_mult(q, buf2, buf, crossbuf)

#define quat_from_euler_xzy(q, x, y, z, buf, buf2, crossbuf)\
    (buf)[0] = sin(x/2);\
    (buf)[1] = 0;\
    (buf)[2] = 0;\
    (buf)[3] = cos(x/2);\
    (q)[0] = 0;\
    (q)[1] = 0;\
    (q)[2] = sin(z/2);\
    (q)[3] = cos(z/2);\
    quat_mult(buf2, buf, q, crossbuf);\
    (buf)[0] = 0;\
    (buf)[1] = sin(y/2);\
    (buf)[2] = 0;\
    (buf)[3] = cos(y/2);\
    quat_mult(q, buf2, buf, crossbuf)

#define quat_from_euler_yxz(q, x, y, z, buf, buf2, crossbuf)\
    (buf)[0] = 0;\
    (buf)[1] = sin(y/2);\
    (buf)[2] = 0;\
    (buf)[3] = cos(y/2);\
    (q)[0] = sin(x/2);\
    (q)[1] = 0;\
    (q)[2] = 0;\
    (q)[3] = cos(x/2);\
    quat_mult(buf2, buf, q, crossbuf);\
    (buf)[0] = 0;\
    (buf)[1] = 0;\
    (buf)[2] = sin(z/2);\
    (buf)[3] = cos(z/2);\
    quat_mult(q, buf2, buf, crossbuf)

#define quat_from_euler_yzx(q, x, y, z, buf, buf2, crossbuf)\
    (buf)[0] = 0;\
    (buf)[1] = sin(y/2);\
    (buf)[2] = 0;\
    (buf)[3] = cos(y/2);\
    (q)[0] = 0;\
    (q)[1] = 0;\
    (q)[2] = sin(z/2);\
    (q)[3] = cos(z/2);\
    quat_mult(buf2, buf, q, crossbuf);\
    (buf)[0] = sin(x/2);\
    (buf)[1] = 0;\
    (buf)[2] = 0;\
    (buf)[3] = cos(x/2);\
    quat_mult(q, buf2, buf, crossbuf)

#define quat_from_euler_zxy(q, x, y, z, buf, buf2, crossbuf)\
    (buf)[0] = 0;\
    (buf)[1] = 0;\
    (buf)[2] = sin(z/2);\
    (buf)[3] = cos(z/2);\
    (q)[0] = sin(x/2);\
    (q)[1] = 0;\
    (q)[2] = 0;\
    (q)[3] = cos(x/2);\
    quat_mult(buf2, buf, q, crossbuf);\
    (buf)[0] = 0;\
    (buf)[1] = sin(y/2);\
    (buf)[2] = 0;\
    (buf)[3] = cos(y/2);\
    quat_mult(q, buf2, buf, crossbuf)

#define quat_from_euler_zyx(q, x, y, z, buf, buf2, crossbuf)\
    (buf)[0] = 0;\
    (buf)[1] = 0;\
    (buf)[2] = sin(z/2);\
    (buf)[3] = cos(z/2);\
    (q)[0] = 0;\
    (q)[1] = sin(y/2);\
    (q)[2] = 0;\
    (q)[3] = cos(y/2);\
    quat_mult(buf2, buf, q, crossbuf);\
    (buf)[0] = sin(x/2);\
    (buf)[1] = 0;\
    (buf)[2] = 0;\
    (buf)[3] = cos(x/2);\
    quat_mult(q, buf2, buf, crossbuf)

/* hvector4_transform_by_quat */

#define hvector4_transform_by_quat(r, v, q, tmp, invbuf, crossbuf, dotbuf)\
    quat_inverse(invbuf, q, dotbuf);\
    quat_mult(tmp, q, v, crossbuf);\
    quat_mult(r, tmp, invbuf, crossbuf)

/* *_from_matrix* */

#define translate_from_matrix44(r, m)\
    (r)[0] = (m)[12];\
    (r)[1] = (m)[13];\
    (r)[2] = (m)[14]

/* Argument 'r' must be an array of 3 values and angle (in radians) will be written into 'r[0]'
 * Matrix 'm' must not contain scale. */
#define rotate_x_from_matrix44(r, m, tmpvec)\
    vector3_set((r), 0.0, 1.0, 0.0);\
    vector3_transform((tmpvec), (m), (r));\
    (r)[0] = acos((tmpvec)[2])

#define rotate_y_from_matrix44(r, m, tmpvec)\
    vector3_set((r), 0.0, 0.0, 1.0);\
    vector3_transform((tmpvec), (m), (r));\
    (r)[0] = acos((tmpvec)[0])

#define rotate_z_from_matrix44(r, m, tmpvec)\
    vector3_set((r), 1.0, 0.0, 0.0);\
    vector3_transform((tmpvec), (m), (r));\
    (r)[0] = acos((tmpvec)[1])

#define euler_xyz_from_matrix44(r, m, cosbuf)\
    (r)[1] = asin((m)[8]);\
    (cosbuf) = cos((r)[1]);\
    if(fabs(cosbuf) > MICRO)\
    {\
        (r)[0] = atan2(-(m)[9] / (cosbuf), (m)[10] / (cosbuf));\
        (r)[2] = atan2(-(m)[4] / (cosbuf), (m)[0] / (cosbuf));\
    }\
    else /* gimbal lock */ \
    {\
        (r)[0] = 0;\
        (r)[2] = atan2((m)[1], (m)[5]);\
    }

#define euler_xzy_from_matrix44(r, m, cosbuf)\
    (r)[2] = asin(-(m)[4]);\
    (cosbuf) = cos((r)[2]);\
    if(fabs(cosbuf) > MICRO)\
    {\
        (r)[0] = atan2((m)[6] / (cosbuf), (m)[5] / (cosbuf));\
        (r)[1] = atan2((m)[8] / (cosbuf), (m)[0] / (cosbuf));\
    }\
    else /* gimbal lock */ \
    {\
        (r)[0] = 0;\
        (r)[1] = atan2(-(m)[2], (m)[10]);\
    }

#define euler_yxz_from_matrix44(r, m, cosbuf)\
    (r)[0] = asin(-(m)[9]);\
    (cosbuf) = cos((r)[0]);\
    if(fabs(cosbuf) > MICRO)\
    {\
        (r)[1] = atan2((m)[8] / (cosbuf), (m)[10] / (cosbuf));\
        (r)[2] = atan2((m)[1] / (cosbuf), (m)[5] / (cosbuf));\
    }\
    else /* gimbal lock */ \
    {\
        (r)[1] = 0;\
        (r)[2] = atan2(-(m)[4], (m)[0]);\
    }

#define euler_yzx_from_matrix44(r, m, cosbuf)\
    (r)[2] = asin((m)[1]);\
    (cosbuf) = cos((r)[2]);\
    if(fabs(cosbuf) > MICRO)\
    {\
        (r)[0] = atan2(-(m)[9] / (cosbuf), (m)[5] / (cosbuf));\
        (r)[1] = atan2(-(m)[2] / (cosbuf), (m)[0] / (cosbuf));\
    }\
    else /* gimbal lock */ \
    {\
        (r)[0] = atan2((m)[6], (m)[10]);\
        (r)[1] = 0;\
    }

#define euler_zxy_from_matrix44(r, m, cosbuf)\
    (r)[0] = asin((m)[6]);\
    (cosbuf) = cos((r)[0]);\
    if(fabs(cosbuf) > MICRO)\
    {\
        (r)[1] = atan2(-(m)[2] / (cosbuf), (m)[10] / (cosbuf));\
        (r)[2] = atan2(-(m)[4] / (cosbuf), (m)[5] / (cosbuf));\
    }\
    else /* gimbal lock */ \
    {\
        (r)[1] = 0;\
        (r)[2] = atan2((m)[1], (m)[0]);\
    }

#define euler_zyx_from_matrix44(r, m, cosbuf)\
    (r)[1] = asin(-(m)[2]);\
    (cosbuf) = cos((r)[1]);\
    if(fabs(cosbuf) > MICRO)\
    {\
        (r)[0] = atan2((m)[6] / (cosbuf), (m)[10] / (cosbuf));\
        (r)[2] = atan2((m)[1] / (cosbuf), (m)[0] / (cosbuf));\
    }\
    else /* gimbal lock */ \
    {\
        (r)[0] = 0;\
        (r)[2] = atan2(-(m)[2], (m)[10]);\
    }

/* misc */

#define point3_move(p, v, val)\
    (p)[0] += (v)[0]*val;\
    (p)[1] += (v)[1]*val;\
    (p)[2] += (v)[2]*val

#define matrix44_camera_inverse(m, eye, u, v, n)\
    (m)[0] = (u)[0]; (m)[4] = (u)[1]; (m)[8]  = (u)[2]; (m)[12] = -vector3_dot(eye, u);\
    (m)[1] = (v)[0]; (m)[5] = (v)[1]; (m)[9]  = (v)[2]; (m)[13] = -vector3_dot(eye, v);\
    (m)[2] = (n)[0]; (m)[6] = (n)[1]; (m)[10] = (n)[2]; (m)[14] = -vector3_dot(eye, n);\
    (m)[3] = 0;      (m)[7] = 0;      (m)[11] = 0;      (m)[15] = 1

#endif /* __QEWERTY_XFORM_MACROS_H__ */


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

#ifndef __MOTO_TYPES__
#define __MOTO_TYPES__

#include <glib-object.h>

#define MOTO_TYPE_BOOLEAN   G_TYPE_BOOLEAN
#define MOTO_TYPE_BOOLEAN_1 G_TYPE_BOOLEAN
#define MOTO_TYPE_INT       G_TYPE_INT
#define MOTO_TYPE_INT_1     G_TYPE_INT
#define MOTO_TYPE_UINT      G_TYPE_UINT
#define MOTO_TYPE_UINT_1    G_TYPE_UINT
#define MOTO_TYPE_LONG      G_TYPE_LONG
#define MOTO_TYPE_LONG_1    G_TYPE_LONG
#define MOTO_TYPE_ULONG     G_TYPE_ULONG
#define MOTO_TYPE_ULONG_1   G_TYPE_ULONG
#define MOTO_TYPE_INT64     G_TYPE_INT64
#define MOTO_TYPE_INT64_1   G_TYPE_INT64
#define MOTO_TYPE_UINT64    G_TYPE_UINT64
#define MOTO_TYPE_UINT64_1  G_TYPE_UINT64
#define MOTO_TYPE_FLOAT     G_TYPE_FLOAT
#define MOTO_TYPE_FLOAT_1   G_TYPE_FLOAT
#define MOTO_TYPE_DOUBLE    G_TYPE_DOUBLE
#define MOTO_TYPE_DOUBLE_1  G_TYPE_DOUBLE

#define MOTO_TYPE_STRING   G_TYPE_STRING
#define MOTO_TYPE_POINTER  G_TYPE_POINTER

GType moto_float_2_get_type(void);
#define MOTO_TYPE_FLOAT_2 (moto_float_2_get_type())

GType moto_float_3_get_type(void);
#define MOTO_TYPE_FLOAT_3 (moto_float_3_get_type())

GType moto_float_4_get_type(void);
#define MOTO_TYPE_FLOAT_4 (moto_float_4_get_type())

/*

// Vector types

MOTO_TYPE_BOOLEAN_2
MOTO_TYPE_BOOLEAN_3
MOTO_TYPE_BOOLEAN_4

MOTO_TYPE_INT_2
MOTO_TYPE_INT_3
MOTO_TYPE_INT_4

MOTO_TYPE_UINT_2
MOTO_TYPE_UINT_3
MOTO_TYPE_UINT_4

MOTO_TYPE_LONG_2
MOTO_TYPE_LONG_3
MOTO_TYPE_LONG_4

MOTO_TYPE_ULONG_2
MOTO_TYPE_ULONG_3
MOTO_TYPE_ULONG_4

MOTO_TYPE_INT64_2
MOTO_TYPE_INT64_3
MOTO_TYPE_INT64_4

MOTO_TYPE_UINT64_2
MOTO_TYPE_UINT64_3
MOTO_TYPE_UINT64_4

MOTO_TYPE_FLOAT_2
MOTO_TYPE_FLOAT_3
MOTO_TYPE_FLOAT_4

MOTO_TYPE_DOUBLE_2
MOTO_TYPE_DOUBLE_3
MOTO_TYPE_DOUBLE_4

// Matrix types

MOTO_TYPE_BOOLEAN_2x2
MOTO_TYPE_BOOLEAN_3x3
MOTO_TYPE_BOOLEAN_4x4

MOTO_TYPE_INT_2x2
MOTO_TYPE_INT_3x3
MOTO_TYPE_INT_4x4

MOTO_TYPE_UINT_2x2
MOTO_TYPE_UINT_3x3
MOTO_TYPE_UINT_4x4

MOTO_TYPE_LONG_2x2
MOTO_TYPE_LONG_3x3
MOTO_TYPE_LONG_4x4

MOTO_TYPE_ULONG_2x2
MOTO_TYPE_ULONG_3x3
MOTO_TYPE_ULONG_4x4

MOTO_TYPE_INT64_2x2
MOTO_TYPE_INT64_3x3
MOTO_TYPE_INT64_4x4

MOTO_TYPE_UINT64_2x2
MOTO_TYPE_UINT64_3x3
MOTO_TYPE_UINT64_4x4

MOTO_TYPE_FLOAT_2x2
MOTO_TYPE_FLOAT_3x3
MOTO_TYPE_FLOAT_4x4

MOTO_TYPE_DOUBLE_2x2
MOTO_TYPE_DOUBLE_3x3
MOTO_TYPE_DOUBLE_4x4

// Misc types

MOTO_TYPE_FILENAME (Derived from G_TYPE_STRING)
MOTO_TYPE_IMAGE_1D
MOTO_TYPE_IMAGE_2D
MOTO_TYPE_IMAGE_3D

*/

void moto_types_init(void);

#endif /* __MOTO_TYPES__ */

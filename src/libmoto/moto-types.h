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

#ifndef __MOTO_TYPES_H__
#define __MOTO_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define MOTO_TYPE_BOOLEAN     G_TYPE_BOOLEAN
#define MOTO_TYPE_BOOLEAN_1   G_TYPE_BOOLEAN
#define MOTO_TYPE_BOOLEAN_1x1 G_TYPE_BOOLEAN
#define MOTO_TYPE_INT         G_TYPE_INT
#define MOTO_TYPE_INT_1       G_TYPE_INT
#define MOTO_TYPE_INT_1x1     G_TYPE_INT
#define MOTO_TYPE_FLOAT       G_TYPE_FLOAT
#define MOTO_TYPE_FLOAT_1     G_TYPE_FLOAT
#define MOTO_TYPE_FLOAT_1x1   G_TYPE_FLOAT

#define MOTO_TYPE_STRING   G_TYPE_STRING
#define MOTO_TYPE_POINTER  G_TYPE_POINTER

// Vector types

GType moto_boolean_2_get_type(void);
#define MOTO_TYPE_BOOLEAN_2 (moto_boolean_2_get_type())

GType moto_boolean_3_get_type(void);
#define MOTO_TYPE_BOOLEAN_3 (moto_boolean_3_get_type())

GType moto_boolean_4_get_type(void);
#define MOTO_TYPE_BOOLEAN_4 (moto_boolean_4_get_type())

GType moto_int_2_get_type(void);
#define MOTO_TYPE_INT_2 (moto_int_2_get_type())

GType moto_int_3_get_type(void);
#define MOTO_TYPE_INT_3 (moto_int_3_get_type())

GType moto_int_4_get_type(void);
#define MOTO_TYPE_INT_4 (moto_int_4_get_type())

GType moto_float_2_get_type(void);
#define MOTO_TYPE_FLOAT_2 (moto_float_2_get_type())

GType moto_float_3_get_type(void);
#define MOTO_TYPE_FLOAT_3 (moto_float_3_get_type())

GType moto_float_4_get_type(void);
#define MOTO_TYPE_FLOAT_4 (moto_float_4_get_type())

// Matrix tyepes

GType moto_boolean_2x2_get_type(void);
#define MOTO_TYPE_BOOLEAN_2x2 (moto_boolean_2x2_get_type())

GType moto_boolean_3x3_get_type(void);
#define MOTO_TYPE_BOOLEAN_3x3 (moto_boolean_3x3_get_type())

GType moto_boolean_4x4_get_type(void);
#define MOTO_TYPE_BOOLEAN_4x4 (moto_boolean_4x4_get_type())

GType moto_int_2x2_get_type(void);
#define MOTO_TYPE_INT_2x2 (moto_int_2x2_get_type())

GType moto_int_3x3_get_type(void);
#define MOTO_TYPE_INT_3x3 (moto_int_3x3_get_type())

GType moto_int_4x4_get_type(void);
#define MOTO_TYPE_INT_4x4 (moto_int_4x4_get_type())

GType moto_float_2x2_get_type(void);
#define MOTO_TYPE_FLOAT_2x2 (moto_float_2x2_get_type())

GType moto_float_3x3_get_type(void);
#define MOTO_TYPE_FLOAT_3x3 (moto_float_3x3_get_type())

GType moto_float_4x4_get_type(void);
#define MOTO_TYPE_FLOAT_4x4 (moto_float_4x4_get_type())

// Misc types (TODO: Move image types to other place?)

GType moto_image_1d_get_type(void);
#define MOTO_TYPE_IMAGE_1D (moto_image_1d_get_type())

GType moto_image_2d_get_type(void);
#define MOTO_TYPE_IMAGE_2D (moto_image_2d_get_type())

GType moto_image_3d_get_type(void);
#define MOTO_TYPE_IMAGE_3D (moto_image_3d_get_type())

// Utility functions

void moto_types_init(void);

// Following functions return pointer to vector or matrix data if type
// is the same else generate error whithout attempt to convert.

void moto_value_get_boolean_2(GValue *value, gboolean *v0, gboolean *v1);
void moto_value_get_boolean_2_v(GValue *value, gboolean *v);
void moto_value_get_boolean_3(GValue *value, gboolean *v0, gboolean *v1, gboolean *v2);
void moto_value_get_boolean_3_v(GValue *value, gboolean *v);
void moto_value_get_boolean_4(GValue *value, gboolean *v0, gboolean *v1, gboolean *v2, gboolean *v3);
void moto_value_get_boolean_4_v(GValue *value, gboolean *v);

void moto_value_get_int_2(GValue *value, gint *v0, gint *v1);
void moto_value_get_int_2_v(GValue *value, gint *v);
void moto_value_get_int_3(GValue *value, gint *v0, gint *v1, gint *v2);
void moto_value_get_int_3_v(GValue *value, gint *v);
void moto_value_get_int_4(GValue *value, gint *v0, gint *v1, gint *v2, gint *v3);
void moto_value_get_int_4_v(GValue *value, gint *v);

void moto_value_get_float_2(GValue *value, gfloat *v0, gfloat *v1);
void moto_value_get_float_2_v(GValue *value, gfloat *v);
void moto_value_get_float_3(GValue *value, gfloat *v0, gfloat *v1, gfloat *v2);
void moto_value_get_float_3_v(GValue *value, gfloat *v);
void moto_value_get_float_4(GValue *value, gfloat *v0, gfloat *v1, gfloat *v2, gfloat *v3);
void moto_value_get_float_4_v(GValue *value, gfloat *v);

gboolean *moto_value_get_boolean_2x2(GValue *value);
gboolean *moto_value_get_boolean_3x3(GValue *value);
gboolean *moto_value_get_boolean_4x4(GValue *value);

gint *moto_value_get_int_2x2(GValue *value);
gint *moto_value_get_int_3x3(GValue *value);
gint *moto_value_get_int_4x4(GValue *value);

gfloat *moto_value_get_float_2x2(GValue *value);
gfloat *moto_value_get_float_3x3(GValue *value);
gfloat *moto_value_get_float_4x4(GValue *value);

// Following functions fill mem with vector or matrix data of needed type if types
// are transformable and return TRUE else return FALSE.

gboolean moto_value_get_as_boolean_1(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_2(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_3(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_4(GValue *value, gboolean *mem);

gboolean moto_value_get_as_int_1(GValue *value, gint *mem);
gboolean moto_value_get_as_int_2(GValue *value, gint *mem);
gboolean moto_value_get_as_int_3(GValue *value, gint *mem);
gboolean moto_value_get_as_int_4(GValue *value, gint *mem);

gboolean moto_value_get_as_float_1(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_2(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_3(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_4(GValue *value, gfloat *mem);

gboolean moto_value_get_as_boolean_1x1(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_2x2(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_3x3(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_4x4(GValue *value, gboolean *mem);

gboolean moto_value_get_as_int_2x2(GValue *value, gint *mem);
gboolean moto_value_get_as_int_3x3(GValue *value, gint *mem);
gboolean moto_value_get_as_int_4x4(GValue *value, gint *mem);

gboolean moto_value_get_as_float_2x2(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_3x3(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_4x4(GValue *value, gfloat *mem);

// Setting values

void moto_value_set_boolean_2(GValue *value, gboolean v0, gboolean v1);
void moto_value_set_boolean_2_v(GValue *value, const gboolean *v);
void moto_value_set_boolean_3(GValue *value, gboolean v0, gboolean v1, gboolean v2);
void moto_value_set_boolean_3_v(GValue *value, const gboolean *v);
void moto_value_set_boolean_4(GValue *value, gboolean v0, gboolean v1, gboolean v2, gboolean v3);
void moto_value_set_boolean_4_v(GValue *value, const gboolean *v);

void moto_value_set_int_2(GValue *value, gint v0, gint v1);
void moto_value_set_int_2_v(GValue *value, const gint *v);
void moto_value_set_int_3(GValue *value, gint v0, gint v1, gint v2);
void moto_value_set_int_3_v(GValue *value, const gint *v);
void moto_value_set_int_4(GValue *value, gint v0, gint v1, gint v2, gint v3);
void moto_value_set_int_4_v(GValue *value, const gint *v);

void moto_value_set_float_2(GValue *value, gfloat v0, gfloat v1);
void moto_value_set_float_2_v(GValue *value, const gfloat *v);
void moto_value_set_float_3(GValue *value, gfloat v0, gfloat v1, gfloat v2);
void moto_value_set_float_3_v(GValue *value, const gfloat *v);
void moto_value_set_float_4(GValue *value, gfloat v0, gfloat v1, gfloat v2, gfloat v3);
void moto_value_set_float_4_v(GValue *value, const gfloat *v);

void moto_value_set_boolean_1x1(GValue *value, gboolean v0);
void moto_value_set_boolean_1x1_v(GValue *value, const gboolean *v);
void moto_value_set_boolean_2x2(GValue *value,
        gboolean v00, gboolean v01,
        gboolean v10, gboolean v11);
void moto_value_set_boolean_2x2_v(GValue *value, const gboolean *v);
void moto_value_set_boolean_3x3(GValue *value,
        gboolean v00, gboolean v01, gboolean v02,
        gboolean v10, gboolean v11, gboolean v12,
        gboolean v20, gboolean v21, gboolean v22);
void moto_value_set_boolean_3x3_v(GValue *value, const gboolean *v);
void moto_value_set_boolean_4x4(GValue *value,
        gboolean v00, gboolean v01, gboolean v02, gboolean v03,
        gboolean v10, gboolean v11, gboolean v12, gboolean v13,
        gboolean v20, gboolean v21, gboolean v22, gboolean v23,
        gboolean v30, gboolean v31, gboolean v32, gboolean v33);
void moto_value_set_boolean_4x4_v(GValue *value, const gboolean *v);

void moto_value_set_int_1x1(GValue *value, gint v0);
void moto_value_set_int_1x1_v(GValue *value, const gint *v);
void moto_value_set_int_2x2(GValue *value,
        gint v00, gint v01,
        gint v10, gint v11);
void moto_value_set_int_2x2_v(GValue *value, const gint *v);
void moto_value_set_int_3x3(GValue *value,
        gint v00, gint v01, gint v02,
        gint v10, gint v11, gint v12,
        gint v20, gint v21, gint v22);
void moto_value_set_int_3x3_v(GValue *value, const gint *v);
void moto_value_set_int_4x4(GValue *value,
        gint v00, gint v01, gint v02, gint v03,
        gint v10, gint v11, gint v12, gint v13,
        gint v20, gint v21, gint v22, gint v23,
        gint v30, gint v31, gint v32, gint v33);
void moto_value_set_int_4x4_v(GValue *value, const gint *v);

void moto_value_set_float_1x1(GValue *value, gfloat v0);
void moto_value_set_float_1x1_v(GValue *value, const gfloat *v);
void moto_value_set_float_2x2(GValue *value,
        gfloat v00, gfloat v01,
        gfloat v10, gfloat v11);
void moto_value_set_float_2x2_v(GValue *value, const gfloat *v);
void moto_value_set_float_3x3(GValue *value,
        gfloat v00, gfloat v01, gfloat v02,
        gfloat v10, gfloat v11, gfloat v12,
        gfloat v20, gfloat v21, gfloat v22);
void moto_value_set_float_3x3_v(GValue *value, const gfloat *v);
void moto_value_set_float_4x4(GValue *value,
        gfloat v00, gfloat v01, gfloat v02, gfloat v03,
        gfloat v10, gfloat v11, gfloat v12, gfloat v13,
        gfloat v20, gfloat v21, gfloat v22, gfloat v23,
        gfloat v30, gfloat v31, gfloat v32, gfloat v33);
void moto_value_set_float_4x4_v(GValue *value, const gfloat *v);

// Setting individual element of vector or matrix

void moto_value_set_boolean_2_element(GValue *value, gsize i, gboolean v);
void moto_value_set_boolean_3_element(GValue *value, gsize i, gboolean v);
void moto_value_set_boolean_4_element(GValue *value, gsize i, gboolean v);

void moto_value_set_int_2_element(GValue *value, gsize i, gint v);
void moto_value_set_int_3_element(GValue *value, gsize i, gint v);
void moto_value_set_int_4_element(GValue *value, gsize i, gint v);

void moto_value_set_float_2_element(GValue *value, gsize i, gfloat v);
void moto_value_set_float_3_element(GValue *value, gsize i, gfloat v);
void moto_value_set_float_4_element(GValue *value, gsize i, gfloat v);

void moto_value_set_boolean_2x2_element(GValue *value, gsize i, gsize j, gboolean v);
void moto_value_set_boolean_3x3_element(GValue *value, gsize i, gsize j, gboolean v);
void moto_value_set_boolean_4x4_element(GValue *value, gsize i, gsize j, gboolean v);

void moto_value_set_int_2x2_element(GValue *value, gsize i, gsize j, gint v);
void moto_value_set_int_3x3_element(GValue *value, gsize i, gsize j, gint v);
void moto_value_set_int_4x4_element(GValue *value, gsize i, gsize j, gint v);

void moto_value_set_float_2x2_element(GValue *value, gsize i, gsize j, gfloat v);
void moto_value_set_float_3x3_element(GValue *value, gsize i, gsize j, gfloat v);
void moto_value_set_float_4x4_element(GValue *value, gsize i, gsize j, gfloat v);

G_END_DECLS

#endif /* __MOTO_TYPES_H__ */

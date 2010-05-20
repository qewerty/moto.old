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

#define MOTO_TYPE_BOOL    G_TYPE_BOOLEAN
#define MOTO_TYPE_INT         G_TYPE_INT
#define MOTO_TYPE_FLOAT       G_TYPE_FLOAT

#define MOTO_TYPE_STRING   G_TYPE_STRING
#define MOTO_TYPE_POINTER  G_TYPE_POINTER

// Vector types

GType moto_bool2_get_type(void);
#define MOTO_TYPE_BOOL2 (moto_bool2_get_type())

GType moto_bool3_get_type(void);
#define MOTO_TYPE_BOOL3 (moto_bool3_get_type())

GType moto_bool4_get_type(void);
#define MOTO_TYPE_BOOL4 (moto_bool4_get_type())

GType moto_int2_get_type(void);
#define MOTO_TYPE_INT2 (moto_int2_get_type())

GType moto_int3_get_type(void);
#define MOTO_TYPE_INT3 (moto_int3_get_type())

GType moto_int4_get_type(void);
#define MOTO_TYPE_INT4 (moto_int4_get_type())

GType moto_float2_get_type(void);
#define MOTO_TYPE_FLOAT2 (moto_float2_get_type())

GType moto_float3_get_type(void);
#define MOTO_TYPE_FLOAT3 (moto_float3_get_type())

GType moto_float4_get_type(void);
#define MOTO_TYPE_FLOAT4 (moto_float4_get_type())

GType moto_bool_array_get_type(void);
#define MOTO_TYPE_BOOL_ARRAY (moto_bool_array_get_type())

GType moto_int_array_get_type(void);
#define MOTO_TYPE_INT_ARRAY (moto_int_array_get_type())

GType moto_float_array_get_type(void);
#define MOTO_TYPE_FLOAT_ARRAY (moto_float_array_get_type())

// Matrix tyepes

GType moto_bool2x2_get_type(void);
#define MOTO_TYPE_BOOL2x2 (moto_bool2x2_get_type())

GType moto_bool3x3_get_type(void);
#define MOTO_TYPE_BOOL3x3 (moto_bool3x3_get_type())

GType moto_bool4x4_get_type(void);
#define MOTO_TYPE_BOOL4x4 (moto_bool4x4_get_type())

GType moto_int2x2_get_type(void);
#define MOTO_TYPE_INT2x2 (moto_int2x2_get_type())

GType moto_int3x3_get_type(void);
#define MOTO_TYPE_INT3x3 (moto_int3x3_get_type())

GType moto_int4x4_get_type(void);
#define MOTO_TYPE_INT4x4 (moto_int4x4_get_type())

GType moto_float2x2_get_type(void);
#define MOTO_TYPE_FLOAT2x2 (moto_float2x2_get_type())

GType moto_float3x3_get_type(void);
#define MOTO_TYPE_FLOAT3x3 (moto_float3x3_get_type())

GType moto_float4x4_get_type(void);
#define MOTO_TYPE_FLOAT4x4 (moto_float4x4_get_type())

// Misc types (TODO: Move image types to other place?)

GType moto_image_1d_get_type(void);
#define MOTO_TYPE_IMAGE_1D (moto_image_1d_get_type())

GType moto_image_2d_get_type(void);
#define MOTO_TYPE_IMAGE_2D (moto_image_2d_get_type())

GType moto_image_3d_get_type(void);
#define MOTO_TYPE_IMAGE_3D (moto_image_3d_get_type())

// Utility functions

void moto_types_init(void);

gsize moto_value_get_float_array_size(GValue *value);
gfloat* moto_value_get_float_array(GValue *value, gsize *size);
void moto_value_set_float_array(GValue *value, const gfloat* data, gsize size);

// Following functions return pointer to vector or matrix data if type
// is the same else generate error whithout attempt to convert.

void moto_value_get_bool2(GValue *value, gboolean *v0, gboolean *v1);
void moto_value_get_bool2_v(GValue *value, gboolean *v);
void moto_value_get_bool3(GValue *value, gboolean *v0, gboolean *v1, gboolean *v2);
void moto_value_get_bool3_v(GValue *value, gboolean *v);
void moto_value_get_bool4(GValue *value, gboolean *v0, gboolean *v1, gboolean *v2, gboolean *v3);
void moto_value_get_bool4_v(GValue *value, gboolean *v);

void moto_value_get_int2(GValue *value, gint *v0, gint *v1);
void moto_value_get_int2_v(GValue *value, gint *v);
void moto_value_get_int3(GValue *value, gint *v0, gint *v1, gint *v2);
void moto_value_get_int3_v(GValue *value, gint *v);
void moto_value_get_int4(GValue *value, gint *v0, gint *v1, gint *v2, gint *v3);
void moto_value_get_int4_v(GValue *value, gint *v);

void moto_value_get_float2(GValue *value, gfloat *v0, gfloat *v1);
void moto_value_get_float2_v(GValue *value, gfloat *v);
void moto_value_get_float3(GValue *value, gfloat *v0, gfloat *v1, gfloat *v2);
void moto_value_get_float3_v(GValue *value, gfloat *v);
void moto_value_get_float4(GValue *value, gfloat *v0, gfloat *v1, gfloat *v2, gfloat *v3);
void moto_value_get_float4_v(GValue *value, gfloat *v);

gboolean *moto_value_get_bool2x2(GValue *value);
gboolean *moto_value_get_bool3x3(GValue *value);
gboolean *moto_value_get_bool4x4(GValue *value);

gint *moto_value_get_int2x2(GValue *value);
gint *moto_value_get_int3x3(GValue *value);
gint *moto_value_get_int4x4(GValue *value);

gfloat *moto_value_get_float2x2(GValue *value);
gfloat *moto_value_get_float3x3(GValue *value);
gfloat *moto_value_get_float4x4(GValue *value);

// Following functions fill mem with vector or matrix data of needed type if types
// are transformable and return TRUE else return FALSE.

gboolean moto_value_get_as_bool1(GValue *value, gboolean *mem);
gboolean moto_value_get_as_bool2(GValue *value, gboolean *mem);
gboolean moto_value_get_as_bool3(GValue *value, gboolean *mem);
gboolean moto_value_get_as_bool4(GValue *value, gboolean *mem);

gboolean moto_value_get_as_int1(GValue *value, gint *mem);
gboolean moto_value_get_as_int2(GValue *value, gint *mem);
gboolean moto_value_get_as_int3(GValue *value, gint *mem);
gboolean moto_value_get_as_int4(GValue *value, gint *mem);

gboolean moto_value_get_as_float1(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float2(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float3(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float4(GValue *value, gfloat *mem);

// Setting values

void moto_value_set_bool2(GValue *value, gboolean v0, gboolean v1);
void moto_value_set_bool2_v(GValue *value, const gboolean *v);
void moto_value_set_bool3(GValue *value, gboolean v0, gboolean v1, gboolean v2);
void moto_value_set_bool3_v(GValue *value, const gboolean *v);
void moto_value_set_bool4(GValue *value, gboolean v0, gboolean v1, gboolean v2, gboolean v3);
void moto_value_set_bool4_v(GValue *value, const gboolean *v);

void moto_value_set_int2(GValue *value, gint v0, gint v1);
void moto_value_set_int2_v(GValue *value, const gint *v);
void moto_value_set_int3(GValue *value, gint v0, gint v1, gint v2);
void moto_value_set_int3_v(GValue *value, const gint *v);
void moto_value_set_int4(GValue *value, gint v0, gint v1, gint v2, gint v3);
void moto_value_set_int4_v(GValue *value, const gint *v);

void moto_value_set_float2(GValue *value, gfloat v0, gfloat v1);
void moto_value_set_float2_v(GValue *value, const gfloat *v);
void moto_value_set_float3(GValue *value, gfloat v0, gfloat v1, gfloat v2);
void moto_value_set_float3_v(GValue *value, const gfloat *v);
void moto_value_set_float4(GValue *value, gfloat v0, gfloat v1, gfloat v2, gfloat v3);
void moto_value_set_float4_v(GValue *value, const gfloat *v);

G_END_DECLS

#endif /* __MOTO_TYPES_H__ */

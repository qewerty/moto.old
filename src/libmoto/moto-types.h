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
#define MOTO_TYPE_UINT        G_TYPE_UINT
#define MOTO_TYPE_UINT_1      G_TYPE_UINT
#define MOTO_TYPE_UINT_1x1    G_TYPE_UINT
#define MOTO_TYPE_LONG        G_TYPE_LONG
#define MOTO_TYPE_LONG_1      G_TYPE_LONG
#define MOTO_TYPE_LONG_1x1    G_TYPE_LONG
#define MOTO_TYPE_ULONG       G_TYPE_ULONG
#define MOTO_TYPE_ULONG_1     G_TYPE_ULONG
#define MOTO_TYPE_ULONG_1x1   G_TYPE_ULONG
#define MOTO_TYPE_INT64       G_TYPE_INT64
#define MOTO_TYPE_INT64_1     G_TYPE_INT64
#define MOTO_TYPE_INT64_1x1   G_TYPE_INT64
#define MOTO_TYPE_UINT64      G_TYPE_UINT64
#define MOTO_TYPE_UINT64_1    G_TYPE_UINT64
#define MOTO_TYPE_UINT64_1x1  G_TYPE_UINT64
#define MOTO_TYPE_FLOAT       G_TYPE_FLOAT
#define MOTO_TYPE_FLOAT_1     G_TYPE_FLOAT
#define MOTO_TYPE_FLOAT_1x1   G_TYPE_FLOAT
#define MOTO_TYPE_DOUBLE      G_TYPE_DOUBLE
#define MOTO_TYPE_DOUBLE_1    G_TYPE_DOUBLE
#define MOTO_TYPE_DOUBLE_1x1  G_TYPE_DOUBLE

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

GType moto_uint_2_get_type(void);
#define MOTO_TYPE_UINT_2 (moto_uint_2_get_type())

GType moto_uint_3_get_type(void);
#define MOTO_TYPE_UINT_3 (moto_uint_3_get_type())

GType moto_uint_4_get_type(void);
#define MOTO_TYPE_UINT_4 (moto_uint_4_get_type())

GType moto_long_2_get_type(void);
#define MOTO_TYPE_LONG_2 (moto_long_2_get_type())

GType moto_long_3_get_type(void);
#define MOTO_TYPE_LONG_3 (moto_long_3_get_type())

GType moto_long_4_get_type(void);
#define MOTO_TYPE_LONG_4 (moto_long_4_get_type())

GType moto_ulong_2_get_type(void);
#define MOTO_TYPE_ULONG_2 (moto_ulong_2_get_type())

GType moto_ulong_3_get_type(void);
#define MOTO_TYPE_ULONG_3 (moto_ulong_3_get_type())

GType moto_ulong_4_get_type(void);
#define MOTO_TYPE_ULONG_4 (moto_ulong_4_get_type())

GType moto_int64_2_get_type(void);
#define MOTO_TYPE_INT64_2 (moto_int64_2_get_type())

GType moto_int64_3_get_type(void);
#define MOTO_TYPE_INT64_3 (moto_int64_3_get_type())

GType moto_int64_4_get_type(void);
#define MOTO_TYPE_INT64_4 (moto_int64_4_get_type())

GType moto_uint64_2_get_type(void);
#define MOTO_TYPE_UINT64_2 (moto_uint64_2_get_type())

GType moto_uint64_3_get_type(void);
#define MOTO_TYPE_UINT64_3 (moto_uint64_3_get_type())

GType moto_uint64_4_get_type(void);
#define MOTO_TYPE_UINT64_4 (moto_uint64_4_get_type())

GType moto_float_2_get_type(void);
#define MOTO_TYPE_FLOAT_2 (moto_float_2_get_type())

GType moto_float_3_get_type(void);
#define MOTO_TYPE_FLOAT_3 (moto_float_3_get_type())

GType moto_float_4_get_type(void);
#define MOTO_TYPE_FLOAT_4 (moto_float_4_get_type())

GType moto_double_2_get_type(void);
#define MOTO_TYPE_DOUBLE_2 (moto_double_2_get_type())

GType moto_double_3_get_type(void);
#define MOTO_TYPE_DOUBLE_3 (moto_double_3_get_type())

GType moto_double_4_get_type(void);
#define MOTO_TYPE_DOUBLE_4 (moto_double_4_get_type())

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

GType moto_uint_2x2_get_type(void);
#define MOTO_TYPE_UINT_2x2 (moto_uint_2x2_get_type())

GType moto_uint_3x3_get_type(void);
#define MOTO_TYPE_UINT_3x3 (moto_uint_3x3_get_type())

GType moto_uint_4x4_get_type(void);
#define MOTO_TYPE_UINT_4x4 (moto_uint_4x4_get_type())

GType moto_long_2x2_get_type(void);
#define MOTO_TYPE_LONG_2x2 (moto_long_2x2_get_type())

GType moto_long_3x3_get_type(void);
#define MOTO_TYPE_LONG_3x3 (moto_long_3x3_get_type())

GType moto_long_4x4_get_type(void);
#define MOTO_TYPE_LONG_4x4 (moto_long_4x4_get_type())

GType moto_ulong_2x2_get_type(void);
#define MOTO_TYPE_ULONG_2x2 (moto_ulong_2x2_get_type())

GType moto_ulong_3x3_get_type(void);
#define MOTO_TYPE_ULONG_3x3 (moto_ulong_3x3_get_type())

GType moto_ulong_4x4_get_type(void);
#define MOTO_TYPE_ULONG_4x4 (moto_ulong_4x4_get_type())

GType moto_int64_2x2_get_type(void);
#define MOTO_TYPE_INT64_2x2 (moto_int64_2x2_get_type())

GType moto_int64_3x3_get_type(void);
#define MOTO_TYPE_INT64_3x3 (moto_int64_3x3_get_type())

GType moto_int64_4x4_get_type(void);
#define MOTO_TYPE_INT64_4x4 (moto_int64_4x4_get_type())

GType moto_uint64_2x2_get_type(void);
#define MOTO_TYPE_UINT64_2x2 (moto_uint64_2x2_get_type())

GType moto_uint64_3x3_get_type(void);
#define MOTO_TYPE_UINT64_3x3 (moto_uint64_3x3_get_type())

GType moto_uint64_4x4_get_type(void);
#define MOTO_TYPE_UINT64_4x4 (moto_uint64_4x4_get_type())

GType moto_float_2x2_get_type(void);
#define MOTO_TYPE_FLOAT_2x2 (moto_float_2x2_get_type())

GType moto_float_3x3_get_type(void);
#define MOTO_TYPE_FLOAT_3x3 (moto_float_3x3_get_type())

GType moto_float_4x4_get_type(void);
#define MOTO_TYPE_FLOAT_4x4 (moto_float_4x4_get_type())

GType moto_double_2x2_get_type(void);
#define MOTO_TYPE_DOUBLE_2x2 (moto_double_2x2_get_type())

GType moto_double_3x3_get_type(void);
#define MOTO_TYPE_DOUBLE_3x3 (moto_double_3x3_get_type())

GType moto_double_4x4_get_type(void);
#define MOTO_TYPE_DOUBLE_4x4 (moto_double_4x4_get_type())

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

gboolean *moto_value_get_boolean_1(GValue *value);
gboolean *moto_value_get_boolean_2(GValue *value);
gboolean *moto_value_get_boolean_3(GValue *value);
gboolean *moto_value_get_boolean_4(GValue *value);

gint *moto_value_get_int_1(GValue *value);
gint *moto_value_get_int_2(GValue *value);
gint *moto_value_get_int_3(GValue *value);
gint *moto_value_get_int_4(GValue *value);

guint *moto_value_get_uint_1(GValue *value);
guint *moto_value_get_uint_2(GValue *value);
guint *moto_value_get_uint_3(GValue *value);
guint *moto_value_get_uint_4(GValue *value);

glong *moto_value_get_long_1(GValue *value);
glong *moto_value_get_long_2(GValue *value);
glong *moto_value_get_long_3(GValue *value);
glong *moto_value_get_long_4(GValue *value);

gulong *moto_value_get_ulong_1(GValue *value);
gulong *moto_value_get_ulong_2(GValue *value);
gulong *moto_value_get_ulong_3(GValue *value);
gulong *moto_value_get_ulong_4(GValue *value);

gint64 *moto_value_get_int64_1(GValue *value);
gint64 *moto_value_get_int64_2(GValue *value);
gint64 *moto_value_get_int64_3(GValue *value);
gint64 *moto_value_get_int64_4(GValue *value);

guint64 *moto_value_get_uint64_1(GValue *value);
guint64 *moto_value_get_uint64_2(GValue *value);
guint64 *moto_value_get_uint64_3(GValue *value);
guint64 *moto_value_get_uint64_4(GValue *value);

gfloat *moto_value_get_float_1(GValue *value);
gfloat *moto_value_get_float_2(GValue *value);
gfloat *moto_value_get_float_3(GValue *value);
gfloat *moto_value_get_float_4(GValue *value);

gdouble *moto_value_get_double_1(GValue *value);
gdouble *moto_value_get_double_2(GValue *value);
gdouble *moto_value_get_double_3(GValue *value);
gdouble *moto_value_get_double_4(GValue *value);

gboolean *moto_value_get_boolean_1x1(GValue *value);
gboolean *moto_value_get_boolean_2x2(GValue *value);
gboolean *moto_value_get_boolean_3x3(GValue *value);
gboolean *moto_value_get_boolean_4x4(GValue *value);

gint *moto_value_get_int_1x1(GValue *value);
gint *moto_value_get_int_2x2(GValue *value);
gint *moto_value_get_int_3x3(GValue *value);
gint *moto_value_get_int_4x4(GValue *value);

guint *moto_value_get_uint_1x1(GValue *value);
guint *moto_value_get_uint_2x2(GValue *value);
guint *moto_value_get_uint_3x3(GValue *value);
guint *moto_value_get_uint_4x4(GValue *value);

glong *moto_value_get_long_1x1(GValue *value);
glong *moto_value_get_long_2x2(GValue *value);
glong *moto_value_get_long_3x3(GValue *value);
glong *moto_value_get_long_4x4(GValue *value);

gulong *moto_value_get_ulong_1x1(GValue *value);
gulong *moto_value_get_ulong_2x2(GValue *value);
gulong *moto_value_get_ulong_3x3(GValue *value);
gulong *moto_value_get_ulong_4x4(GValue *value);

gint64 *moto_value_get_int64_1x1(GValue *value);
gint64 *moto_value_get_int64_2x2(GValue *value);
gint64 *moto_value_get_int64_3x3(GValue *value);
gint64 *moto_value_get_int64_4x4(GValue *value);

guint64 *moto_value_get_uint64_1x1(GValue *value);
guint64 *moto_value_get_uint64_2x2(GValue *value);
guint64 *moto_value_get_uint64_3x3(GValue *value);
guint64 *moto_value_get_uint64_4x4(GValue *value);

gfloat *moto_value_get_float_1x1(GValue *value);
gfloat *moto_value_get_float_2x2(GValue *value);
gfloat *moto_value_get_float_3x3(GValue *value);
gfloat *moto_value_get_float_4x4(GValue *value);

gdouble *moto_value_get_double_1x1(GValue *value);
gdouble *moto_value_get_double_2x2(GValue *value);
gdouble *moto_value_get_double_3x3(GValue *value);
gdouble *moto_value_get_double_4x4(GValue *value);

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

gboolean moto_value_get_as_uint_1(GValue *value, guint *mem);
gboolean moto_value_get_as_uint_2(GValue *value, guint *mem);
gboolean moto_value_get_as_uint_3(GValue *value, guint *mem);
gboolean moto_value_get_as_uint_4(GValue *value, guint *mem);

gboolean moto_value_get_as_long_1(GValue *value, glong *mem);
gboolean moto_value_get_as_long_2(GValue *value, glong *mem);
gboolean moto_value_get_as_long_3(GValue *value, glong *mem);
gboolean moto_value_get_as_long_4(GValue *value, glong *mem);

gboolean moto_value_get_as_ulong_1(GValue *value, gulong *mem);
gboolean moto_value_get_as_ulong_2(GValue *value, gulong *mem);
gboolean moto_value_get_as_ulong_3(GValue *value, gulong *mem);
gboolean moto_value_get_as_ulong_4(GValue *value, gulong *mem);

gboolean moto_value_get_as_int64_1(GValue *value, gint64 *mem);
gboolean moto_value_get_as_int64_2(GValue *value, gint64 *mem);
gboolean moto_value_get_as_int64_3(GValue *value, gint64 *mem);
gboolean moto_value_get_as_int64_4(GValue *value, gint64 *mem);

gboolean moto_value_get_as_uint64_1(GValue *value, guint64 *mem);
gboolean moto_value_get_as_uint64_2(GValue *value, guint64 *mem);
gboolean moto_value_get_as_uint64_3(GValue *value, guint64 *mem);
gboolean moto_value_get_as_uint64_4(GValue *value, guint64 *mem);

gboolean moto_value_get_as_float_1(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_2(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_3(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_4(GValue *value, gfloat *mem);

gboolean moto_value_get_as_double_1(GValue *value, gdouble *mem);
gboolean moto_value_get_as_double_2(GValue *value, gdouble *mem);
gboolean moto_value_get_as_double_3(GValue *value, gdouble *mem);
gboolean moto_value_get_as_double_4(GValue *value, gdouble *mem);

gboolean moto_value_get_as_boolean_1x1(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_2x2(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_3x3(GValue *value, gboolean *mem);
gboolean moto_value_get_as_boolean_4x4(GValue *value, gboolean *mem);

gboolean moto_value_get_as_int_1x1(GValue *value, gint *mem);
gboolean moto_value_get_as_int_2x2(GValue *value, gint *mem);
gboolean moto_value_get_as_int_3x3(GValue *value, gint *mem);
gboolean moto_value_get_as_int_4x4(GValue *value, gint *mem);

gboolean moto_value_get_as_uint_1x1(GValue *value, guint *mem);
gboolean moto_value_get_as_uint_2x2(GValue *value, guint *mem);
gboolean moto_value_get_as_uint_3x3(GValue *value, guint *mem);
gboolean moto_value_get_as_uint_4x4(GValue *value, guint *mem);

gboolean moto_value_get_as_long_1x1(GValue *value, glong *mem);
gboolean moto_value_get_as_long_2x2(GValue *value, glong *mem);
gboolean moto_value_get_as_long_3x3(GValue *value, glong *mem);
gboolean moto_value_get_as_long_4x4(GValue *value, glong *mem);

gboolean moto_value_get_as_ulong_1x1(GValue *value, gulong *mem);
gboolean moto_value_get_as_ulong_2x2(GValue *value, gulong *mem);
gboolean moto_value_get_as_ulong_3x3(GValue *value, gulong *mem);
gboolean moto_value_get_as_ulong_4x4(GValue *value, gulong *mem);

gboolean moto_value_get_as_int64_1x1(GValue *value, gint64 *mem);
gboolean moto_value_get_as_int64_2x2(GValue *value, gint64 *mem);
gboolean moto_value_get_as_int64_3x3(GValue *value, gint64 *mem);
gboolean moto_value_get_as_int64_4x4(GValue *value, gint64 *mem);

gboolean moto_value_get_as_uint64_1x1(GValue *value, guint64 *mem);
gboolean moto_value_get_as_uint64_2x2(GValue *value, guint64 *mem);
gboolean moto_value_get_as_uint64_3x3(GValue *value, guint64 *mem);
gboolean moto_value_get_as_uint64_4x4(GValue *value, guint64 *mem);

gboolean moto_value_get_as_float_1x1(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_2x2(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_3x3(GValue *value, gfloat *mem);
gboolean moto_value_get_as_float_4x4(GValue *value, gfloat *mem);

gboolean moto_value_get_as_double_1x1(GValue *value, gdouble *mem);
gboolean moto_value_get_as_double_2x2(GValue *value, gdouble *mem);
gboolean moto_value_get_as_double_3x3(GValue *value, gdouble *mem);
gboolean moto_value_get_as_double_4x4(GValue *value, gdouble *mem);

// Setting values

void moto_value_set_boolean_1(GValue *value, gboolean v0);
void moto_value_set_boolean_1_v(GValue *value, gboolean *v);
void moto_value_set_boolean_2(GValue *value, gboolean v0, gboolean v1);
void moto_value_set_boolean_2_v(GValue *value, gboolean *v);
void moto_value_set_boolean_3(GValue *value, gboolean v0, gboolean v1, gboolean v2);
void moto_value_set_boolean_3_v(GValue *value, gboolean *v);
void moto_value_set_boolean_4(GValue *value, gboolean v0, gboolean v1, gboolean v2, gboolean v3);
void moto_value_set_boolean_4_v(GValue *value, gboolean *v);

void moto_value_set_int_1(GValue *value, gint v0);
void moto_value_set_int_1_v(GValue *value, gint *v);
void moto_value_set_int_2(GValue *value, gint v0, gint v1);
void moto_value_set_int_2_v(GValue *value, gint *v);
void moto_value_set_int_3(GValue *value, gint v0, gint v1, gint v2);
void moto_value_set_int_3_v(GValue *value, gint *v);
void moto_value_set_int_4(GValue *value, gint v0, gint v1, gint v2, gint v3);
void moto_value_set_int_4_v(GValue *value, gint *v);

void moto_value_set_uint_1(GValue *value, guint v0);
void moto_value_set_uint_1_v(GValue *value, guint *v);
void moto_value_set_uint_2(GValue *value, guint v0, guint v1);
void moto_value_set_uint_2_v(GValue *value, guint *v);
void moto_value_set_uint_3(GValue *value, guint v0, guint v1, guint v2);
void moto_value_set_uint_3_v(GValue *value, guint *v);
void moto_value_set_uint_4(GValue *value, guint v0, guint v1, guint v2, guint v3);
void moto_value_set_uint_4_v(GValue *value, guint *v);

void moto_value_set_long_1(GValue *value, glong v0);
void moto_value_set_long_1_v(GValue *value, glong *v);
void moto_value_set_long_2(GValue *value, glong v0, glong v1);
void moto_value_set_long_2_v(GValue *value, glong *v);
void moto_value_set_long_3(GValue *value, glong v0, glong v1, glong v2);
void moto_value_set_long_3_v(GValue *value, glong *v);
void moto_value_set_long_4(GValue *value, glong v0, glong v1, glong v2, glong v3);
void moto_value_set_long_4_v(GValue *value, glong *v);

void moto_value_set_ulong_1(GValue *value, gulong v0);
void moto_value_set_ulong_1_v(GValue *value, gulong *v);
void moto_value_set_ulong_2(GValue *value, gulong v0, gulong v1);
void moto_value_set_ulong_2_v(GValue *value, gulong *v);
void moto_value_set_ulong_3(GValue *value, gulong v0, gulong v1, gulong v2);
void moto_value_set_ulong_3_v(GValue *value, gulong *v);
void moto_value_set_ulong_4(GValue *value, gulong v0, gulong v1, gulong v2, gulong v3);
void moto_value_set_ulong_4_v(GValue *value, gulong *v);

void moto_value_set_int64_1(GValue *value, gint64 v0);
void moto_value_set_int64_1_v(GValue *value, gint64 *v);
void moto_value_set_int64_2(GValue *value, gint64 v0, gint64 v1);
void moto_value_set_int64_2_v(GValue *value, gint64 *v);
void moto_value_set_int64_3(GValue *value, gint64 v0, gint64 v1, gint64 v2);
void moto_value_set_int64_3_v(GValue *value, gint64 *v);
void moto_value_set_int64_4(GValue *value, gint64 v0, gint64 v1, gint64 v2, gint64 v3);
void moto_value_set_int64_4_v(GValue *value, gint64 *v);

void moto_value_set_uint64_1(GValue *value, guint64 v0);
void moto_value_set_uint64_1_v(GValue *value, guint64 *v);
void moto_value_set_uint64_2(GValue *value, guint64 v0, guint64 v1);
void moto_value_set_uint64_2_v(GValue *value, guint64 *v);
void moto_value_set_uint64_3(GValue *value, guint64 v0, guint64 v1, guint64 v2);
void moto_value_set_uint64_3_v(GValue *value, guint64 *v);
void moto_value_set_uint64_4(GValue *value, guint64 v0, guint64 v1, guint64 v2, guint64 v3);
void moto_value_set_uint64_4_v(GValue *value, guint64 *v);

void moto_value_set_float_1(GValue *value, gfloat v0);
void moto_value_set_float_1_v(GValue *value, gfloat *v);
void moto_value_set_float_2(GValue *value, gfloat v0, gfloat v1);
void moto_value_set_float_2_v(GValue *value, gfloat *v);
void moto_value_set_float_3(GValue *value, gfloat v0, gfloat v1, gfloat v2);
void moto_value_set_float_3_v(GValue *value, gfloat *v);
void moto_value_set_float_4(GValue *value, gfloat v0, gfloat v1, gfloat v2, gfloat v3);
void moto_value_set_float_4_v(GValue *value, gfloat *v);

void moto_value_set_double_1(GValue *value, gdouble v0);
void moto_value_set_double_1_v(GValue *value, gdouble *v);
void moto_value_set_double_2(GValue *value, gdouble v0, gdouble v1);
void moto_value_set_double_2_v(GValue *value, gdouble *v);
void moto_value_set_double_3(GValue *value, gdouble v0, gdouble v1, gdouble v2);
void moto_value_set_double_3_v(GValue *value, gdouble *v);
void moto_value_set_double_4(GValue *value, gdouble v0, gdouble v1, gdouble v2, gdouble v3);
void moto_value_set_double_4_v(GValue *value, gdouble *v);

void moto_value_set_boolean_1x1(GValue *value, gboolean v0);
void moto_value_set_boolean_1x1_v(GValue *value, gboolean *v);
void moto_value_set_boolean_2x2(GValue *value,
        gboolean v00, gboolean v01,
        gboolean v10, gboolean v11);
void moto_value_set_boolean_2x2_v(GValue *value, gboolean *v);
void moto_value_set_boolean_3x3(GValue *value,
        gboolean v00, gboolean v01, gboolean v02,
        gboolean v10, gboolean v11, gboolean v12,
        gboolean v20, gboolean v21, gboolean v22);
void moto_value_set_boolean_3x3_v(GValue *value, gboolean *v);
void moto_value_set_boolean_4x4(GValue *value,
        gboolean v00, gboolean v01, gboolean v02, gboolean v03,
        gboolean v10, gboolean v11, gboolean v12, gboolean v13,
        gboolean v20, gboolean v21, gboolean v22, gboolean v23,
        gboolean v30, gboolean v31, gboolean v32, gboolean v33);
void moto_value_set_boolean_4x4_v(GValue *value, gboolean *v);

void moto_value_set_int_1x1(GValue *value, gint v0);
void moto_value_set_int_1x1_v(GValue *value, gint *v);
void moto_value_set_int_2x2(GValue *value,
        gint v00, gint v01,
        gint v10, gint v11);
void moto_value_set_int_2x2_v(GValue *value, gint *v);
void moto_value_set_int_3x3(GValue *value,
        gint v00, gint v01, gint v02,
        gint v10, gint v11, gint v12,
        gint v20, gint v21, gint v22);
void moto_value_set_int_3x3_v(GValue *value, gint *v);
void moto_value_set_int_4x4(GValue *value,
        gint v00, gint v01, gint v02, gint v03,
        gint v10, gint v11, gint v12, gint v13,
        gint v20, gint v21, gint v22, gint v23,
        gint v30, gint v31, gint v32, gint v33);
void moto_value_set_int_4x4_v(GValue *value, gint *v);

void moto_value_set_uint_1x1(GValue *value, guint v0);
void moto_value_set_uint_1x1_v(GValue *value, guint *v);
void moto_value_set_uint_2x2(GValue *value,
        guint v00, guint v01,
        guint v10, guint v11);
void moto_value_set_uint_2x2_v(GValue *value, guint *v);
void moto_value_set_uint_3x3(GValue *value,
        guint v00, guint v01, guint v02,
        guint v10, guint v11, guint v12,
        guint v20, guint v21, guint v22);
void moto_value_set_uint_3x3_v(GValue *value, guint *v);
void moto_value_set_uint_4x4(GValue *value,
        guint v00, guint v01, guint v02, guint v03,
        guint v10, guint v11, guint v12, guint v13,
        guint v20, guint v21, guint v22, guint v23,
        guint v30, guint v31, guint v32, guint v33);
void moto_value_set_uint_4x4_v(GValue *value, guint *v);

void moto_value_set_long_1x1(GValue *value, glong v0);
void moto_value_set_long_1x1_v(GValue *value, glong *v);
void moto_value_set_long_2x2(GValue *value,
        glong v00, glong v01,
        glong v10, glong v11);
void moto_value_set_long_2x2_v(GValue *value, glong *v);
void moto_value_set_long_3x3(GValue *value,
        glong v00, glong v01, glong v02,
        glong v10, glong v11, glong v12,
        glong v20, glong v21, glong v22);
void moto_value_set_long_3x3_v(GValue *value, glong *v);
void moto_value_set_long_4x4(GValue *value,
        glong v00, glong v01, glong v02, glong v03,
        glong v10, glong v11, glong v12, glong v13,
        glong v20, glong v21, glong v22, glong v23,
        glong v30, glong v31, glong v32, glong v33);
void moto_value_set_long_4x4_v(GValue *value, glong *v);

void moto_value_set_ulong_1x1(GValue *value, gulong v0);
void moto_value_set_ulong_1x1_v(GValue *value, gulong *v);
void moto_value_set_ulong_2x2(GValue *value,
        gulong v00, gulong v01,
        gulong v10, gulong v11);
void moto_value_set_ulong_2x2_v(GValue *value, gulong *v);
void moto_value_set_ulong_3x3(GValue *value,
        gulong v00, gulong v01, gulong v02,
        gulong v10, gulong v11, gulong v12,
        gulong v20, gulong v21, gulong v22);
void moto_value_set_ulong_3x3_v(GValue *value, gulong *v);
void moto_value_set_ulong_4x4(GValue *value,
        gulong v00, gulong v01, gulong v02, gulong v03,
        gulong v10, gulong v11, gulong v12, gulong v13,
        gulong v20, gulong v21, gulong v22, gulong v23,
        gulong v30, gulong v31, gulong v32, gulong v33);
void moto_value_set_ulong_4x4_v(GValue *value, gulong *v);

void moto_value_set_int64_1x1(GValue *value, gint64 v0);
void moto_value_set_int64_1x1_v(GValue *value, gint64 *v);
void moto_value_set_int64_2x2(GValue *value,
        gint64 v00, gint64 v01,
        gint64 v10, gint64 v11);
void moto_value_set_int64_2x2_v(GValue *value, gint64 *v);
void moto_value_set_int64_3x3(GValue *value,
        gint64 v00, gint64 v01, gint64 v02,
        gint64 v10, gint64 v11, gint64 v12,
        gint64 v20, gint64 v21, gint64 v22);
void moto_value_set_int64_3x3_v(GValue *value, gint64 *v);
void moto_value_set_int64_4x4(GValue *value,
        gint64 v00, gint64 v01, gint64 v02, gint64 v03,
        gint64 v10, gint64 v11, gint64 v12, gint64 v13,
        gint64 v20, gint64 v21, gint64 v22, gint64 v23,
        gint64 v30, gint64 v31, gint64 v32, gint64 v33);
void moto_value_set_int64_4x4_v(GValue *value, gint64 *v);

void moto_value_set_uint64_1x1(GValue *value, guint64 v0);
void moto_value_set_uint64_1x1_v(GValue *value, guint64 *v);
void moto_value_set_uint64_2x2(GValue *value,
        guint64 v00, guint64 v01,
        guint64 v10, guint64 v11);
void moto_value_set_uint64_2x2_v(GValue *value, guint64 *v);
void moto_value_set_uint64_3x3(GValue *value,
        guint64 v00, guint64 v01, guint64 v02,
        guint64 v10, guint64 v11, guint64 v12,
        guint64 v20, guint64 v21, guint64 v22);
void moto_value_set_uint64_3x3_v(GValue *value, guint64 *v);
void moto_value_set_uint64_4x4(GValue *value,
        guint64 v00, guint64 v01, guint64 v02, guint64 v03,
        guint64 v10, guint64 v11, guint64 v12, guint64 v13,
        guint64 v20, guint64 v21, guint64 v22, guint64 v23,
        guint64 v30, guint64 v31, guint64 v32, guint64 v33);
void moto_value_set_uint64_4x4_v(GValue *value, guint64 *v);

void moto_value_set_float_1x1(GValue *value, gfloat v0);
void moto_value_set_float_1x1_v(GValue *value, gfloat *v);
void moto_value_set_float_2x2(GValue *value,
        gfloat v00, gfloat v01,
        gfloat v10, gfloat v11);
void moto_value_set_float_2x2_v(GValue *value, gfloat *v);
void moto_value_set_float_3x3(GValue *value,
        gfloat v00, gfloat v01, gfloat v02,
        gfloat v10, gfloat v11, gfloat v12,
        gfloat v20, gfloat v21, gfloat v22);
void moto_value_set_float_3x3_v(GValue *value, gfloat *v);
void moto_value_set_float_4x4(GValue *value,
        gfloat v00, gfloat v01, gfloat v02, gfloat v03,
        gfloat v10, gfloat v11, gfloat v12, gfloat v13,
        gfloat v20, gfloat v21, gfloat v22, gfloat v23,
        gfloat v30, gfloat v31, gfloat v32, gfloat v33);
void moto_value_set_float_4x4_v(GValue *value, gfloat *v);

void moto_value_set_double_1x1(GValue *value, gdouble v0);
void moto_value_set_double_1x1_v(GValue *value, gdouble *v);
void moto_value_set_double_2x2(GValue *value,
        gdouble v00, gdouble v01,
        gdouble v10, gdouble v11);
void moto_value_set_double_2x2_v(GValue *value, gdouble *v);
void moto_value_set_double_3x3(GValue *value,
        gdouble v00, gdouble v01, gdouble v02,
        gdouble v10, gdouble v11, gdouble v12,
        gdouble v20, gdouble v21, gdouble v22);
void moto_value_set_double_3x3_v(GValue *value, gdouble *v);
void moto_value_set_double_4x4(GValue *value,
        gdouble v00, gdouble v01, gdouble v02, gdouble v03,
        gdouble v10, gdouble v11, gdouble v12, gdouble v13,
        gdouble v20, gdouble v21, gdouble v22, gdouble v23,
        gdouble v30, gdouble v31, gdouble v32, gdouble v33);
void moto_value_set_double_4x4_v(GValue *value, gdouble *v);

// Setting individual element of vector or matrix

void moto_value_set_boolean_2_element(GValue *value, gsize i, gboolean v);
void moto_value_set_boolean_3_element(GValue *value, gsize i, gboolean v);
void moto_value_set_boolean_4_element(GValue *value, gsize i, gboolean v);

void moto_value_set_int_2_element(GValue *value, gsize i, gint v);
void moto_value_set_int_3_element(GValue *value, gsize i, gint v);
void moto_value_set_int_4_element(GValue *value, gsize i, gint v);

void moto_value_set_uint_2_element(GValue *value, gsize i, guint v);
void moto_value_set_uint_3_element(GValue *value, gsize i, guint v);
void moto_value_set_uint_4_element(GValue *value, gsize i, guint v);

void moto_value_set_long_2_element(GValue *value, gsize i, glong v);
void moto_value_set_long_3_element(GValue *value, gsize i, glong v);
void moto_value_set_long_4_element(GValue *value, gsize i, glong v);

void moto_value_set_ulong_2_element(GValue *value, gsize i, gulong v);
void moto_value_set_ulong_3_element(GValue *value, gsize i, gulong v);
void moto_value_set_ulong_4_element(GValue *value, gsize i, gulong v);

void moto_value_set_int64_2_element(GValue *value, gsize i, gint64 v);
void moto_value_set_int64_3_element(GValue *value, gsize i, gint64 v);
void moto_value_set_int64_4_element(GValue *value, gsize i, gint64 v);

void moto_value_set_uint64_2_element(GValue *value, gsize i, guint64 v);
void moto_value_set_uint64_3_element(GValue *value, gsize i, guint64 v);
void moto_value_set_uint64_4_element(GValue *value, gsize i, guint64 v);

void moto_value_set_float_2_element(GValue *value, gsize i, gfloat v);
void moto_value_set_float_3_element(GValue *value, gsize i, gfloat v);
void moto_value_set_float_4_element(GValue *value, gsize i, gfloat v);

void moto_value_set_double_2_element(GValue *value, gsize i, gdouble v);
void moto_value_set_double_3_element(GValue *value, gsize i, gdouble v);
void moto_value_set_double_4_element(GValue *value, gsize i, gdouble v);

void moto_value_set_boolean_2x2_element(GValue *value, gsize i, gsize j, gboolean v);
void moto_value_set_boolean_3x3_element(GValue *value, gsize i, gsize j, gboolean v);
void moto_value_set_boolean_4x4_element(GValue *value, gsize i, gsize j, gboolean v);

void moto_value_set_int_2x2_element(GValue *value, gsize i, gsize j, gint v);
void moto_value_set_int_3x3_element(GValue *value, gsize i, gsize j, gint v);
void moto_value_set_int_4x4_element(GValue *value, gsize i, gsize j, gint v);

void moto_value_set_uint_2x2_element(GValue *value, gsize i, gsize j, guint v);
void moto_value_set_uint_3x3_element(GValue *value, gsize i, gsize j, guint v);
void moto_value_set_uint_4x4_element(GValue *value, gsize i, gsize j, guint v);

void moto_value_set_long_2x2_element(GValue *value, gsize i, gsize j, glong v);
void moto_value_set_long_3x3_element(GValue *value, gsize i, gsize j, glong v);
void moto_value_set_long_4x4_element(GValue *value, gsize i, gsize j, glong v);

void moto_value_set_ulong_2x2_element(GValue *value, gsize i, gsize j, gulong v);
void moto_value_set_ulong_3x3_element(GValue *value, gsize i, gsize j, gulong v);
void moto_value_set_ulong_4x4_element(GValue *value, gsize i, gsize j, gulong v);

void moto_value_set_int64_2x2_element(GValue *value, gsize i, gsize j, gint64 v);
void moto_value_set_int64_3x3_element(GValue *value, gsize i, gsize j, gint64 v);
void moto_value_set_int64_4x4_element(GValue *value, gsize i, gsize j, gint64 v);

void moto_value_set_uint64_2x2_element(GValue *value, gsize i, gsize j, guint64 v);
void moto_value_set_uint64_3x3_element(GValue *value, gsize i, gsize j, guint64 v);
void moto_value_set_uint64_4x4_element(GValue *value, gsize i, gsize j, guint64 v);

void moto_value_set_float_2x2_element(GValue *value, gsize i, gsize j, gfloat v);
void moto_value_set_float_3x3_element(GValue *value, gsize i, gsize j, gfloat v);
void moto_value_set_float_4x4_element(GValue *value, gsize i, gsize j, gfloat v);

void moto_value_set_double_2x2_element(GValue *value, gsize i, gsize j, gdouble v);
void moto_value_set_double_3x3_element(GValue *value, gsize i, gsize j, gdouble v);
void moto_value_set_double_4x4_element(GValue *value, gsize i, gsize j, gdouble v);

G_END_DECLS

#endif /* __MOTO_TYPES_H__ */

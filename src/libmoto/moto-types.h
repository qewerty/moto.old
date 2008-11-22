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

G_END_DECLS

#endif /* __MOTO_TYPES_H__ */

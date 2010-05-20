
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

#ifndef __MOTO_PARAM_SPEC_H__
#define __MOTO_PARAM_SPEC_H__

#include <glib-object.h>

#include "moto-types.h"

G_BEGIN_DECLS

typedef struct _MotoParamSpec      MotoParamSpec;
typedef struct _MotoParamSpecClass MotoParamSpecClass;

typedef struct _MotoParamSpecBool      MotoParamSpecBool;
typedef struct _MotoParamSpecBoolClass MotoParamSpecBoolClass;

typedef struct _MotoParamSpecInt MotoParamSpecInt;
typedef struct _MotoParamSpecIntClass MotoParamSpecIntClass;

typedef struct _MotoParamSpecFloat      MotoParamSpecFloat;
typedef struct _MotoParamSpecFloatClass MotoParamSpecFloatClass;

typedef struct _MotoParamSpecBool2      MotoParamSpecBool2;
typedef struct _MotoParamSpecBool2Class MotoParamSpecBool2Class;

typedef struct _MotoParamSpecInt2      MotoParamSpecInt2;
typedef struct _MotoParamSpecInt2Class MotoParamSpecInt2Class;

typedef struct _MotoParamSpecFloat2      MotoParamSpecFloat2;
typedef struct _MotoParamSpecFloat2Class MotoParamSpecFloat2Class;

typedef struct _MotoParamSpecBool3      MotoParamSpecBool3;
typedef struct _MotoParamSpecBool3Class MotoParamSpecBool3Class;

typedef struct _MotoParamSpecInt3      MotoParamSpecInt3;
typedef struct _MotoParamSpecInt3Class MotoParamSpecInt3Class;

typedef struct _MotoParamSpecFloat3      MotoParamSpecFloat3;
typedef struct _MotoParamSpecFloat3Class MotoParamSpecFloat3Class;

typedef struct _MotoParamSpecBool4      MotoParamSpecBool4;
typedef struct _MotoParamSpecBool4Class MotoParamSpecBool4Class;

typedef struct _MotoParamSpecInt4      MotoParamSpecInt4;
typedef struct _MotoParamSpecInt4Class MotoParamSpecInt4Class;

typedef struct _MotoParamSpecFloat4      MotoParamSpecFloat4;
typedef struct _MotoParamSpecFloat4Class MotoParamSpecFloat4Class;

typedef MotoParamSpec *(*MotoParamSpecCopyMethod)(MotoParamSpec *self);

// MotoParamSpec

struct _MotoParamSpec
{
    GObject parent;
};

struct _MotoParamSpecClass
{
    GObjectClass parent;

    GType value_type;

    MotoParamSpecCopyMethod copy;
};

GType moto_param_spec_get_type(void);

#define MOTO_TYPE_PARAM_SPEC (moto_param_spec_get_type())
#define MOTO_PARAM_SPEC(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC, MotoParamSpec))
#define MOTO_PARAM_SPEC_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC, MotoParamSpecClass))
#define MOTO_IS_PARAM_SPEC(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC))
#define MOTO_IS_PARAM_SPEC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC))
#define MOTO_PARAM_SPEC_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC, MotoParamSpecClass))

GType moto_param_spec_get_value_type(MotoParamSpec *self);

MotoParamSpec *moto_param_spec_copy(MotoParamSpec *self);

// MotoParamSpecBool

struct _MotoParamSpecBool
{
    MotoParamSpec parent;

    gboolean default_value;
};

struct _MotoParamSpecBoolClass
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_bool_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_BOOL (moto_param_spec_bool_get_type())
#define MOTO_PARAM_SPEC_BOOL(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_BOOL, MotoParamSpecBool))
#define MOTO_PARAM_SPEC_bool_class(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_BOOL, MotoParamSpecBoolClass))
#define MOTO_IS_PARAM_SPEC_BOOLEAN(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_BOOL))
#define MOTO_IS_PARAM_SPEC_bool_class(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_BOOL))
#define MOTO_PARAM_SPEC_BOOLGET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_BOOL, MotoParamSpecBoolClass))

MotoParamSpec *moto_param_spec_bool_new(gboolean default_value);

// MotoParamSpecInt

struct _MotoParamSpecInt
{
    MotoParamSpec parent;

    gint default_value;
    gint min;
    gint max;
    gint step;
    gint page_step;
};

struct _MotoParamSpecIntClass
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_int_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_INT (moto_param_spec_int_get_type())
#define MOTO_PARAM_SPEC_INT(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_INT, MotoParamSpecInt))
#define MOTO_PARAM_SPEC_int_class(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_INT, MotoParamSpecIntClass))
#define MOTO_IS_PARAM_SPEC_INT(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_INT))
#define MOTO_IS_PARAM_SPEC_int_class(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_INT))
#define MOTO_PARAM_SPEC_INTGET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_INT, MotoParamSpecIntClass))

MotoParamSpec *moto_param_spec_intnew(gint default_value, gint min, gint max, gint step, gint page_step);

// MotoParamSpecFloat

struct _MotoParamSpecFloat
{
    MotoParamSpec parent;

    gfloat default_value;
    gfloat min;
    gfloat max;
    gfloat step;
    gfloat page_step;
};

struct _MotoParamSpecFloatClass
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_float_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_FLOAT (moto_param_spec_float_get_type())
#define MOTO_PARAM_SPEC_FLOAT(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_FLOAT, MotoParamSpecFloat))
#define MOTO_PARAM_SPEC_float_class(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_FLOAT, MotoParamSpecFloatClass))
#define MOTO_IS_PARAM_SPEC_FLOAT(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT))
#define MOTO_IS_PARAM_SPEC_float_class(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_FLOAT))
#define MOTO_PARAM_SPEC_FLOATGET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT, MotoParamSpecFloatClass))

MotoParamSpec *moto_param_spec_floatnew(gfloat default_value, gfloat min, gfloat max, gfloat step, gfloat page_step);

// MotoParamSpecBool2

struct _MotoParamSpecBool2
{
    MotoParamSpec parent;

    gboolean default_value[2];
};

struct _MotoParamSpecBool2Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_bool2_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_BOOL2 (moto_param_spec_bool2_get_type())
#define MOTO_PARAM_SPEC_BOOL2(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_BOOL2, MotoParamSpecBool2))
#define MOTO_PARAM_SPEC_BOOL2_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_BOOL2, MotoParamSpecBool2Class))
#define MOTO_IS_PARAM_SPEC_BOOL2(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_BOOL2))
#define MOTO_IS_PARAM_SPEC_BOOL2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_BOOL2))
#define MOTO_PARAM_SPEC_BOOL2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_BOOL2, MotoParamSpecBool2Class))

MotoParamSpec *
moto_param_spec_bool2_new(gboolean default_value0, gboolean default_value1);

// MotoParamSpecInt2

struct _MotoParamSpecInt2
{
    MotoParamSpec parent;

    gint default_value[2];
    gint min[2];
    gint max[2];
    gint step[2];
    gint page_step[2];
};

struct _MotoParamSpecInt2Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_int2_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_INT2 (moto_param_spec_int2_get_type())
#define MOTO_PARAM_SPEC_INT2(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_INT2, MotoParamSpecInt2))
#define MOTO_PARAM_SPEC_INT2_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_INT2, MotoParamSpecInt2Class))
#define MOTO_IS_PARAM_SPEC_INT2(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_INT2))
#define MOTO_IS_PARAM_SPEC_INT2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_INT2))
#define MOTO_PARAM_SPEC_INT2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_INT2, MotoParamSpecInt2Class))

MotoParamSpec *
moto_param_spec_int2_new(gint default_value0, gint min1, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min0, gint max1, gint step1, gint page_step1);

// MotoParamSpecFloat2

struct _MotoParamSpecFloat2
{
    MotoParamSpec parent;

    gfloat default_value[2];
    gfloat min[2];
    gfloat max[2];
    gfloat step[2];
    gfloat page_step[2];
};

struct _MotoParamSpecFloat2Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_float2_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_FLOAT2 (moto_param_spec_float2_get_type())
#define MOTO_PARAM_SPEC_FLOAT2(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_FLOAT2, MotoParamSpecFloat2))
#define MOTO_PARAM_SPEC_FLOAT2_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_FLOAT2, MotoParamSpecFloat2Class))
#define MOTO_IS_PARAM_SPEC_FLOAT2(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT2))
#define MOTO_IS_PARAM_SPEC_FLOAT2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_FLOAT2))
#define MOTO_PARAM_SPEC_FLOAT2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT2, MotoParamSpecFloat2Class))

MotoParamSpec *
moto_param_spec_float2_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1);

// MotoParamSpecBool3

struct _MotoParamSpecBool3
{
    MotoParamSpec parent;

    gboolean default_value[3];
};

struct _MotoParamSpecBool3Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_bool3_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_BOOL3 (moto_param_spec_bool3_get_type())
#define MOTO_PARAM_SPEC_BOOL3(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_BOOL3, MotoParamSpecBool3))
#define MOTO_PARAM_SPEC_BOOL3_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_BOOL3, MotoParamSpecBool3Class))
#define MOTO_IS_PARAM_SPEC_BOOL3(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_BOOL3))
#define MOTO_IS_PARAM_SPEC_BOOL3_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_BOOL3))
#define MOTO_PARAM_SPEC_BOOL3_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_BOOL3, MotoParamSpecBool3Class))

MotoParamSpec *
moto_param_spec_bool3_new(gboolean default_value0, gboolean default_value1, gboolean default_value2);

// MotoParamSpecInt3

struct _MotoParamSpecInt3
{
    MotoParamSpec parent;

    gint default_value[3];
    gint min[3];
    gint max[3];
    gint step[3];
    gint page_step[3];
};

struct _MotoParamSpecInt3Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_int3_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_INT3 (moto_param_spec_int3_get_type())
#define MOTO_PARAM_SPEC_INT3(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_INT3, MotoParamSpecInt3))
#define MOTO_PARAM_SPEC_INT3_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_INT3, MotoParamSpecInt3Class))
#define MOTO_IS_PARAM_SPEC_INT3(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_INT3))
#define MOTO_IS_PARAM_SPEC_INT3_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_INT3))
#define MOTO_PARAM_SPEC_INT3_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_INT3, MotoParamSpecInt3Class))

MotoParamSpec *
moto_param_spec_int3_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1,
                          gint default_value2, gint min2, gint max2, gint step2, gint page_step2);

// MotoParamSpecFloat3

struct _MotoParamSpecFloat3
{
    MotoParamSpec parent;

    gfloat default_value[3];
    gfloat min[3];
    gfloat max[3];
    gfloat step[3];
    gfloat page_step[3];
};

struct _MotoParamSpecFloat3Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_float3_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_FLOAT3 (moto_param_spec_float3_get_type())
#define MOTO_PARAM_SPEC_FLOAT3(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_FLOAT3, MotoParamSpecFloat3))
#define MOTO_PARAM_SPEC_FLOAT3_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_FLOAT3, MotoParamSpecFloat3Class))
#define MOTO_IS_PARAM_SPEC_FLOAT3(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT3))
#define MOTO_IS_PARAM_SPEC_FLOAT3_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_FLOAT3))
#define MOTO_PARAM_SPEC_FLOAT3_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT3, MotoParamSpecFloat3Class))

MotoParamSpec *
moto_param_spec_float3_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1,
                            gfloat default_value2, gfloat min2, gfloat max2, gfloat step2, gfloat page_step2);

// MotoParamSpecBool4

struct _MotoParamSpecBool4
{
    MotoParamSpec parent;

    gboolean default_value[4];
};

struct _MotoParamSpecBool4Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_bool4_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_BOOL4 (moto_param_spec_bool4_get_type())
#define MOTO_PARAM_SPEC_BOOL4(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_BOOL4, MotoParamSpecBool4))
#define MOTO_PARAM_SPEC_BOOL4_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_BOOL4, MotoParamSpecBool4Class))
#define MOTO_IS_PARAM_SPEC_BOOL4(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_BOOL4))
#define MOTO_IS_PARAM_SPEC_BOOL4_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_BOOL4))
#define MOTO_PARAM_SPEC_BOOL4_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_BOOL4, MotoParamSpecBool4Class))

MotoParamSpec *
moto_param_spec_bool4_new(gboolean default_value0, gboolean default_value1, gboolean default_value2, gboolean default_value3);

// MotoParamSpecInt4

struct _MotoParamSpecInt4
{
    MotoParamSpec parent;

    gint default_value[4];
    gint min[4];
    gint max[4];
    gint step[4];
    gint page_step[4];
};

struct _MotoParamSpecInt4Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_int4_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_INT4 (moto_param_spec_int4_get_type())
#define MOTO_PARAM_SPEC_INT4(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_INT4, MotoParamSpecInt4))
#define MOTO_PARAM_SPEC_INT4_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_INT4, MotoParamSpecInt4Class))
#define MOTO_IS_PARAM_SPEC_INT4(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_INT4))
#define MOTO_IS_PARAM_SPEC_INT4_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_INT4))
#define MOTO_PARAM_SPEC_INT4_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_INT4, MotoParamSpecInt4Class))

MotoParamSpec *
moto_param_spec_int4_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1,
                          gint default_value2, gint min2, gint max2, gint step2, gint page_step2,
                          gint default_value3, gint min3, gint max3, gint step3, gint page_step3);

// MotoParamSpecFloat4

struct _MotoParamSpecFloat4
{
    MotoParamSpec parent;

    gfloat default_value[4];
    gfloat min[4];
    gfloat max[4];
    gfloat step[4];
    gfloat page_step[4];
};

struct _MotoParamSpecFloat4Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_float4_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_FLOAT4 (moto_param_spec_float4_get_type())
#define MOTO_PARAM_SPEC_FLOAT4(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_FLOAT4, MotoParamSpecFloat4))
#define MOTO_PARAM_SPEC_FLOAT4_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_FLOAT4, MotoParamSpecFloat4Class))
#define MOTO_IS_PARAM_SPEC_FLOAT4(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT4))
#define MOTO_IS_PARAM_SPEC_FLOAT4_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_FLOAT4))
#define MOTO_PARAM_SPEC_FLOAT4_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT4, MotoParamSpecFloat4Class))

MotoParamSpec *
moto_param_spec_float4_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1,
                            gfloat default_value2, gfloat min2, gfloat max2, gfloat step2, gfloat page_step2,
                            gfloat default_value3, gfloat min3, gfloat max3, gfloat step3, gfloat page_step3);

G_END_DECLS

#endif /* __MOTO_PARAM_SPEC_H__ */


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

#include "moto-node.h"
#include "moto-types.h"

G_BEGIN_DECLS

typedef struct _MotoParamSpec      MotoParamSpec;
typedef struct _MotoParamSpecClass MotoParamSpecClass;

typedef struct _MotoParamSpecBoolean      MotoParamSpecBoolean;
typedef struct _MotoParamSpecBooleanClass MotoParamSpecBooleanClass;

typedef struct _MotoParamSpecInt MotoParamSpecInt;
typedef struct _MotoParamSpecIntClass MotoParamSpecIntClass;

typedef struct _MotoParamSpecFloat      MotoParamSpecFloat;
typedef struct _MotoParamSpecFloatClass MotoParamSpecFloatClass;

typedef struct _MotoParamSpecBoolean_2      MotoParamSpecBoolean_2;
typedef struct _MotoParamSpecBoolean_2Class MotoParamSpecBoolean_2Class;

typedef struct _MotoParamSpecInt_2      MotoParamSpecInt_2;
typedef struct _MotoParamSpecInt_2Class MotoParamSpecInt_2Class;

typedef struct _MotoParamSpecFloat_2      MotoParamSpecFloat_2;
typedef struct _MotoParamSpecFloat_2Class MotoParamSpecFloat_2Class;

typedef struct _MotoParamSpecBoolean_3      MotoParamSpecBoolean_3;
typedef struct _MotoParamSpecBoolean_3Class MotoParamSpecBoolean_3Class;

typedef struct _MotoParamSpecInt_3      MotoParamSpecInt_3;
typedef struct _MotoParamSpecInt_3Class MotoParamSpecInt_3Class;

typedef struct _MotoParamSpecFloat_3      MotoParamSpecFloat_3;
typedef struct _MotoParamSpecFloat_3Class MotoParamSpecFloat_3Class;

typedef struct _MotoParamSpecBoolean_4      MotoParamSpecBoolean_4;
typedef struct _MotoParamSpecBoolean_4Class MotoParamSpecBoolean_4Class;

typedef struct _MotoParamSpecInt_4      MotoParamSpecInt_4;
typedef struct _MotoParamSpecInt_4Class MotoParamSpecInt_4Class;

typedef struct _MotoParamSpecFloat_4      MotoParamSpecFloat_4;
typedef struct _MotoParamSpecFloat_4Class MotoParamSpecFloat_4Class;

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

// MotoParamSpecBoolean

struct _MotoParamSpecBoolean
{
    MotoParamSpec parent;

    gboolean default_value;
};

struct _MotoParamSpecBooleanClass
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_boolean_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_BOOLEAN (moto_param_spec_boolean_get_type())
#define MOTO_PARAM_SPEC_BOOLEAN(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_BOOLEAN, MotoParamSpecBoolean))
#define MOTO_PARAM_SPEC_BOOLEAN_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_BOOLEAN, MotoParamSpecBooleanClass))
#define MOTO_IS_PARAM_SPEC_BOOLEAN(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_BOOLEAN))
#define MOTO_IS_PARAM_SPEC_BOOLEAN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_BOOLEAN))
#define MOTO_PARAM_SPEC_BOOLEAN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_BOOLEAN, MotoParamSpecBooleanClass))

MotoParamSpec *moto_param_spec_boolean_new(gboolean default_value);

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
#define MOTO_PARAM_SPEC_INT_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_INT, MotoParamSpecIntClass))
#define MOTO_IS_PARAM_SPEC_INT(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_INT))
#define MOTO_IS_PARAM_SPEC_INT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_INT))
#define MOTO_PARAM_SPEC_INT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_INT, MotoParamSpecIntClass))

MotoParamSpec *moto_param_spec_int_new(gint default_value, gint min, gint max, gint step, gint page_step);

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
#define MOTO_PARAM_SPEC_FLOAT_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_FLOAT, MotoParamSpecFloatClass))
#define MOTO_IS_PARAM_SPEC_FLOAT(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT))
#define MOTO_IS_PARAM_SPEC_FLOAT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_FLOAT))
#define MOTO_PARAM_SPEC_FLOAT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT, MotoParamSpecFloatClass))

MotoParamSpec *moto_param_spec_float_new(gfloat default_value, gfloat min, gfloat max, gfloat step, gfloat page_step);

// MotoParamSpecBoolean_2

struct _MotoParamSpecBoolean_2
{
    MotoParamSpec parent;

    gboolean default_value[2];
};

struct _MotoParamSpecBoolean_2Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_boolean_2_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_BOOLEAN_2 (moto_param_spec_boolean_2_get_type())
#define MOTO_PARAM_SPEC_BOOLEAN_2(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_BOOLEAN_2, MotoParamSpecBoolean_2))
#define MOTO_PARAM_SPEC_BOOLEAN_2_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_BOOLEAN_2, MotoParamSpecBoolean_2Class))
#define MOTO_IS_PARAM_SPEC_BOOLEAN_2(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_BOOLEAN_2))
#define MOTO_IS_PARAM_SPEC_BOOLEAN_2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_BOOLEAN_2))
#define MOTO_PARAM_SPEC_BOOLEAN_2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_BOOLEAN_2, MotoParamSpecBoolean_2Class))

MotoParamSpec *
moto_param_spec_boolean_2_new(gboolean default_value0, gboolean default_value1);

// MotoParamSpecInt_2

struct _MotoParamSpecInt_2
{
    MotoParamSpec parent;

    gint default_value[2];
    gint min[2];
    gint max[2];
    gint step[2];
    gint page_step[2];
};

struct _MotoParamSpecInt_2Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_int_2_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_INT_2 (moto_param_spec_int_2_get_type())
#define MOTO_PARAM_SPEC_INT_2(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_INT_2, MotoParamSpecInt_2))
#define MOTO_PARAM_SPEC_INT_2_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_INT_2, MotoParamSpecInt_2Class))
#define MOTO_IS_PARAM_SPEC_INT_2(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_INT_2))
#define MOTO_IS_PARAM_SPEC_INT_2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_INT_2))
#define MOTO_PARAM_SPEC_INT_2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_INT_2, MotoParamSpecInt_2Class))

MotoParamSpec *
moto_param_spec_int_2_new(gint default_value0, gint min1, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min0, gint max1, gint step1, gint page_step1);

// MotoParamSpecFloat_2

struct _MotoParamSpecFloat_2
{
    MotoParamSpec parent;

    gfloat default_value[2];
    gfloat min[2];
    gfloat max[2];
    gfloat step[2];
    gfloat page_step[2];
};

struct _MotoParamSpecFloat_2Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_float_2_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_FLOAT_2 (moto_param_spec_float_2_get_type())
#define MOTO_PARAM_SPEC_FLOAT_2(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_FLOAT_2, MotoParamSpecFloat_2))
#define MOTO_PARAM_SPEC_FLOAT_2_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_FLOAT_2, MotoParamSpecFloat_2Class))
#define MOTO_IS_PARAM_SPEC_FLOAT_2(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT_2))
#define MOTO_IS_PARAM_SPEC_FLOAT_2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_FLOAT_2))
#define MOTO_PARAM_SPEC_FLOAT_2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT_2, MotoParamSpecFloat_2Class))

MotoParamSpec *
moto_param_spec_float_2_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1);

// MotoParamSpecBoolean_3

struct _MotoParamSpecBoolean_3
{
    MotoParamSpec parent;

    gboolean default_value[3];
};

struct _MotoParamSpecBoolean_3Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_boolean_3_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_BOOLEAN_3 (moto_param_spec_boolean_3_get_type())
#define MOTO_PARAM_SPEC_BOOLEAN_3(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_BOOLEAN_3, MotoParamSpecBoolean_3))
#define MOTO_PARAM_SPEC_BOOLEAN_3_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_BOOLEAN_3, MotoParamSpecBoolean_3Class))
#define MOTO_IS_PARAM_SPEC_BOOLEAN_3(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_BOOLEAN_3))
#define MOTO_IS_PARAM_SPEC_BOOLEAN_3_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_BOOLEAN_3))
#define MOTO_PARAM_SPEC_BOOLEAN_3_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_BOOLEAN_3, MotoParamSpecBoolean_3Class))

MotoParamSpec *
moto_param_spec_boolean_3_new(gboolean default_value0, gboolean default_value1, gboolean default_value2);

// MotoParamSpecInt_3

struct _MotoParamSpecInt_3
{
    MotoParamSpec parent;

    gint default_value[3];
    gint min[3];
    gint max[3];
    gint step[3];
    gint page_step[3];
};

struct _MotoParamSpecInt_3Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_int_3_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_INT_3 (moto_param_spec_int_3_get_type())
#define MOTO_PARAM_SPEC_INT_3(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_INT_3, MotoParamSpecInt_3))
#define MOTO_PARAM_SPEC_INT_3_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_INT_3, MotoParamSpecInt_3Class))
#define MOTO_IS_PARAM_SPEC_INT_3(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_INT_3))
#define MOTO_IS_PARAM_SPEC_INT_3_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_INT_3))
#define MOTO_PARAM_SPEC_INT_3_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_INT_3, MotoParamSpecInt_3Class))

MotoParamSpec *
moto_param_spec_int_3_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1,
                          gint default_value2, gint min2, gint max2, gint step2, gint page_step2);

// MotoParamSpecFloat_3

struct _MotoParamSpecFloat_3
{
    MotoParamSpec parent;

    gfloat default_value[3];
    gfloat min[3];
    gfloat max[3];
    gfloat step[3];
    gfloat page_step[3];
};

struct _MotoParamSpecFloat_3Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_float_3_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_FLOAT_3 (moto_param_spec_float_3_get_type())
#define MOTO_PARAM_SPEC_FLOAT_3(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_FLOAT_3, MotoParamSpecFloat_3))
#define MOTO_PARAM_SPEC_FLOAT_3_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_FLOAT_3, MotoParamSpecFloat_3Class))
#define MOTO_IS_PARAM_SPEC_FLOAT_3(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT_3))
#define MOTO_IS_PARAM_SPEC_FLOAT_3_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_FLOAT_3))
#define MOTO_PARAM_SPEC_FLOAT_3_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT_3, MotoParamSpecFloat_3Class))

MotoParamSpec *
moto_param_spec_float_3_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1,
                            gfloat default_value2, gfloat min2, gfloat max2, gfloat step2, gfloat page_step2);

// MotoParamSpecBoolean_4

struct _MotoParamSpecBoolean_4
{
    MotoParamSpec parent;

    gboolean default_value[4];
};

struct _MotoParamSpecBoolean_4Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_boolean_4_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_BOOLEAN_4 (moto_param_spec_boolean_4_get_type())
#define MOTO_PARAM_SPEC_BOOLEAN_4(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_BOOLEAN_4, MotoParamSpecBoolean_4))
#define MOTO_PARAM_SPEC_BOOLEAN_4_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_BOOLEAN_4, MotoParamSpecBoolean_4Class))
#define MOTO_IS_PARAM_SPEC_BOOLEAN_4(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_BOOLEAN_4))
#define MOTO_IS_PARAM_SPEC_BOOLEAN_4_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_BOOLEAN_4))
#define MOTO_PARAM_SPEC_BOOLEAN_4_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_BOOLEAN_4, MotoParamSpecBoolean_4Class))

MotoParamSpec *
moto_param_spec_boolean_4_new(gboolean default_value0, gboolean default_value1, gboolean default_value2, gboolean default_value3);

// MotoParamSpecInt_4

struct _MotoParamSpecInt_4
{
    MotoParamSpec parent;

    gint default_value[4];
    gint min[4];
    gint max[4];
    gint step[4];
    gint page_step[4];
};

struct _MotoParamSpecInt_4Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_int_4_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_INT_4 (moto_param_spec_int_4_get_type())
#define MOTO_PARAM_SPEC_INT_4(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_INT_4, MotoParamSpecInt_4))
#define MOTO_PARAM_SPEC_INT_4_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_INT_4, MotoParamSpecInt_4Class))
#define MOTO_IS_PARAM_SPEC_INT_4(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_INT_4))
#define MOTO_IS_PARAM_SPEC_INT_4_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_INT_4))
#define MOTO_PARAM_SPEC_INT_4_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_INT_4, MotoParamSpecInt_4Class))

MotoParamSpec *
moto_param_spec_int_4_new(gint default_value0, gint min0, gint max0, gint step0, gint page_step0,
                          gint default_value1, gint min1, gint max1, gint step1, gint page_step1,
                          gint default_value2, gint min2, gint max2, gint step2, gint page_step2,
                          gint default_value3, gint min3, gint max3, gint step3, gint page_step3);

// MotoParamSpecFloat_4

struct _MotoParamSpecFloat_4
{
    MotoParamSpec parent;

    gfloat default_value[4];
    gfloat min[4];
    gfloat max[4];
    gfloat step[4];
    gfloat page_step[4];
};

struct _MotoParamSpecFloat_4Class
{
    MotoParamSpecClass parent;
};

GType moto_param_spec_float_4_get_type(void);

#define MOTO_TYPE_PARAM_SPEC_FLOAT_4 (moto_param_spec_float_4_get_type())
#define MOTO_PARAM_SPEC_FLOAT_4(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC_FLOAT_4, MotoParamSpecFloat_4))
#define MOTO_PARAM_SPEC_FLOAT_4_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC_FLOAT_4, MotoParamSpecFloat_4Class))
#define MOTO_IS_PARAM_SPEC_FLOAT_4(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT_4))
#define MOTO_IS_PARAM_SPEC_FLOAT_4_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC_FLOAT_4))
#define MOTO_PARAM_SPEC_FLOAT_4_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC_FLOAT_4, MotoParamSpecFloat_4Class))

MotoParamSpec *
moto_param_spec_float_4_new(gfloat default_value0, gfloat min0, gfloat max0, gfloat step0, gfloat page_step0,
                            gfloat default_value1, gfloat min1, gfloat max1, gfloat step1, gfloat page_step1,
                            gfloat default_value2, gfloat min2, gfloat max2, gfloat step2, gfloat page_step2,
                            gfloat default_value3, gfloat min3, gfloat max3, gfloat step3, gfloat page_step3);

G_END_DECLS

#endif /* __MOTO_PARAM_SPEC_H__ */

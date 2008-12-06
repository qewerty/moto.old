
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

G_BEGIN_DECLS

typedef struct _MotoParamSpec      MotoParamSpec;
typedef struct _MotoParamSpecClass MotoParamSpecClass;

typedef struct _MotoParamSpecBoolean      MotoParamSpecBoolean;
typedef struct _MotoParamSpecBooleanClass MotoParamSpecBooleanClass;

typedef struct _MotoParamSpecInt MotoParamSpecInt;
typedef struct _MotoParamSpecInt MotoParamSpecIntClass;

typedef struct _MotoParamSpecFloat      MotoParamSpecFloat;
typedef struct _MotoParamSpecFloatClass MotoParamSpecFloatClass;

// MotoParamSpec

struct _MotoParamSpec
{
    GInitiallyUnowned parent;
};

struct _MotoParamSpecClass
{
    GInitiallyUnownedClass parent;

    GType value_type;
};

GType moto_param_spec_get_type(void);

#define MOTO_TYPE_PARAM_SPEC (moto_param_spec_get_type())
#define MOTO_PARAM_SPEC(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_SPEC, MotoParamSpec))
#define MOTO_PARAM_SPEC_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_SPEC, MotoParamSpecClass))
#define MOTO_IS_PARAM_SPEC(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_SPEC))
#define MOTO_IS_PARAM_SPEC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_SPEC))
#define MOTO_PARAM_SPEC_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_SPEC, MotoParamSpecClass))

GType moto_param_spec_get_value_type(MotoParamSpec *self);

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

G_END_DECLS

#endif /* __MOTO_PARAM_SPEC_H__ */

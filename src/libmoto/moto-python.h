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

#ifndef __MOTO_PYTHON_H__
#define __MOTO_PYTHON_H__

#include <Python.h>
#include <glib-object.h>

G_BEGIN_DECLS

// Py -> moto

typedef gboolean (*MotoGValueFromPyObjectFunc)(GValue *v, PyObject *obj);

gboolean moto_boolfrom_PyObject(gboolean *v, PyObject *obj);
gboolean moto_bool2_from_PyObject(gboolean *v, PyObject *obj);
gboolean moto_bool3_from_PyObject(gboolean *v, PyObject *obj);
gboolean moto_bool4_from_PyObject(gboolean *v, PyObject *obj);

gboolean moto_intfrom_PyObject(gint *v, PyObject *obj);
gboolean moto_int2_from_PyObject(gint *v, PyObject *obj);
gboolean moto_int3_from_PyObject(gint *v, PyObject *obj);
gboolean moto_int4_from_PyObject(gint *v, PyObject *obj);

gboolean moto_floatfrom_PyObject(gfloat *v, PyObject *obj);
gboolean moto_float2_from_PyObject(gfloat *v, PyObject *obj);
gboolean moto_float3_from_PyObject(gfloat *v, PyObject *obj);
gboolean moto_float4_from_PyObject(gfloat *v, PyObject *obj);

gboolean moto_string_from_PyObject(gchar **v, PyObject *obj);

gboolean moto_GValue_from_PyObject(GValue *v, PyObject *obj);

PyObject *moto_PyFunction_from_args_and_body(const gchar *argsdef, const gchar *body);

// moto -> Py

PyObject *moto_PyObject_from_GValue(GValue *v);

G_END_DECLS

#endif /* __MOTO_PYTHON_H__ */

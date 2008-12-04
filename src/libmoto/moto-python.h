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

#include <glib-object.h>
#include <Python.h>

G_BEGIN_DECLS

typedef gboolean (*MotoGValueFromPyObjectFunc)(GValue *v, PyObject *obj);

gboolean moto_boolean_from_PyObject(gboolean *v, PyObject *obj);
gboolean moto_boolean_2_from_PyObject(gboolean *v, PyObject *obj);
gboolean moto_boolean_3_from_PyObject(gboolean *v, PyObject *obj);
gboolean moto_boolean_4_from_PyObject(gboolean *v, PyObject *obj);

gboolean moto_int_from_PyObject(gint *v, PyObject *obj);
gboolean moto_int_2_from_PyObject(gint *v, PyObject *obj);
gboolean moto_int_3_from_PyObject(gint *v, PyObject *obj);
gboolean moto_int_4_from_PyObject(gint *v, PyObject *obj);

gboolean moto_float_from_PyObject(gfloat *v, PyObject *obj);
gboolean moto_float_2_from_PyObject(gfloat *v, PyObject *obj);
gboolean moto_float_3_from_PyObject(gfloat *v, PyObject *obj);
gboolean moto_float_4_from_PyObject(gfloat *v, PyObject *obj);

gboolean moto_GValue_from_PyObject(GValue *v, PyObject *obj);

G_END_DECLS

#endif /* __MOTO_PYTHON_H__ */

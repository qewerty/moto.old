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

#ifndef __MOTO_BREP_H__
#define __MOTO_BREP_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MotoBRep MotoBRep;
typedef struct _MotoBRepClass MotoBRepClass;

typedef struct _MotoBRepCreator MotoBRepCreator;
typedef struct _MotoBRepCreatorClass MotoBRepCreatorClass;

typedef void (*MotoBRepMethodClear)(MotoBRep *self);

/* class MotoBRep */

struct _MotoBRep
{
    GInitiallyUnowned parent;
};

struct _MotoBRepClass
{
    GInitiallyUnownedClass parent;

    MotoBRepMethodClear clear;
};

GType moto_brep_get_type(void);

#define MOTO_TYPE_BREP (moto_brep_get_type())
#define MOTO_BREP(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_BREP, MotoBRep))
#define MOTO_BREP_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_BREP_, MotoBRepClass))
#define MOTO_IS_BREP_(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_BREP))
#define MOTO_IS_BREP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_BREP))
#define MOTO_BREP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_BREP, MotoBRepClass))

void moto_brep_clear(MotoBRep *self);
gboolean moto_brep_is_empty(MotoBRep *self);

void moto_brep_update(MotoBRep *self);

G_END_DECLS

#endif /* __MOTO_BREP_H__ */


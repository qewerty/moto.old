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

#ifndef __MOTO_GEOM_H__
#define __MOTO_GEOM_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MotoGeom MotoGeom;
typedef struct _MotoGeomClass MotoGeomClass;

typedef struct _MotoGeomCreator MotoGeomCreator;
typedef struct _MotoGeomCreatorClass MotoGeomCreatorClass;

typedef gboolean (*MotoGeomPrepareMethod)(MotoGeom *self);

/* class MotoGeom */

struct _MotoGeom
{
    GInitiallyUnowned parent;
};

struct _MotoGeomClass
{
    GInitiallyUnownedClass parent;

    MotoGeomPrepareMethod prepare;
};

GType moto_geom_get_type(void);

#define MOTO_TYPE_GEOM (moto_geom_get_type())
#define MOTO_GEOM(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOM, MotoGeom))
#define MOTO_GEOM_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOM_, MotoGeomClass))
#define MOTO_IS_GEOM_(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOM))
#define MOTO_IS_GEOM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOM))
#define MOTO_GEOM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOM, MotoGeomClass))

void moto_geom_update(MotoGeom *self);

gboolean moto_geom_prepare(MotoGeom *self);

G_END_DECLS

#endif /* __MOTO_GEOM_H__ */


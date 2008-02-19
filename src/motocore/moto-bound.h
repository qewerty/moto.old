
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

#ifndef MOTO_BOUND_H
#define MOTO_BOUND_H

#include "glib-object.h"

typedef struct _MotoBound MotoBound;
typedef struct _MotoBoundClass MotoBoundClass;
typedef struct _MotoBoundPriv MotoBoundPriv;

/* class MotoBound */

struct _MotoBound
{
    GObject parent;

    MotoBoundPriv *priv;
};

struct _MotoBoundClass
{
    GObjectClass parent;
};

GType moto_bound_get_type(void);

#define MOTO_TYPE_BOUND (moto_bound_get_type())
#define MOTO_BOUND(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_BOUND, MotoBound))
#define MOTO_BOUND_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_BOUND, MotoBoundClass))
#define MOTO_IS_BOUND(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_BOUND))
#define MOTO_IS_BOUND_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_BOUND))
#define MOTO_BOUND_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_BOUND, MotoBoundClass))

MotoBound *moto_bound_new(gfloat minx, gfloat maxx,
                          gfloat miny, gfloat maxy,
                          gfloat minz, gfloat maxz);

MotoBound *moto_bound_new_from_array(gfloat array[6]);

void moto_bound_draw(MotoBound *self);

#endif /* MOTO_BOUND_H */

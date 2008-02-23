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

#ifndef MOTO_SYSTEM_H
#define MOTO_SYSTEM_H

#include <glib-object.h>

#include "moto-forward.h"

/* class MotoSystem */

struct _MotoSystem
{
    GObject parent;

    MotoSystemPriv *priv;
};

struct _MotoSystemClass
{
    GObjectClass parent;
};

GType moto_system_get_type(void);

#define MOTO_TYPE_SYSTEM (moto_system_get_type())
#define MOTO_SYSTEM(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SYSTEM, MotoSystem))
#define MOTO_SYSTEM_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SYSTEM, MotoSystemClass))
#define MOTO_IS_SYSTEM(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SYSTEM))
#define MOTO_IS_SYSTEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SYSTEM))
#define MOTO_SYSTEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SYSTEM, MotoSystemClass))

MotoSystem *moto_system_new();

MotoWorld *moto_system_get_current_world(MotoSystem *self);
void moto_system_add_world(MotoSystem *self, MotoWorld *world, gboolean set_current);
void moto_system_set_world_current(MotoSystem *self, MotoWorld *world);

#endif /* MOTO_SYSTEM_H */


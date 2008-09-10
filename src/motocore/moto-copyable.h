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

#ifndef MOTO_COPYABLE_H
#define MOTO_COPYABLE_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MotoCopyable MotoCopyable;
typedef struct _MotoCopyableIface MotoCopyableIface;

typedef MotoCopyable *(*MotoCopyableCopyMethod)(MotoCopyable *self);

struct _MotoCopyableIface
{
    GTypeInterface parent;

    MotoCopyableCopyMethod         copy;
};

#define MOTO_TYPE_COPYABLE           (moto_copyable_get_type())
#define MOTO_COPYABLE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COPYABLE, MotoCopyable))
#define MOTO_IS_COPYABLE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COPYABLE))
#define MOTO_COPYABLE_GET_INTERFACE(inst)   (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MOTO_TYPE_COPYABLE, MotoCopyableIface))

GType moto_copyable_get_type(void) G_GNUC_CONST;

MotoCopyable *moto_copyable_copy(MotoCopyable *self);

G_END_DECLS

#endif /* MOTO_COPYABLE_H */


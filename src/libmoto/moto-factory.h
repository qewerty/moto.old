
/* ##################################################################################
#
#  Moto Animation Factory (http://motoanim.sf.net)
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

#ifndef __MOTO_FACTORY_H__
#define __MOTO_FACTORY_H__

#include <glib.h>

G_BEGIN_DECLS

/* class MotoFactory */

typedef struct _MotoFactory MotoFactory;

typedef gpointer (*MotoFactoryCreateFunc)(gpointer user_data);
typedef void (*MotoFactoryFreeFunc)(GQuark key_id, gpointer data, gpointer user_data);

struct _MotoFactory
{
    MotoFactoryCreateFunc   create_object;
    MotoFactoryFreeFunc     free_object;

    gpointer user_data;

    GData *dl;
};

void moto_factory_init(MotoFactory *self,
        MotoFactoryCreateFunc create_object, MotoFactoryFreeFunc free_object,
        gpointer user_data);
void moto_factory_free_all(MotoFactory *self);

gpointer moto_factory_get(MotoFactory *self, const gchar *key);

G_END_DECLS

#endif /* __MOTO_FACTORY_H__ */


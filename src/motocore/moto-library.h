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

#ifndef MOTO_LIBRARY_H
#define MOTO_LIBRARY_H

#include "glib-object.h"

typedef struct _MotoLibrary MotoLibrary;
typedef struct _MotoLibraryClass MotoLibraryClass;
typedef struct _MotoLibraryPriv MotoLibraryPriv;

/* class MotoLibrary */

struct _MotoLibrary
{
    GObject parent;

    MotoLibraryPriv *priv;
};

struct _MotoLibraryClass
{
    GObjectClass parent;
};

GType moto_library_get_type(void);

#define MOTO_TYPE_LIBRARY (moto_library_get_type())
#define MOTO_LIBRARY(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_LIBRARY, MotoLibrary))
#define MOTO_LIBRARY_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_LIBRARY, MotoLibraryClass))
#define MOTO_IS_LIBRARY(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_LIBRARY))
#define MOTO_IS_LIBRARY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_LIBRARY))
#define MOTO_LIBRARY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_LIBRARY, MotoLibraryClass))

MotoLibrary *moto_library_new(const gchar *name);

void moto_library_new_slot(MotoLibrary *self, const gchar *slot_name, GType type);
/* void moto_library_delete_slot(MotoLibrary *self, const gchar *slot_name); */

/* Library check type of entry for given slot and returns FALSE if entry is not added. */
gboolen moto_library_new_entry(MotoLibrary *self,
        const gchar *slot_name, const gchar *entry_name, gpointer entry);
gpointer moto_library_get_entry(MotoLibrary *self,
        const gchar *slot_name, const gchar *entry_name);

#endif /* MOTO_LIBRARY_H */

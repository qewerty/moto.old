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

/* WARNING! Simple shelf only for testing. */

#ifndef __MOTO_SHELF_H__
#define __MOTO_SHELF_H__

#include <gtk/gtk.h>

#include "libmoto/moto-system.h"

G_BEGIN_DECLS

typedef struct _MotoShelf MotoShelf;
typedef struct _MotoShelfClass MotoShelfClass;
typedef struct _MotoShelfPriv MotoShelfPriv;

typedef void (*MotoShelfFunc)(MotoShelf *shelf, MotoSystem *system);

/* class MotoShelf */

struct _MotoShelf
{
    GtkNotebook parent;

    MotoShelfPriv *priv;
};

struct _MotoShelfClass
{
    GtkNotebookClass parent;
};

GType moto_shelf_get_type(void);

#define MOTO_TYPE_SHELF (moto_shelf_get_type())
#define MOTO_SHELF(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SHELF, MotoShelf))
#define MOTO_SHELF_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SHELF, MotoShelfClass))
#define MOTO_IS_SHELF(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SHELF))
#define MOTO_IS_SHELF_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SHELF))
#define MOTO_SHELF_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SHELF, MotoShelfClass))

GtkWidget *moto_shelf_new(MotoSystem *system, GtkWindow *window);

void moto_shelf_add_tab(MotoShelf *self, const gchar *name);
void moto_shelf_add_item(MotoShelf *self, const gchar *tab_name, const gchar *name, MotoShelfFunc func);

G_END_DECLS

#endif /* __MOTO_SHELF_H__ */


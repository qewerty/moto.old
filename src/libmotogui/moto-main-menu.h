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

#ifndef MOTO_MAIN_MENU_H
#define MOTO_MAIN_MENU_H

#include <gtk/gtk.h>

typedef struct _MotoMainMenu MotoMainMenu;
typedef struct _MotoMainMenuClass MotoMainMenuClass;
typedef struct _MotoMainMenuPriv MotoMainMenuPriv;

/* class MotoMainMenu */

struct _MotoMainMenu
{
    GtkMenuBar parent;

    MotoMainMenuPriv *priv;
};

struct _MotoMainMenuClass
{
    GtkMenuBarClass parent;
};

GType moto_main_menu_get_type(void);

#define MOTO_TYPE_MAIN_MENU (moto_main_menu_get_type())
#define MOTO_MAIN_MENU(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MAIN_MENU, MotoMainMenu))
#define MOTO_MAIN_MENU_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MAIN_MENU, MotoMainMenuClass))
#define MOTO_IS_MAIN_MENU(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MAIN_MENU))
#define MOTO_IS_MAIN_MENU_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MAIN_MENU))
#define MOTO_MAIN_MENU_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MAIN_MENU, MotoMainMenuClass))

GtkWidget *moto_main_menu_new();

#endif /* MOTO_MAIN_MENU_H */

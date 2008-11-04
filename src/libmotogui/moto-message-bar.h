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

/* WARNING! Simple message_bar only for testing. */

#ifndef __MOTO_MESSAGE_BAR_H__
#define __MOTO_MESSAGE_BAR_H__

#include <gtk/gtk.h>

#include "libmoto/moto-messager.h"

G_BEGIN_DECLS

typedef struct _MotoMessageBar MotoMessageBar;
typedef struct _MotoMessageBarClass MotoMessageBarClass;
typedef struct _MotoMessageBarPriv MotoMessageBarPriv;

/* class MotoMessageBar */

struct _MotoMessageBar
{
    GtkStatusbar parent;

    MotoMessageBarPriv *priv;
};

struct _MotoMessageBarClass
{
    GtkStatusbarClass parent;
};

GType moto_message_bar_get_type(void);

#define MOTO_TYPE_MESSAGE_BAR (moto_message_bar_get_type())
#define MOTO_MESSAGE_BAR(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MESSAGE_BAR, MotoMessageBar))
#define MOTO_MESSAGE_BAR_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MESSAGE_BAR, MotoMessageBarClass))
#define MOTO_IS_MESSAGE_BAR(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MESSAGE_BAR))
#define MOTO_IS_MESSAGE_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MESSAGE_BAR))
#define MOTO_MESSAGE_BAR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MESSAGE_BAR, MotoMessageBarClass))

GtkWidget *moto_message_bar_new(MotoMessager *messager);

G_END_DECLS

#endif /* __MOTO_MESSAGE_BAR_H__ */



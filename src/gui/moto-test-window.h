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

#ifndef MOTO_TEST_WINDOW_H
#define MOTO_TEST_WINDOW_H

#include <gtk/gtk.h>

typedef struct _MotoTestWindow MotoTestWindow;
typedef struct _MotoTestWindowClass MotoTestWindowClass;
typedef struct _MotoTestWindowPriv MotoTestWindowPriv;

/* class MotoTestWindow */

struct _MotoTestWindow
{
    GtkWindow parent;

    MotoTestWindowPriv *priv;
};

struct _MotoTestWindowClass
{
    GtkWindowClass parent;
};

GType moto_test_window_get_type(void);

#define MOTO_TYPE_TEST_WINDOW (moto_test_window_get_type())
#define MOTO_TEST_WINDOW(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_TEST_WINDOW, MotoTestWindow))
#define MOTO_TEST_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_TEST_WINDOW, MotoTestWindowClass))
#define MOTO_IS_TEST_WINDOW(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_TEST_WINDOW))
#define MOTO_IS_TEST_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_TEST_WINDOW))
#define MOTO_TEST_WINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_TEST_WINDOW, MotoTestWindowClass))

GtkWindow *moto_test_window_new();

void moto_test_window_redraw_3dview(MotoTestWindow *self);
void moto_test_window_update_param_editor(MotoTestWindow *self);

#endif /* MOTO_TEST_WINDOW_H */

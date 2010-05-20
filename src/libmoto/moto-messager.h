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

#ifndef __MOTO_MESSAGER_H__
#define __MOTO_MESSAGER_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MotoMessager MotoMessager;
typedef struct _MotoMessagerClass MotoMessagerClass;

/* class MotoMessager */

struct _MotoMessager
{
    GObject parent;
};

struct _MotoMessagerClass
{
    GObjectClass parent;

    /* signal */
    guint info_message_signal_id;
    guint warning_message_signal_id;
    guint error_message_signal_id;
};

GType moto_messager_get_type(void);

#define MOTO_TYPE_MESSAGER (moto_messager_get_type())
#define MOTO_MESSAGER(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MESSAGER, MotoMessager))
#define MOTO_MESSAGER_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MESSAGER, MotoMessagerClass))
#define MOTO_IS_MESSAGER(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MESSAGER))
#define MOTO_IS_MESSAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MESSAGER))
#define MOTO_MESSAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MESSAGER, MotoMessagerClass))

MotoMessager *moto_messager_singleton(void);
void moto_messager_set_printmessages(gboolean status);
gboolean moto_messager_get_printmessages(void);

void moto_info(const gchar *format, ...)  G_GNUC_PRINTF(1, 2);
void moto_warning(const gchar *format, ...)  G_GNUC_PRINTF(1, 2);
void moto_error(const gchar *format, ...)  G_GNUC_PRINTF(1, 2);

G_END_DECLS

#endif /* __MOTO_MESSAGER_H__ */

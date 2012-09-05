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

/* WARNING! Simple inspector only for testing. */

#ifndef __MOTO_INSPECTOR_H__
#define __MOTO_INSPECTOR_H__

#include <gtk/gtk.h>

#include "libmoto/moto-scene-node.h"
#include "moto-test-window.h"

G_BEGIN_DECLS

typedef struct _MotoInspector MotoInspector;
typedef struct _MotoInspectorClass MotoInspectorClass;
typedef struct _MotoInspectorPriv MotoInspectorPriv;

/* class MotoInspector */

struct _MotoInspector
{
    GtkVBox parent;
};

struct _MotoInspectorClass
{
    GtkVBoxClass parent;
};

GType moto_inspector_get_type(void);

#define MOTO_TYPE_INSPECTOR (moto_inspector_get_type())
#define MOTO_INSPECTOR(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_INSPECTOR, MotoInspector))
#define MOTO_INSPECTOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_INSPECTOR, MotoInspectorClass))
#define MOTO_IS_INSPECTOR(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_INSPECTOR))
#define MOTO_IS_INSPECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_INSPECTOR))
#define MOTO_INSPECTOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_INSPECTOR, MotoInspectorClass))

GtkWidget *moto_inspector_new(MotoTestWindow *window);

void moto_inspector_set_node(MotoInspector *self, MotoNode *node);

gboolean moto_inspector_has_prev_node(MotoInspector *self);
gboolean moto_inspector_has_next_node(MotoInspector *self);
void moto_inspector_goto_prev_node(MotoInspector *self);
void moto_inspector_goto_next_node(MotoInspector *self);
void moto_inspector_goto_first_node(MotoInspector *self);
void moto_inspector_goto_last_node(MotoInspector *self);

G_END_DECLS

#endif /* __MOTO_INSPECTOR_H__ */



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

/* WARNING! Simple param_editor only for testing. */

#ifndef __MOTO_PARAM_EDITOR_H__
#define __MOTO_PARAM_EDITOR_H__

#include <gtk/gtk.h>

#include "libmoto/moto-scene-node.h"
#include "moto-test-window.h"

G_BEGIN_DECLS

typedef struct _MotoParamEditor MotoParamEditor;
typedef struct _MotoParamEditorClass MotoParamEditorClass;
typedef struct _MotoParamEditorPriv MotoParamEditorPriv;

/* class MotoParamEditor */

struct _MotoParamEditor
{
    GtkVBox parent;
};

struct _MotoParamEditorClass
{
    GtkVBoxClass parent;
};

GType moto_param_editor_get_type(void);

#define MOTO_TYPE_PARAM_EDITOR (moto_param_editor_get_type())
#define MOTO_PARAM_EDITOR(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_EDITOR, MotoParamEditor))
#define MOTO_PARAM_EDITOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_EDITOR, MotoParamEditorClass))
#define MOTO_IS_PARAM_EDITOR(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_EDITOR))
#define MOTO_IS_PARAM_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_EDITOR))
#define MOTO_PARAM_EDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_EDITOR, MotoParamEditorClass))

GtkWidget *moto_param_editor_new(MotoTestWindow *window);

void moto_param_editor_set_node(MotoParamEditor *self, MotoNode *node);

gboolean moto_param_editor_has_prev_node(MotoParamEditor *self);
gboolean moto_param_editor_has_next_node(MotoParamEditor *self);
void moto_param_editor_goto_prev_node(MotoParamEditor *self);
void moto_param_editor_goto_next_node(MotoParamEditor *self);
void moto_param_editor_goto_first_node(MotoParamEditor *self);
void moto_param_editor_goto_last_node(MotoParamEditor *self);

G_END_DECLS

#endif /* __MOTO_PARAM_EDITOR_H__ */



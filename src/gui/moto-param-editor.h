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

#ifndef MOTO_PARAM_EDITOR_H
#define MOTO_PARAM_EDITOR_H

#include <gtk/gtk.h>

#include "motocore/moto-world.h"
#include "moto-test-window.h"

typedef struct _MotoParamEditor MotoParamEditor;
typedef struct _MotoParamEditorClass MotoParamEditorClass;
typedef struct _MotoParamEditorPriv MotoParamEditorPriv;

/* class MotoParamEditor */

struct _MotoParamEditor
{
    GtkTable parent;

    MotoParamEditorPriv *priv;
};

struct _MotoParamEditorClass
{
    GtkTableClass parent;
};

GType moto_param_editor_get_type(void);

#define MOTO_TYPE_PARAM_EDITOR (moto_param_editor_get_type())
#define MOTO_PARAM_EDITOR(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM_EDITOR, MotoParamEditor))
#define MOTO_PARAM_EDITOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM_EDITOR, MotoParamEditorClass))
#define MOTO_IS_PARAM_EDITOR(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM_EDITOR))
#define MOTO_IS_PARAM_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM_EDITOR))
#define MOTO_PARAM_EDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM_EDITOR, MotoParamEditorClass))

GtkWidget *moto_param_editor_new(MotoTestWindow *window);

void moto_param_editor_update(MotoParamEditor *self, MotoNode *node);

#endif /* MOTO_PARAM_EDITOR_H */



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

#ifndef MOTO_TOOL_BOX_H
#define MOTO_TOOL_BOX_H

#include <gtk/gtk.h>

#include "motocore/moto-forward.h"

typedef struct _MotoToolBox MotoToolBox;
typedef struct _MotoToolBoxClass MotoToolBoxClass;
typedef struct _MotoToolBoxPriv MotoToolBoxPriv;

/* class MotoToolBox */

struct _MotoToolBox
{
    GtkVBox parent;

    MotoToolBoxPriv *priv;
};

struct _MotoToolBoxClass
{
    GtkVBoxClass parent;
};

GType moto_tool_box_get_type(void);

#define MOTO_TYPE_TOOL_BOX (moto_tool_box_get_type())
#define MOTO_TOOL_BOX(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_TOOL_BOX, MotoToolBox))
#define MOTO_TOOL_BOX_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_TOOL_BOX, MotoToolBoxClass))
#define MOTO_IS_TOOL_BOX(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_TOOL_BOX))
#define MOTO_IS_TOOL_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_TOOL_BOX))
#define MOTO_TOOL_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_TOOL_BOX, MotoToolBoxClass))

GtkWidget *moto_tool_box_new(MotoSystem *system);
void moto_tool_box_set_system(MotoToolBox *self, MotoSystem *system);

#endif /* MOTO_TOOL_BOX_H */

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

#ifndef __MOTO_OUTLINER_H__
#define __MOTO_OUTLINER_H__

#include <gtk/gtk.h>

#include "libmoto/moto-world.h"
#include "moto-test-window.h"


G_BEGIN_DECLS

typedef struct _MotoOutliner MotoOutliner;
typedef struct _MotoOutlinerClass MotoOutlinerClass;

struct _MotoOutliner
{
    GtkVBox parent;
};

struct _MotoOutlinerClass
{
    GtkVBoxClass parent;
};

GType moto_outliner_get_type();

#define MOTO_TYPE_OUTLINER (moto_outliner_get_type())
#define MOTO_OUTLINER(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_OUTLINER, MotoOutliner))
#define MOTO_OUTLINER_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_OUTLINER, MotoOutlinerClass))
#define MOTO_IS_OUTLINER(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_OUTLINER))
#define MOTO_IS_OUTLINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_OUTLINER))
#define MOTO_OUTLINER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_OUTLINER, MotoOutlinerClass))

GtkWidget *moto_outliner_new(MotoTestWindow *window, MotoSceneNode *scene_node);

void moto_outliner_update(MotoOutliner *self);

G_END_DECLS

#endif /* __MOTO_OUTLINER_H__ */

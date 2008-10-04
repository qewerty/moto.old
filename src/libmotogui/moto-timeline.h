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

/* WARNING! Simple timeline only for testing. */

#ifndef MOTO_TIMELINE_H
#define MOTO_TIMELINE_H

#include <gtk/gtk.h>

#include "libmoto/moto-world.h"

typedef struct _MotoTimeline MotoTimeline;
typedef struct _MotoTimelineClass MotoTimelineClass;
typedef struct _MotoTimelinePriv MotoTimelinePriv;

/* class MotoTimeline */

struct _MotoTimeline
{
    GtkDrawingArea parent;

    MotoTimelinePriv *priv;
};

struct _MotoTimelineClass
{
    GtkDrawingAreaClass parent;
};

GType moto_timeline_get_type(void);

#define MOTO_TYPE_TIMELINE (moto_timeline_get_type())
#define MOTO_TIMELINE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_TIMELINE, MotoTimeline))
#define MOTO_TIMELINE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_TIMELINE, MotoTimelineClass))
#define MOTO_IS_TIMELINE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_TIMELINE))
#define MOTO_IS_TIMELINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_TIMELINE))
#define MOTO_TIMELINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_TIMELINE, MotoTimelineClass))

GtkWidget *moto_timeline_new(MotoWorld *world);

#endif /* MOTO_TIMELINE_H */



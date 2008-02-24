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

#ifndef MOTO_GEOMETRY_VIEW_NODE_H
#define MOTO_GEOMETRY_VIEW_NODE_H

#include "moto-geometry-node.h"

typedef struct _MotoGeometryViewNode MotoGeometryViewNode;
typedef struct _MotoGeometryViewNodeClass MotoGeometryViewNodeClass;
typedef struct _MotoGeometryViewNodePriv MotoGeometryViewNodePriv;

typedef void (*MotoGeometryViewNodeDrawMethod)(MotoGeometryViewNode *self);
typedef void (*MotoGeometryViewNodePrepareForDrawMethod)(MotoGeometryViewNode *self);

/* class MotoGeometryViewNode */

struct _MotoGeometryViewNode
{
    MotoNode parent;

    MotoGeometryViewNodePriv *priv;
};

struct _MotoGeometryViewNodeClass
{
    MotoNodeClass parent;

    MotoGeometryViewNodeDrawMethod draw;
    MotoGeometryViewNodePrepareForDrawMethod prepare_for_draw;

    /* signals */
    guint before_draw_signal_id;
    guint after_draw_signal_id;
    guint before_prepare_for_draw_signal_id;
    guint after_prepare_for_draw_signal_id;
};

GType moto_geometry_view_node_get_type(void);

#define MOTO_TYPE_GEOMETRY_VIEW_NODE (moto_geometry_view_node_get_type())
#define MOTO_GEOMETRY_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeometryViewNode))
#define MOTO_GEOMETRY_VIEW_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeometryViewNodeClass))
#define MOTO_IS_GEOMETRY_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOMETRY_VIEW_NODE))
#define MOTO_IS_GEOMETRY_VIEW_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOMETRY_VIEW_NODE))
#define MOTO_GEOMETRY_VIEW_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeometryViewNodeClass))

void moto_geometry_view_node_draw(MotoGeometryViewNode *self);
void moto_geometry_view_node_prepare_for_draw(MotoGeometryViewNode *self);

#endif /* MOTO_GEOMETRY_VIEW_NODE_H */

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

#ifndef MOTO_GEOMETRY_NODE_H
#define MOTO_GEOMETRY_NODE_H

#include "moto-node.h"
#include "moto-bound.h"

typedef struct _MotoGeometryNode MotoGeometryNode;
typedef struct _MotoGeometryNodeClass MotoGeometryNodeClass;

typedef MotoBound *(*MotoGeometryNodeGetBoundMethod)(MotoGeometryNode *self);

/* class MotoGeometryNode */

struct _MotoGeometryNode
{
    MotoNode parent;
};

struct _MotoGeometryNodeClass
{
    MotoNodeClass parent;

    MotoGeometryNodeGetBoundMethod get_bound;
};

GType moto_geometry_node_get_type(void);

#define MOTO_TYPE_GEOMETRY_NODE (moto_geometry_node_get_type())
#define MOTO_GEOMETRY_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOMETRY_NODE, MotoGeometryNode))
#define MOTO_GEOMETRY_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOMETRY_NODE, MotoGeometryNodeClass))
#define MOTO_IS_GEOMETRY_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOMETRY_NODE))
#define MOTO_IS_GEOMETRY_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOMETRY_NODE))
#define MOTO_GEOMETRY_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOMETRY_NODE, MotoGeometryNodeClass))

MotoBound *moto_geometry_node_get_bound(MotoGeometryNode *self);

#endif /* MOTO_GEOMETRY_NODE_H */

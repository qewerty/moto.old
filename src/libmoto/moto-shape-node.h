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

#ifndef __MOTO_SHAPE_NODE_H__
#define __MOTO_SHAPE_NODE_H__

#include "moto-node.h"
#include "moto-bound.h"

G_BEGIN_DECLS

typedef struct _MotoShapeNode MotoShapeNode;
typedef struct _MotoShapeNodeClass MotoShapeNodeClass;

typedef MotoBound *(*MotoShapeNodeGetBoundMethod)(MotoShapeNode *self);

/* class MotoShapeNode */

struct _MotoShapeNode
{
    MotoNode parent;
};

struct _MotoShapeNodeClass
{
    MotoNodeClass parent;

    MotoShapeNodeGetBoundMethod get_bound;
};

GType moto_shape_node_get_type(void);

#define MOTO_TYPE_SHAPE_NODE (moto_shape_node_get_type())
#define MOTO_SHAPE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SHAPE_NODE, MotoShapeNode))
#define MOTO_SHAPE_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SHAPE_NODE, MotoShapeNodeClass))
#define MOTO_IS_SHAPE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SHAPE_NODE))
#define MOTO_IS_SHAPE_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SHAPE_NODE))
#define MOTO_SHAPE_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SHAPE_NODE, MotoShapeNodeClass))

MotoBound *moto_shape_node_get_bound(MotoShapeNode *self);

G_END_DECLS

#endif /* __MOTO_SHAPE_NODE_H__ */

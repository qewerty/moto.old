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

#ifndef __MOTO_OP_NODE_H__
#define __MOTO_OP_NODE_H__

#include "moto-shape-node.h"
#include "moto-geom.h"
#include "moto-mesh.h"

G_BEGIN_DECLS

typedef struct _MotoOpNode MotoOpNode;
typedef struct _MotoOpNodeClass MotoOpNodeClass;

typedef MotoGeom* (*MotoOpPerformMethod)(MotoOpNode *self, MotoGeom *in, gboolean *the_same);

/* class MotoOpNode */

struct _MotoOpNode
{
    MotoShapeNode parent;
};

struct _MotoOpNodeClass
{
    MotoShapeNodeClass parent;

    MotoOpPerformMethod perform;
};

GType moto_op_node_get_type(void);

#define MOTO_TYPE_OP_NODE (moto_op_node_get_type())
#define MOTO_OP_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_OP_NODE, MotoOpNode))
#define MOTO_OP_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_OP_NODE, MotoOpNodeClass))
#define MOTO_IS_OP_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_OP_NODE))
#define MOTO_IS_OP_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_OP_NODE))
#define MOTO_OP_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_OP_NODE, MotoOpNodeClass))

void moto_op_node_set_selection(MotoOpNode *self,
    MotoMeshSelection *selection);
MotoMeshSelection *moto_op_node_get_selection(MotoOpNode *self);

MotoGeom *moto_op_node_perform(MotoOpNode *self, MotoGeom *in, gboolean *the_same);

G_END_DECLS

#endif /* __MOTO_OP_NODE_H__ */




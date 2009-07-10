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

#ifndef __MOTO_GEOM_OP_SKEL_NODE_H__
#define __MOTO_GEOM_OP_SKEL_NODE_H__

#include "moto-geom-op-node.h"

G_BEGIN_DECLS

typedef struct _MotoGeomOpSkelNode MotoGeomOpSkelNode;
typedef struct _MotoGeomOpSkelNodeClass MotoGeomOpSkelNodeClass;

/* class MotoGeomOpSkelNode */

struct _MotoGeomOpSkelNode
{
    MotoGeomOpNode parent;
};

struct _MotoGeomOpSkelNodeClass
{
    MotoGeomOpNodeClass parent;
};

GType moto_geom_op_skel_node_get_type(void);

#define MOTO_TYPE_GEOM_OP_SKEL_NODE (moto_geom_op_skel_node_get_type())
#define MOTO_GEOM_OP_SKEL_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOM_OP_SKEL_NODE, MotoGeomOpSkelNode))
#define MOTO_GEOM_OP_SKEL_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOM_OP_SKEL_NODE, MotoGeomOpSkelNodeClass))
#define MOTO_IS_GEOM_OP_SKEL_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOM_OP_SKEL_NODE))
#define MOTO_IS_GEOM_OP_SKEL_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOM_OP_SKEL_NODE))
#define MOTO_GEOM_OP_SKEL_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOM_OP_SKEL_NODE, MotoGeomOpSkelNodeClass))

MotoGeomOpSkelNode *moto_geom_op_skel_node_new(const gchar *name);

G_END_DECLS

#endif /* __MOTO_GEOM_OP_SKEL_NODE_H__ */



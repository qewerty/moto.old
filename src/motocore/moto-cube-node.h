
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

#ifndef MOTO_CUBE_NODE_H
#define MOTO_CUBE_NODE_H

#include "moto-geometry-node.h"

typedef struct _MotoCubeNode MotoCubeNode;
typedef struct _MotoCubeNodeClass MotoCubeNodeClass;
typedef struct _MotoCubeNodePriv MotoCubeNodePriv;

/* class MotoCubeNode */

struct _MotoCubeNode
{
    MotoGeometryNode parent;

    MotoCubeNodePriv *priv;
};

struct _MotoCubeNodeClass
{
    MotoGeometryNodeClass parent;
};

GType moto_cube_node_get_type(void);

#define MOTO_TYPE_CUBE_NODE (moto_cube_node_get_type())
#define MOTO_CUBE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_CUBE_NODE, MotoCubeNode))
#define MOTO_CUBE_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_CUBE_NODE, MotoCubeNodeClass))
#define MOTO_IS_CUBE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_CUBE_NODE))
#define MOTO_IS_CUBE_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_CUBE_NODE))
#define MOTO_CUBE_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_CUBE_NODE, MotoCubeNodeClass))

MotoCubeNode *moto_cube_node_new();

#endif /* MOTO_CUBE_NODE_H */

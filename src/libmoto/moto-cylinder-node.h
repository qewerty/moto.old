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

#ifndef __MOTO_CYLINDER_NODE_H__
#define __MOTO_CYLINDER_NODE_H__

#include "moto-geom-node.h"

G_BEGIN_DECLS

typedef struct _MotoCylinderNode MotoCylinderNode;
typedef struct _MotoCylinderNodeClass MotoCylinderNodeClass;
typedef struct _MotoCylinderNodePriv MotoCylinderNodePriv;

/* class MotoCylinderNode */

struct _MotoCylinderNode
{
    MotoGeometryNode parent;

    MotoCylinderNodePriv *priv;
};

struct _MotoCylinderNodeClass
{
    MotoGeometryNodeClass parent;
};

GType moto_cylinder_node_get_type(void);

#define MOTO_TYPE_CYLINDER_NODE (moto_cylinder_node_get_type())
#define MOTO_CYLINDER_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_CYLINDER_NODE, MotoCylinderNode))
#define MOTO_CYLINDER_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_CYLINDER_NODE, MotoCylinderNodeClass))
#define MOTO_IS_CYLINDER_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_CYLINDER_NODE))
#define MOTO_IS_CYLINDER_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_CYLINDER_NODE))
#define MOTO_CYLINDER_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_CYLINDER_NODE, MotoCylinderNodeClass))

MotoCylinderNode *moto_cylinder_node_new(const gchar *name);

G_END_DECLS

#endif /* __MOTO_CYLINDER_NODE_H__ */


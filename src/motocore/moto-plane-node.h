
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

#ifndef MOTO_PLANE_NODE_H
#define MOTO_PLANE_NODE_H

#include "moto-geometry-node.h"

typedef struct _MotoPlaneNode MotoPlaneNode;
typedef struct _MotoPlaneNodeClass MotoPlaneNodeClass;
typedef struct _MotoPlaneNodePriv MotoPlaneNodePriv;

typedef enum
{
    MOTO_ORIENTATION_XY,
    MOTO_ORIENTATION_YZ,
    MOTO_ORIENTATION_ZX,
} MotoOrientation;

GType moto_orientation_get_type(void);
#define MOTO_TYPE_ORIENTATION (moto_orientation_get_type())

/* class MotoPlaneNode */

struct _MotoPlaneNode
{
    MotoGeometryNode parent;

    MotoPlaneNodePriv *priv;
};

struct _MotoPlaneNodeClass
{
    MotoGeometryNodeClass parent;
};

GType moto_plane_node_get_type(void);

#define MOTO_TYPE_PLANE_NODE (moto_plane_node_get_type())
#define MOTO_PLANE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PLANE_NODE, MotoPlaneNode))
#define MOTO_PLANE_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PLANE_NODE, MotoPlaneNodeClass))
#define MOTO_IS_PLANE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PLANE_NODE))
#define MOTO_IS_PLANE_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PLANE_NODE))
#define MOTO_PLANE_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PLANE_NODE, MotoPlaneNodeClass))

MotoPlaneNode *moto_plane_node_new(const gchar *name);

#endif /* MOTO_PLANE_NODE_H */

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

#ifndef __MOTO_REMOVE_NODE_H__
#define __MOTO_REMOVE_NODE_H__

#include "moto-node.h"
#include "moto-mesh.h"
#include "moto-op-node.h"

G_BEGIN_DECLS

typedef struct _MotoRemoveNode MotoRemoveNode;
typedef struct _MotoRemoveNodeClass MotoRemoveNodeClass;

/* class MotoRemoveNode */

struct _MotoRemoveNode
{
    MotoOpNode parent;
};

struct _MotoRemoveNodeClass
{
    MotoOpNodeClass parent;
};

GType moto_remove_node_get_type(void);

#define MOTO_TYPE_REMOVE_NODE (moto_remove_node_get_type())
#define MOTO_REMOVE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_REMOVE_NODE, MotoRemoveNode))
#define MOTO_REMOVE_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_REMOVE_NODE, MotoRemoveNodeClass))
#define MOTO_IS_REMOVE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_REMOVE_NODE))
#define MOTO_IS_REMOVE_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_REMOVE_NODE))
#define MOTO_REMOVE_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_REMOVE_NODE, MotoRemoveNodeClass))

MotoRemoveNode *moto_remove_node_new(const gchar *name);

G_END_DECLS

#endif /* __MOTO_REMOVE_NODE_H__ */



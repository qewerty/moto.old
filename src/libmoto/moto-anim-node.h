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

#ifndef __MOTO_ANIM_NODE_H__
#define __MOTO_ANIM_NODE_H__

#include "moto-node.h"
#include "moto-bound.h"

G_BEGIN_DECLS

typedef struct _MotoAnimNode MotoAnimNode;
typedef struct _MotoAnimNodeClass MotoAnimNodeClass;

/* MotoAnimNode */

struct _MotoAnimNode
{
    MotoNode parent;
};

struct _MotoAnimNodeClass
{
    MotoNodeClass parent;
};

GType moto_anim_node_get_type(void);

#define MOTO_TYPE_ANIM_NODE (moto_anim_node_get_type())
#define MOTO_ANIM_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_ANIM_NODE, MotoAnimNode))
#define MOTO_ANIM_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_ANIM_NODE, MotoAnimNodeClass))
#define MOTO_IS_ANIM_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_ANIM_NODE))
#define MOTO_IS_ANIM_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_ANIM_NODE))
#define MOTO_ANIM_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_ANIM_NODE, MotoAnimNodeClass))

void moto_anim_node_foreach_animator_type(MotoAnimNode *self);

gint moto_anim_node_insert_animator(MotoAnimNode *self, gint index, gfloat width);
gint moto_anim_node_append_animator(MotoAnimNode *self, gfloat width);
gint moto_anim_node_prepend_animator(MotoAnimNode *self, gfloat width);
void moto_anim_node_delete_animator(MotoAnimNode *self, guint index);

G_END_DECLS

#endif /* __MOTO_ANIM_NODE_H__ */


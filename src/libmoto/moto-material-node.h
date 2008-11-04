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

#ifndef __MOTO_MATERIAL_NODE_H__
#define __MOTO_MATERIAL_NODE_H__

#include "moto-node.h"

G_BEGIN_DECLS

typedef struct _MotoMaterialNode MotoMaterialNode;
typedef struct _MotoMaterialNodeClass MotoMaterialNodeClass;

typedef void (*MotoMaterialNodeUseMethod)(MotoMaterialNode *self);

/* class MotoMaterialNode */

struct _MotoMaterialNode
{
    MotoNode parent;
};

struct _MotoMaterialNodeClass
{
    MotoNodeClass parent;

    MotoMaterialNodeUseMethod use;

    /* signals */

};

GType moto_material_node_get_type(void);

#define MOTO_TYPE_MATERIAL_NODE (moto_material_node_get_type())
#define MOTO_MATERIAL_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MATERIAL_NODE, MotoMaterialNode))
#define MOTO_MATERIAL_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MATERIAL_NODE, MotoMaterialNodeClass))
#define MOTO_IS_MATERIAL_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MATERIAL_NODE))
#define MOTO_IS_MATERIAL_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MATERIAL_NODE))
#define MOTO_MATERIAL_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MATERIAL_NODE, MotoMaterialNodeClass))

/* Enables the material in OpenGL viewport. */
void moto_material_node_use(MotoMaterialNode *self);

G_END_DECLS

#endif /* __MOTO_MATERIAL_NODE_H__ */

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

#ifndef MOTO_SLER_MATERIAL_NODE_H
#define MOTO_SLER_MATERIAL_NODE_H

#include "moto-material-node.h"

typedef struct _MotoSlerMaterialNode MotoSlerMaterialNode;
typedef struct _MotoSlerMaterialNodeClass MotoSlerMaterialNodeClass;
typedef struct _MotoSlerMaterialNodePriv MotoSlerMaterialNodePriv;

typedef struct _MotoSlerMaterialNodeFactory MotoSlerMaterialNodeFactory;
typedef struct _MotoSlerMaterialNodeFactoryClass MotoSlerMaterialNodeFactoryClass;
typedef struct _MotoSlerMaterialNodeFactoryPriv MotoSlerMaterialNodeFactoryPriv;

/* class MotoSlerMaterialNode */

struct _MotoSlerMaterialNode
{
    MotoMaterialNode parent;

    MotoSlerMaterialNodePriv *priv;
};

struct _MotoSlerMaterialNodeClass
{
    MotoMaterialNodeClass parent;

    /* signals */

};

GType moto_sler_material_node_get_type(void);

#define MOTO_TYPE_SLER_MATERIAL_NODE (moto_sler_material_node_get_type())
#define MOTO_SLER_MATERIAL_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SLER_MATERIAL_NODE, MotoSlerMaterialNode))
#define MOTO_SLER_MATERIAL_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SLER_MATERIAL_NODE, MotoSlerMaterialNodeClass))
#define MOTO_IS_SLER_MATERIAL_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SLER_MATERIAL_NODE))
#define MOTO_IS_SLER_MATERIAL_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SLER_MATERIAL_NODE))
#define MOTO_SLER_MATERIAL_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SLER_MATERIAL_NODE, MotoSlerMaterialNodeClass))

/* Enables the sler_material in OpenGL viewport. */
MotoSlerMaterialNode *moto_sler_material_node_new(const gchar *name);

/* class MotoNodeFactory */

struct _MotoSlerMaterialNodeFactory
{
    MotoNodeFactory parent;
};

struct _MotoSlerMaterialNodeFactoryClass
{
    MotoNodeFactoryClass parent;
};

GType moto_sler_material_node_factory_get_type(void);

#define MOTO_TYPE_SLER_MATERIAL_NODE_FACTORY (moto_sler_material_node_factory_get_type())
#define MOTO_SLER_MATERIAL_NODE_FACTORY(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SLER_MATERIAL_NODE_FACTORY, MotoSlerMaterialNodeFactory))
#define MOTO_SLER_MATERIAL_NODE_FACTORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SLER_MATERIAL_NODE_FACTORY, MotoSlerMaterialNodeFactoryClass))
#define MOTO_IS_SLER_MATERIAL_NODE_FACTORY(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SLER_MATERIAL_NODE_FACTORY))
#define MOTO_IS_SLER_MATERIAL_NODE_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SLER_MATERIAL_NODE_FACTORY))
#define MOTO_SLER_MATERIAL_NODE_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SLER_MATERIAL_NODE_FACTORY, MotoSlerMaterialNodeFactoryClass))

MotoNodeFactory *moto_sler_material_node_factory_new();

#endif /* MOTO_SLER_MATERIAL_NODE_H */

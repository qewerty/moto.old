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

#ifndef MOTO_MESH_VIEW_NODE_H
#define MOTO_MESH_VIEW_NODE_H

#include "moto-mesh.h"
#include "moto-geometry-view-node.h"

typedef struct _MotoMeshViewNode MotoMeshViewNode;
typedef struct _MotoMeshViewNodeClass MotoMeshViewNodeClass;
typedef struct _MotoMeshViewNodePriv MotoMeshViewNodePriv;

typedef struct _MotoMeshViewNodeFactory MotoMeshViewNodeFactory;
typedef struct _MotoMeshViewNodeFactoryClass MotoMeshViewNodeFactoryClass;
typedef struct _MotoMeshViewNodeFactoryPriv MotoMeshViewNodeFactoryPriv;

/* class MotoMeshViewNode */

struct _MotoMeshViewNode
{
    MotoGeometryViewNode parent;

    MotoMeshViewNodePriv *priv;
};

struct _MotoMeshViewNodeClass
{
    MotoGeometryViewNodeClass parent;
};

GType moto_mesh_view_node_get_type(void);

#define MOTO_TYPE_MESH_VIEW_NODE (moto_mesh_view_node_get_type())
#define MOTO_MESH_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MESH_VIEW_NODE, MotoMeshViewNode))
#define MOTO_MESH_VIEW_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MESH_VIEW_NODE, MotoMeshViewNodeClass))
#define MOTO_IS_MESH_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MESH_VIEW_NODE))
#define MOTO_IS_MESH_VIEW_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MESH_VIEW_NODE))
#define MOTO_MESH_VIEW_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MESH_VIEW_NODE, MotoMeshViewNodeClass))

MotoMeshViewNode *moto_mesh_view_node_new();

MotoMesh *moto_mesh_view_node_get_mesh(MotoMeshViewNode *self);
MotoMeshSelection *moto_mesh_view_node_get_selection(MotoMeshViewNode *self);

/* class MotoMeshViewNodeFactory */

struct _MotoMeshViewNodeFactory
{
    MotoNodeFactory parent;
};

struct _MotoMeshViewNodeFactoryClass
{
    MotoNodeFactoryClass parent;
};

GType moto_mesh_view_node_factory_get_type(void);

#define MOTO_TYPE_MESH_VIEW_NODE_FACTORY (moto_mesh_view_node_factory_get_type())
#define MOTO_MESH_VIEW_NODE_FACTORY(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MESH_VIEW_NODE_FACTORY, MotoMeshViewNodeFactory))
#define MOTO_MESH_VIEW_NODE_FACTORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MESH_VIEW_NODE_FACTORY, MotoMeshViewNodeFactoryClass))
#define MOTO_IS_MESH_VIEW_NODE_FACTORY(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MESH_VIEW_NODE_FACTORY))
#define MOTO_IS_MESH_VIEW_NODE_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MESH_VIEW_NODE_FACTORY))
#define MOTO_MESH_VIEW_NODE_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MESH_VIEW_NODE_FACTORY, MotoMeshViewNodeFactoryClass))

MotoNodeFactory *moto_mesh_view_node_factory_new();

#endif /* MOTO_MESH_VIEW_NODE_H */

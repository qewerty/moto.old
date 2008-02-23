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

#ifndef MOTO_MESH_FILE_NODE_H
#define MOTO_MESH_FILE_NODE_H

#include "moto-geometry-node.h"

typedef struct _MotoMeshFileNode MotoMeshFileNode;
typedef struct _MotoMeshFileNodeClass MotoMeshFileNodeClass;
typedef struct _MotoMeshFileNodePriv MotoMeshFileNodePriv;

typedef struct _MotoMeshFileNodeFactory MotoMeshFileNodeFactory;
typedef struct _MotoMeshFileNodeFactoryClass MotoMeshFileNodeFactoryClass;
typedef struct _MotoMeshFileNodeFactoryPriv MotoMeshFileNodeFactoryPriv;

/* class MotoMeshFileNode */

struct _MotoMeshFileNode
{
    MotoGeometryNode parent;

    MotoMeshFileNodePriv *priv;
};

struct _MotoMeshFileNodeClass
{
    MotoGeometryNodeClass parent;
};

GType moto_mesh_file_node_get_type(void);

#define MOTO_TYPE_MESH_FILE_NODE (moto_mesh_file_node_get_type())
#define MOTO_MESH_FILE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MESH_FILE_NODE, MotoMeshFileNode))
#define MOTO_MESH_FILE_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MESH_FILE_NODE, MotoMeshFileNodeClass))
#define MOTO_IS_MESH_FILE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MESH_FILE_NODE))
#define MOTO_IS_MESH_FILE_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MESH_FILE_NODE))
#define MOTO_MESH_FILE_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MESH_FILE_NODE, MotoMeshFileNodeClass))

MotoMeshFileNode *moto_mesh_file_node_new();

/* class MotoNodeFactory */

struct _MotoMeshFileNodeFactoryFactory
{
    MotoNodeFactory parent;
};

struct _MotoMeshFileNodeFactoryClass
{
    MotoNodeFactoryClass parent;
};

GType moto_mesh_file_node_factory_get_type(void);

#define MOTO_TYPE_OBJECT_NODE_FACTORY (moto_mesh_file_node_factory_get_type())
#define MOTO_OBJECT_NODE_FACTORY(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_OBJECT_NODE_FACTORY, MotoMeshFileNodeFactory))
#define MOTO_OBJECT_NODE_FACTORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_OBJECT_NODE_FACTORY, MotoMeshFileNodeFactoryClass))
#define MOTO_IS_OBJECT_NODE_FACTORY(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_OBJECT_NODE_FACTORY))
#define MOTO_IS_OBJECT_NODE_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_OBJECT_NODE_FACTORY))
#define MOTO_OBJECT_NODE_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_OBJECT_NODE_FACTORY, MotoMeshFileNodeFactoryClass))

MotoNodeFactory *moto_mesh_file_node_factory_new();

#endif /* MOTO_MESH_FILE_NODE_H */

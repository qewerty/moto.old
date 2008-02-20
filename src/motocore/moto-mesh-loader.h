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

#ifndef MOTO_MESH_LOADER_H
#define MOTO_MESH_LOADER_H

#include "moto-mesh.h"

typedef struct _MotoMeshLoader MotoMeshLoader;
typedef struct _MotoMeshLoaderClass MotoMeshLoaderClass;
typedef struct _MotoMeshLoaderPriv MotoMeshLoaderPriv;

typedef MotoMesh *(*MotoMeshLoaderLoadMethod)(MotoMeshLoader *self, const gchar *name);

/* class MotoMeshLoader */

struct _MotoMeshLoader
{
    GObject parent;

    MotoMeshLoaderPriv *priv;
};

struct _MotoMeshLoaderClass
{
    GObjectClass parent;

    MotoMeshLoaderLoadMethod load;
};

GType moto_mesh_loader_get_type(void);

#define MOTO_TYPE_MESH_LOADER (moto_mesh_loader_get_type())
#define MOTO_MESH_LOADER(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MESH_LOADER, MotoMeshLoader))
#define MOTO_MESH_LOADER_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MESH_LOADER, MotoMeshLoaderClass))
#define MOTO_IS_MESH_LOADER(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MESH_LOADER))
#define MOTO_IS_MESH_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MESH_LOADER))
#define MOTO_MESH_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MESH_LOADER, MotoMeshLoaderClass))

MotoMesh *moto_mesh_loader_load(MotoMeshLoader *self, const gchar *filename);

#endif /* MOTO_MESH_LOADER_H */

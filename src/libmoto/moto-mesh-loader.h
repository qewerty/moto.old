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

#ifndef __MOTO_MESH_LOADER_H__
#define __MOTO_MESH_LOADER_H__

#include "moto-mesh.h"

G_BEGIN_DECLS

typedef struct _MotoMeshLoader MotoMeshLoader;
typedef struct _MotoMeshLoaderClass MotoMeshLoaderClass;
typedef struct _MotoMeshLoaderPriv MotoMeshLoaderPriv;

typedef GSList *(*MotoMeshLoaderGetExtensionsMethod)(MotoMeshLoader *self);
typedef gboolean (*MotoMeshLoaderCanMethod)(MotoMeshLoader *self, const gchar *filename);
typedef MotoMesh *(*MotoMeshLoaderLoadMethod)(MotoMeshLoader *self, const gchar *filename);

/* class MotoMeshLoader */

struct _MotoMeshLoader
{
    GObject parent;
};

struct _MotoMeshLoaderClass
{
    GObjectClass parent;

    MotoMeshLoaderGetExtensionsMethod   get_extensions;
    MotoMeshLoaderCanMethod             can;
    MotoMeshLoaderLoadMethod            load;
};

GType moto_mesh_loader_get_type(void);

#define MOTO_TYPE_MESH_LOADER (moto_mesh_loader_get_type())
#define MOTO_MESH_LOADER(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MESH_LOADER, MotoMeshLoader))
#define MOTO_MESH_LOADER_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MESH_LOADER, MotoMeshLoaderClass))
#define MOTO_IS_MESH_LOADER(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MESH_LOADER))
#define MOTO_IS_MESH_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MESH_LOADER))
#define MOTO_MESH_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MESH_LOADER, MotoMeshLoaderClass))

GSList *moto_mesh_loader_get_extentions(MotoMeshLoader *self);
gboolean moto_mesh_loader_can(MotoMeshLoader *self, const gchar *filename);
MotoMesh *moto_mesh_loader_load(MotoMeshLoader *self, const gchar *filename);

G_END_DECLS

#endif /* __MOTO_MESH_LOADER_H__ */

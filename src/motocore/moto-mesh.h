
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

#ifndef MOTO_MESH_H
#define MOTO_MESH_H

#include "moto-mesh.h"

typedef struct _MotoMesh MotoMesh;
typedef struct _MotoMeshClass MotoMeshClass;

typedef struct _MotoMeshVertex MotoMeshVertex;
typedef struct _MotoMeshVertexAttr MotoMeshVertexAttr;

/* class MotoMesh */

struct _MotoMeshVertex
{
    gfloat xyz[3];
    gfloat no[3];
};

struct _MotoMeshVertexAttr
{
    GString *name;
    guint chnum;
    gfloat *data;
};

struct _MotoMesh
{
    GObject parent;

    guint verts_num;
    MotoMeshVertex *verts;

    GSList *verts_attrs;
};

struct _MotoMeshClass
{
    GObjectClass parent;
};

GType moto_mesh_get_type(void);

#define MOTO_TYPE_MESH (moto_mesh_get_type())
#define MOTO_MESH(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_MESH, MotoMesh))
#define MOTO_MESH_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_MESH, MotoMeshClass))
#define MOTO_IS_MESH(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_MESH))
#define MOTO_IS_MESH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_MESH))
#define MOTO_MESH_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_MESH, MotoMeshClass))

MotoMesh *moto_mesh_new(guint verts_num);

void moto_mesh_add_attr(MotoMesh *self, const gchar *attr_name, guint chnum);
MotoMeshVertexAttr *moto_mesh_get_attr(MotoMesh *self, const gchar *attr_name);

#endif /* MOTO_MESH_H */



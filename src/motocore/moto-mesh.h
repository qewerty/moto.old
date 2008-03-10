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

#include "glib-object.h"

typedef struct _MotoMesh MotoMesh;
typedef struct _MotoMeshClass MotoMeshClass;

typedef struct _MotoMeshVertex MotoMeshVertex;
typedef struct _MotoMeshEdge MotoMeshEdge;

typedef struct _MotoMeshFace MotoMeshFace;
typedef struct _MotoMeshSubFace MotoMeshSubFace;

typedef struct _MotoMeshVertexAttr MotoMeshVertexAttr;

typedef struct _MotoMeshSelection MotoMeshSelection;

typedef void (*MotoMeshForeachVertexFunc)(MotoMesh *mesh, MotoMeshVertex *vert);
typedef void (*MotoMeshForeachEdgeFunc)(MotoMesh *self, MotoMeshEdge *edge);
typedef void (*MotoMeshFaceForeachVertexFunc)(MotoMeshFace *self, MotoMeshVertex *vert);

/* class MotoMesh */

struct _MotoMeshVertex
{
    gfloat xyz[3];
    gfloat normal[3];
};

struct _MotoMeshEdge
{
    guint a, b;
};

struct _MotoMeshFace
{
    gfloat normal[3];
    guint verts_num;
    guint *indecies;

    /* If convexes is NULL face is not tesselated and considered as convex. */
    MotoMeshSubFace *convexes;
};

struct _MotoMeshSubFace
{
    guint verts_num;
    guint *indecies;
};

void moto_mesh_face_alloc(MotoMeshFace *self);
void moto_mesh_face_free(MotoMeshFace *self);
void moto_mesh_face_calc_normal(MotoMeshFace *self, MotoMesh *mesh);
/* Tesselation must be performed after normal is calculated.  */
void moto_mesh_face_tesselate(MotoMeshFace *self, MotoMesh *mesh);
// void moto_mesh_face_draw(MotoMeshFace *self);

void moto_mesh_face_foreach_vertex(MotoMeshFace *face,
        MotoMeshFaceForeachVertexFunc func, MotoMesh *mesh);

struct _MotoMeshVertexAttr
{
    GString *name;
    guint chnum;
    gfloat *data;
};

struct _MotoMeshSelection
{
    guint verts_num;
    guint32 *verts;
    guint edges_num;
    guint32 *edges;
    guint faces_num;
    guint32 *faces;
};

MotoMeshSelection *moto_mesh_selection_new(guint verts_num, guint edges_num, guint faces_num);
MotoMeshSelection *moto_mesh_selection_copy(MotoMeshSelection *other);
MotoMeshSelection *moto_mesh_selection_for_mesh(MotoMesh *mesh);
void moto_mesh_selection_free(MotoMeshSelection *self);

void moto_mesh_selection_select_vertex(MotoMeshSelection *self, guint index);
void moto_mesh_selection_deselect_vertex(MotoMeshSelection *self, guint index);
void moto_mesh_selection_deselect_all_verts(MotoMeshSelection *self);
void moto_mesh_selection_toggle_vertex_selection(MotoMeshSelection *self, guint index);
gboolean moto_mesh_selection_is_vertex_selected(MotoMeshSelection *self, guint index);

void moto_mesh_selection_select_edge(MotoMeshSelection *self, guint index);
void moto_mesh_selection_deselect_edge(MotoMeshSelection *self, guint index);
void moto_mesh_selection_deselect_all_edges(MotoMeshSelection *self);
void moto_mesh_selection_toggle_edge_selection(MotoMeshSelection *self, guint index);
gboolean moto_mesh_selection_is_edge_selected(MotoMeshSelection *self, guint index);

void moto_mesh_selection_select_face(MotoMeshSelection *self, guint index);
void moto_mesh_selection_deselect_face(MotoMeshSelection *self, guint index);
void moto_mesh_selection_deselect_all_faces(MotoMeshSelection *self);
void moto_mesh_selection_toggle_face_selection(MotoMeshSelection *self, guint index);
gboolean moto_mesh_selection_is_face_selected(MotoMeshSelection *self, guint index);

void moto_mesh_selection_deselect_all(MotoMeshSelection *self);

gboolean moto_mesh_selection_is_valid(MotoMeshSelection *self, MotoMesh *mesh);

struct _MotoMesh
{
    GObject parent;

    guint verts_num;
    MotoMeshVertex *verts;

    guint edges_num;
    MotoMeshEdge *edges;

    guint faces_num;
    MotoMeshFace *faces;

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

MotoMesh *moto_mesh_new(guint verts_num, guint edges_num, guint faces_num);
MotoMesh *moto_mesh_copy(MotoMesh *other);

MotoMeshVertexAttr * moto_mesh_add_attr(MotoMesh *self, const gchar *attr_name, guint chnum);
MotoMeshVertexAttr *moto_mesh_get_attr(MotoMesh *self, const gchar *attr_name);

void moto_mesh_foreach_vertex(MotoMesh *self,
        MotoMeshForeachVertexFunc func);
void moto_mesh_foreach_edge(MotoMesh *self,
        MotoMeshForeachEdgeFunc func);

#endif /* MOTO_MESH_H */



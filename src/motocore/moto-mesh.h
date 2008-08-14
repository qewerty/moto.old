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

#include <GL/gl.h>
#include <glib-object.h>

#include "moto-ray.h"

typedef struct _MotoMesh MotoMesh;
typedef struct _MotoMeshClass MotoMeshClass;

typedef struct _MotoMeshTriplet MotoMeshTriplet;

typedef struct _MotoMeshVert16 MotoMeshVert16;
typedef struct _MotoMeshVert32 MotoMeshVert32;
typedef struct _MotoMeshEdge16 MotoMeshEdge16;
typedef struct _MotoMeshEdge32 MotoMeshEdge32;
typedef struct _MotoMeshFace16 MotoMeshFace16;
typedef struct _MotoMeshFace32 MotoMeshFace32;

typedef struct _MotoHalfEdge16 MotoHalfEdge16;
typedef struct _MotoHalfEdge32 MotoHalfEdge32;

typedef struct _MotoMeshFaceHole MotoMeshFaceHole;
typedef struct _MotoTriangle MotoTriangle;

typedef struct _MotoMeshVertAttr MotoMeshVertAttr;

typedef struct _MotoMeshSelection MotoMeshSelection;

typedef void (*MotoMeshForeachVertexFunc)(MotoMesh *mesh,
        gpointer vert, gpointer user_data);
typedef void (*MotoMeshForeachEdgeFunc)(MotoMesh *self,
        gpointer edge, gpointer user_data);
typedef void (*MotoMeshForeachFaceFunc)(MotoMesh *self,
        gpointer face, gpointer user_data);
/*
typedef void (*MotoMeshFaceForeachVertexFunc)(MotoMeshFace *self,
        gpointer vert, gpointer user_data);
*/

typedef struct _MotoMeshOp MotoMeshOp;

/* class MotoMesh */

struct _MotoMeshVert16
{
    guint16 half_edge;
};

struct _MotoMeshVert32
{
    guint32 half_edge;
};

// WARNING: Must be convertable to float*!
struct _MotoMeshTriplet
{
    gfloat x, y, z;
};

struct _MotoMeshEdge16
{
    guint16 half_edge;
};

struct _MotoMeshEdge32
{
    guint32 half_edge;
};

/*
struct _MotoMeshFaceHole
{
    guint v_num;
    guint *indecies;
    union
    {
        guint16 *v16;
        guint32 *v32;
        guint64 *v64;
    };
};
*/

void moto_mesh_face_hole_init(MotoMeshFaceHole *self, guint v_num);

struct _MotoTriangle16
{
    guint16 a, b, c; // 48bit
};

struct _MotoTriangle32
{
    guint32 a, b, c; // 96bit
};

struct _MotoMeshFace16
{
    guint v_num;
    guint16 half_edge;
};

struct _MotoMeshFace32
{
    guint v_num;
    guint32 half_edge;
};

struct _MotoMeshVertAttr
{
    guint chnum;
    gfloat *data;
};

struct _MotoMeshSelection
{
    guint v_num;
    guint32 *verts;
    guint e_num;
    guint32 *edges;
    guint f_num;
    guint32 *faces;

    guint32 selected_v_num;
    guint32 selected_e_num;
    guint32 selected_f_num;
};

MotoMeshSelection *moto_mesh_selection_new(guint v_num, guint e_num, guint f_num);
MotoMeshSelection *moto_mesh_selection_copy(MotoMeshSelection *other);
void moto_mesh_selection_copy_smth(MotoMeshSelection *self, MotoMeshSelection *other);
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

struct _MotoHalfEdge16
{
    guint16 next;
    // guint16 prev;
    guint16 pair;

    guint16 v_origin;
    guint16 f_left;
    guint16 edge;
};

struct _MotoHalfEdge32
{
    guint32 next;
    // guint32 prev;
    guint32 pair;

    guint32 v_origin;
    guint32 f_left;
    guint32 edge;
};

#define moto_half_edge_pair(self, mesh, t) (((t)mesh->he_data)[self->pair])
#define moto_half_edge_next(self, mesh) (mesh->he_data[self->next])
#define moto_half_edge_prev(self, mesh) (mesh->he_data[self->prev])
#define moto_half_edge_v_orig(self, mesh) (mesh->v_data[self->v_origin])
#define moto_half_edge_f_left(self, mesh) (mesh->f_data[self->f_left])
#define moto_half_edge_edge(self, mesh) (mesh->v_data[self->edge])

#define moto_half_edge_v_dest(self, mesh) \
    (moto_half_edge_origin(& moto_half_edge_pair(self, mesh)), mesh)

#define moto_half_edge_v_next ...
#define moto_half_edge_v_prev ...

struct _MotoMesh
{
    GObject parent;

    /* WARNING! All members are private but opened for performance reasons.
     *          Don't use them directly! */

    gboolean b32; // 16bit or 32bit are used for indecies
    GLenum index_gl_type;

    // Verts
    guint v_num;
    gpointer v_data;
    MotoMeshTriplet *v_coords;
    MotoMeshTriplet *v_normals;
    GData *v_attrs;

    // Edges
    guint e_num;
    gpointer e_data;
    gpointer e_verts;
    guint32 *e_hard_flags;

    gboolean e_use_creases;
    gfloat *e_creases;

    // Faces
    guint f_num;
    guint f_tess_num;
    gpointer f_data;
    gpointer f_verts;
    gpointer f_tess_verts;
    MotoMeshTriplet *f_normals;
    gboolean f_use_hidden;
    guint32 *f_hidden_flags;

    // Half-edge data
    gpointer he_data;
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

MotoMesh *moto_mesh_new(guint v_num, guint e_num, guint f_num, guint f_verts_num);
MotoMesh *moto_mesh_copy(MotoMesh *other);

#define moto_mesh_get_index_size(mesh) (((mesh)->b32)?4:2)

#define moto_mesh_max_valid_index(mesh) (((mesh)->b32)?G_MAXUINT32-1:G_MAXUINT16-1)
#define moto_mesh_is_index_valid(mesh, index) (index <= moto_mesh_max_valid_index(mesh))
#define moto_mesh_invalid_index(mesh) (((mesh)->b32)?G_MAXUINT32:G_MAXUINT16)

void moto_mesh_calc_faces_normals(MotoMesh *self);
// WARNING: Call only when he_data are set correctly!
void moto_mesh_calc_verts_normals(MotoMesh *self);
void moto_mesh_calc_normals(MotoMesh *self);

MotoMeshVertAttr * moto_mesh_add_attr(MotoMesh *self, const gchar *attr_name, guint chnum);
MotoMeshVertAttr *moto_mesh_get_attr(MotoMesh *self, const gchar *attr_name);

void moto_mesh_foreach_vertex(MotoMesh *self,
        MotoMeshForeachVertexFunc func, gpointer user_data);
void moto_mesh_foreach_edge(MotoMesh *self,
        MotoMeshForeachEdgeFunc func, gpointer user_data);
void moto_mesh_foreach_face(MotoMesh *self,
        MotoMeshForeachFaceFunc func, gpointer user_data);

void moto_mesh_foreach_selected_vertex(MotoMesh *self,
        MotoMeshSelection *selection, MotoMeshForeachVertexFunc func, gpointer user_data);
void moto_mesh_foreach_selected_edge(MotoMesh *self,
        MotoMeshSelection *selection, MotoMeshForeachEdgeFunc func, gpointer user_data);
void moto_mesh_foreach_selected_face(MotoMesh *self,
        MotoMeshSelection *selection, MotoMeshForeachFaceFunc func, gpointer user_data);

void moto_mesh_tesselate_faces(MotoMesh *self);

void moto_mesh_set_edge_hard(MotoMesh *self, guint32 index);
void moto_mesh_set_edge_soft(MotoMesh *self, guint32 index);
void moto_mesh_is_edge_hard(MotoMesh *self, guint32 index);

void moto_mesh_hide_face(MotoMesh *self, guint32 index);
void moto_mesh_show_face(MotoMesh *self, guint32 index);
void moto_mesh_is_face_hidden(MotoMesh *self, guint32 index);

void moto_mesh_update_he_data(MotoMesh *self);

void moto_mesh_prepare(MotoMesh *self);

void moto_mesh_grow_vert_selection(MotoMesh *self, MotoMeshSelection *selection);
void moto_mesh_select_less_verts(MotoMesh *self, MotoMeshSelection *selection);
void moto_mesh_grow_edge_selection(MotoMesh *self, MotoMeshSelection *selection);
void moto_mesh_grow_face_selection(MotoMesh *self, MotoMeshSelection *selection);

#endif /* MOTO_MESH_H */



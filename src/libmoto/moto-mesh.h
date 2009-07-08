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

#ifndef __MOTO_MESH_H__
#define __MOTO_MESH_H__

#include <GL/glew.h>
#include <glib-object.h>

#include "moto-ray.h"
#include "moto-bitmask.h"
#include "moto-bound.h"

G_BEGIN_DECLS

typedef struct _MotoMesh MotoMesh;
typedef struct _MotoMeshClass MotoMeshClass;

typedef struct _MotoVector MotoVector;

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
struct _MotoVector
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

struct _MotoMeshFace16
{
    guint16 v_offset;
    guint16 half_edge;
};

struct _MotoMeshFace32
{
    guint32 v_offset;
    guint32 half_edge;
};

struct _MotoMeshVertAttr
{
    guint chnum;
    gfloat *data;
};

struct _MotoMeshSelection
{
    MotoBitmask *verts;
    MotoBitmask *edges;
    MotoBitmask *faces;
};

MotoMeshSelection *moto_mesh_selection_new(guint v_num, guint e_num, guint f_num);
MotoMeshSelection *moto_mesh_selection_copy(MotoMeshSelection *other);
void moto_mesh_selection_copy_smth(MotoMeshSelection *self, MotoMeshSelection *other);
MotoMeshSelection *moto_mesh_selection_new_for_mesh(MotoMesh *mesh);
void moto_mesh_selection_free(MotoMeshSelection *self);

guint32 moto_mesh_selection_get_v_num(MotoMeshSelection *self);
guint32 moto_mesh_selection_get_e_num(MotoMeshSelection *self);
guint32 moto_mesh_selection_get_f_num(MotoMeshSelection *self);

guint32 moto_mesh_selection_get_selected_v_num(MotoMeshSelection *self);
guint32 moto_mesh_selection_get_selected_e_num(MotoMeshSelection *self);
guint32 moto_mesh_selection_get_selected_f_num(MotoMeshSelection *self);

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

void moto_mesh_selection_select_more_verts(MotoMeshSelection *self, MotoMesh *mesh);
void moto_mesh_selection_select_less_verts(MotoMeshSelection *self, MotoMesh*mesh);
void moto_mesh_selection_select_inverse_verts(MotoMeshSelection *self, MotoMesh*mesh);

void moto_mesh_selection_select_more_edges(MotoMeshSelection *self, MotoMesh*mesh);
void moto_mesh_selection_select_less_edges(MotoMeshSelection *self, MotoMesh*mesh);
void moto_mesh_selection_select_inverse_edges(MotoMeshSelection *self, MotoMesh*mesh);

void moto_mesh_selection_select_more_faces(MotoMeshSelection *self, MotoMesh *mesh);
void moto_mesh_selection_select_less_faces(MotoMeshSelection *self, MotoMesh *mesh);
void moto_mesh_selection_select_inverse_faces(MotoMeshSelection *self, MotoMesh *mesh);

struct _MotoHalfEdge16
{
    guint16 next;
    guint16 prev;

    guint16 f_left;
};

struct _MotoHalfEdge32
{
    guint32 next;
    guint32 prev;

    guint32 f_left;
};

#define moto_half_edge_pair(he) ((he%2) ? (he)-1 : (he)+1)
#define moto_half_edge_edge(he) ((he%2) ? (he-1)/2 : (he)/2)

#define moto_half_edge_v_dest(self, mesh) \
    (moto_half_edge_origin(& moto_half_edge_pair(self, mesh)), mesh)

#define moto_half_edge_v_next ...
#define moto_half_edge_v_prev ...

struct _MotoMesh
{
    /* WARNING! All members are private but opened for performance reasons.
     *          Don't use them directly! */

    GObject parent;

    gboolean b32; // 16bit or 32bit are used for indecies
    GLenum index_gl_type;

    // Verts
    guint v_num;
    gpointer v_data;
    MotoVector *v_coords;
    MotoVector *v_normals;
    GData *v_attrs;

    // Edges
    guint e_num;
    gpointer e_data; // FIXME: Remove?
    gpointer e_verts;
    guint32 *e_hard_flags;

    gboolean e_use_creases;
    gfloat *e_creases;

    // Faces
    guint f_num;
    guint f_v_num;
    gpointer f_data;
    gpointer f_verts;

    guint f_tess_num;
    gboolean tesselated;
    gpointer f_tess_verts;

    gboolean f_use_normals;
    MotoVector *f_normals;

    gboolean f_use_hidden;
    guint32 *f_hidden_flags;

    // Half-edge data
    gboolean he_calculated;
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
MotoMesh *moto_mesh_new_copy(MotoMesh *other);
void moto_mesh_copy(MotoMesh *self, MotoMesh *other);

#define MOTO_DECLARE_MESH_DATA_16(mesh) \
    MotoMeshVert16 *v_data  = (MotoMeshVert16*)mesh->v_data; \
    MotoMeshEdge16 *e_data  = (MotoMeshFace16*)mesh->e_data; \
    MotoMeshFace16 *f_data  = (MotoMeshFace16*)mesh->f_data; \
    guint16 *f_verts        = (guint16*)mesh->f_verts; \
    guint16 *f_tess_verts   = (guint16*)mesh->f_tess_verts; \
    guint16 *e_verts        = (guint16*)mesh->e_verts; \
    MotoHalfEdge16 *he_data = (MotoHalfEdge16*)mesh->he_data

#define MOTO_DECLARE_MESH_DATA_32(mesh) \
    MotoMeshVert32 *v_data  = (MotoMeshVert32*)mesh->v_data; \
    MotoMeshEdge32 *e_data  = (MotoMeshFace32*)mesh->e_data; \
    MotoMeshFace32 *f_data  = (MotoMeshFace32*)mesh->f_data; \
    guint32 *f_verts        = (guint32*)mesh->f_verts; \
    guint32 *f_tess_verts   = (guint32*)mesh->f_tess_verts; \
    guint32 *e_verts        = (guint32*)mesh->e_verts; \
    MotoHalfEdge32 *he_data = (MotoHalfEdge32*)mesh->he_data

/**
 * moto_mesh_clear_e_data:
 * @self: a #MotoMesh.
 *
 * Clears all edge data to invalid values.
 */
void moto_mesh_clear_e_data(MotoMesh *self);

/**
 * moto_mesh_clear_he_data:
 * @self: a #MotoMesh.
 *
 * Clears all half-edge data to invalid values.
 */
void moto_mesh_clear_he_data(MotoMesh *self);

#define moto_mesh_get_index_size(mesh) (((mesh)->b32)?4:2)

#define moto_mesh_max_valid_index(mesh) (((mesh)->b32)?G_MAXUINT32-1:G_MAXUINT16-1)
#define moto_mesh_is_index_valid(mesh, index) (index <= moto_mesh_max_valid_index(mesh))
#define moto_mesh_invalid_index(mesh) (((mesh)->b32)?G_MAXUINT32:G_MAXUINT16)

#define moto_mesh_get_f_v_num(mesh) (mesh->f_v_num)

gboolean moto_mesh_has_edge(MotoMesh *self, guint32 v1, guint32 v2);

void moto_mesh_calc_faces_normals(MotoMesh *self);
void moto_mesh_calc_verts_normals(MotoMesh *self);
void moto_mesh_calc_normals(MotoMesh *self);

gboolean moto_mesh_set_face(MotoMesh *self, guint32 fi, guint32 v_offset, guint32 *f_verts);

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

guint moto_mesh_get_face_v_num(MotoMesh *self, guint fi);

gboolean moto_mesh_update_he_data(MotoMesh *self);

gboolean moto_mesh_prepare(MotoMesh *self);

gboolean moto_mesh_intersect_face(MotoMesh *self, guint fi, MotoRay *ray, gfloat *dist);

void moto_mesh_calc_bound(MotoMesh* self, MotoBound* bound);
MotoBound* moto_mesh_create_bound(MotoMesh* self);

MotoMesh* moto_mesh_extrude_faces(MotoMesh *self,
    MotoMeshSelection *selection, guint sections,
    gfloat length);

MotoMesh* moto_mesh_extrude_region(MotoMesh *self,
    MotoMeshSelection *selection, guint sections,
    gfloat tx, gfloat ty, gfloat tz,
    gfloat rx, gfloat ry, gfloat rz,
    gfloat sx, gfloat sy, gfloat sz);

G_END_DECLS

#endif /* __MOTO_MESH_H__ */



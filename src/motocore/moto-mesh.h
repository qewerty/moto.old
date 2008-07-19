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

#include "moto-ray.h"

typedef struct _MotoMesh MotoMesh;
typedef struct _MotoMeshClass MotoMeshClass;
typedef struct _MotoMeshPriv MotoMeshPriv;

typedef struct _MotoMeshVert MotoMeshVert;
typedef struct _MotoMeshEdge MotoMeshEdge;

typedef struct _MotoMeshFace MotoMeshFace;
typedef struct _MotoMeshFaceHole MotoMeshFaceHole;
typedef struct _MotoTriangle MotoTriangle;
typedef struct _MotoMeshSubFace MotoMeshSubFace;

typedef struct _MotoMeshVertAttr MotoMeshVertAttr;

typedef struct _MotoMeshSelection MotoMeshSelection;

typedef void (*MotoMeshForeachVertexFunc)(MotoMesh *mesh, MotoMeshVert *vert);
typedef void (*MotoMeshForeachEdgeFunc)(MotoMesh *self, MotoMeshEdge *edge);
typedef void (*MotoMeshFaceForeachVertexFunc)(MotoMeshFace *self, MotoMeshVert *vert);

typedef struct _MotoMeshOp MotoMeshOp;

/* class MotoMesh */

struct _MotoMeshVert
{
    gfloat xyz[3];
    gfloat normal[3];
};

struct _MotoMeshEdge
{
    guint a, b;
};

struct _MotoMeshFaceHole
{
    guint verts_num;
    guint *indecies;
    union
    {
        guint16 *v16;
        guint32 *v32;
        guint64 *v64;
    };
};

void moto_mesh_face_hole_init(MotoMeshFaceHole *self, guint verts_num);

struct _MotoTriangle16
{
    guint16 a, b, c; // 96bit
};

struct _MotoTriangle32
{
    guint32 a, b, c; // 96bit
};

struct _MotoMeshFace
{
    gfloat normal[3];
    guint verts_num;
    guint *indecies;
    union
    {
        /* Small meshes use small indecies. ;) */
        guint16 *v16;
        guint32 *v32;
        guint64 *v64; /* Very big meshes. 0_o */
    };

    guint holes_num;
    MotoMeshFaceHole *holes;

    /* read-only */
    guint triangles_num;
    /*
    union
    {
        MotoTriangle16  *tri16;
        MotoTriangle132 *tri32;
    }
    */
};

struct _MotoMeshSubFace
{
    guint verts_num;
    guint *indecies;
};

void moto_mesh_face_init(MotoMeshFace *self, guint verts_num, guint holes_num);
void moto_mesh_face_alloc(MotoMeshFace *self); // TODO: Remove deprecated!
void moto_mesh_face_free(MotoMeshFace *self);
void moto_mesh_face_calc_normal(MotoMeshFace *self, MotoMesh *mesh);
/* Tesselation must be performed after normal is calculated.  */
void moto_mesh_face_tesselate(MotoMeshFace *self, MotoMesh *mesh);
// void moto_mesh_face_draw(MotoMeshFace *self);
gboolean moto_mesh_face_intersect_ray(MotoMeshFace *self,  MotoMesh *mesh, MotoRay *ray, gfloat *dist);

void moto_mesh_face_foreach_vertex(MotoMeshFace *face,
        MotoMeshFaceForeachVertexFunc func, MotoMesh *mesh);

struct _MotoMeshVertAttr
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
    MotoMeshVert *verts;

    guint edges_num;
    MotoMeshEdge *edges;
    guint32 *hard_edge_flags;

    guint faces_num;
    MotoMeshFace *faces;

    GSList *verts_attrs;

    MotoMeshPriv *priv;
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

#define moto_mesh_get_index_size(mesh) (((mesh)->verts_num <= G_MAXUINT16)?1:2)

MotoMeshVertAttr * moto_mesh_add_attr(MotoMesh *self, const gchar *attr_name, guint chnum);
MotoMeshVertAttr *moto_mesh_get_attr(MotoMesh *self, const gchar *attr_name);

void moto_mesh_foreach_vertex(MotoMesh *self,
        MotoMeshForeachVertexFunc func);
void moto_mesh_foreach_edge(MotoMesh *self,
        MotoMeshForeachEdgeFunc func);
// void moto_mesh_foreach_face(MotoMesh *self,
//        MotoMeshForeachFaceFunc func);

void moto_mesh_tesselate_faces(MotoMesh *self);

/* Euler operators.
 * Adpated from BMesh (http://wiki.blender.org/index.php/BlenderDev/Bmesh).
 * A mesh object remembers information about how many new verts/edges/faces must be created or deleted
 * while perforforming any of these operators. You do not need think about memory for mesh components. */

struct _MotoMeshOp
{
    GObject parent;
};

/**
 * moto_mesh_op_empty:
 *
 * Creates new mesh operator which do nothing.
 */
MotoMeshOp *moto_mesh_op_empty();

/**
 * moto_mesh_op_combine:
 * @ops: Array of pointers to mesh operators.
 *
 * Creates new mesh operator which perform sequence of other operators in order given in array passed though ops argument.
 *
 * Returns: instance of newly created MotoMeshOp.
 */
MotoMeshOp *moto_mesh_op_combine(MotoMeshOp *ops);

MotoMesh *moto_mesh_op_perform(MotoMeshOp *self, MotoMesh *mesh, MotoMeshSelection *selection);
gboolean moto_mesh_op_check(MotoMeshOp *self, MotoMesh *mesh);

/* make face & kill face */

/**
 * moto_mesh_MF:
 * @indecies: The pointer to array of vertex indecies which will be verts of new face.
 *
 * Creates new face from given vertex indecies.
 *
 * Returns: MotoMeshFace instance or NULL if indecies are invalid.
 */
MotoMeshOp *moto_mesh_op_MF(MotoMesh *self, guint *indecies);

/**
 * moto_mesh_KF:
 * @face_index: The index of the face to delete.
 *
 * Delete the face by given index.
 *
 * Returns: TRUE on succeed or FALSE of operation can not be performed.
 */
gboolean moto_mesh_KF(MotoMesh *self, guint face_index);

/* make edge & kill edge */
MotoMeshEdge *moto_mesh_ME(MotoMesh *self, guint v1, guint v2);
gboolean moto_mesh_KE(MotoMesh *self, MotoMeshEdge *e);

/* make vert & kill vert */
MotoMeshVert *moto_mesh_MV(MotoMesh *self, MotoMeshEdge *e);
gboolean moto_mesh_KV(MotoMesh *self, MotoMeshVert *v);

/* slpit edge, make vert & join edge, kill vert */
MotoMeshVert *moto_mesh_SEMV(MotoMesh *self, MotoMeshEdge *e);
gboolean moto_mesh_JEKV(MotoMesh *self, MotoMeshEdge *e);

/* slpit face, make edge & join face, kill edge */
MotoMeshEdge *moto_mesh_SFME(MotoMesh *self,
        MotoMeshFace *f, MotoMeshVert *v1, MotoMeshVert *v2);
gboolean moto_mesh_JFKE(MotoMesh *self,
        MotoMeshFace *f1, MotoMeshFace *f2);

/* Perform previous operators.  */
void moto_mesh_perform_operators(MotoMesh *self);

/* Check has the mesh unperformed euler operations or not.  */
gboolean moto_mesh_has_operators(MotoMesh *self);

/* Clear sequence of mesh operators. */
void moto_mesh_clear_operators(MotoMesh *self);

#endif /* MOTO_MESH_H */



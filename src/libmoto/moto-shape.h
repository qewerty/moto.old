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

#ifndef __MOTO_SHAPE_H__
#define __MOTO_SHAPE_H__

#include <glib-object.h>
#include "moto-enums.h"
#include "moto-bitmask.h"

G_BEGIN_DECLS

typedef struct _MotoShape MotoShape;
typedef struct _MotoShapeClass MotoShapeClass;

typedef struct _MotoShapeSelection MotoShapeSelection;

typedef gboolean (*MotoShapePrepareMethod)(MotoShape *self);
typedef gboolean (*MotoShapeIsStructTheSameMethod)(MotoShape *self, MotoShape *other);

typedef void (*MotoShapeSelectMoreMethod)(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
typedef void (*MotoShapeSelectLessMethod)(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
typedef void (*MotoShapeSelectInverseMethod)(MotoShape* self,
        MotoShapeSelection* selection, MotoSelectionMode mode);

/* class MotoShape */

struct _MotoShape
{
    GInitiallyUnowned parent;
};

struct _MotoShapeClass
{
    GInitiallyUnownedClass parent;

    MotoShapePrepareMethod prepare;
    MotoShapeIsStructTheSameMethod is_struct_the_same;
    MotoShapeSelectMoreMethod select_more;
    MotoShapeSelectLessMethod select_less;
    MotoShapeSelectInverseMethod select_inverse;
};

GType moto_shape_get_type(void);

#define MOTO_TYPE_SHAPE (moto_shape_get_type())
#define MOTO_SHAPE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SHAPE, MotoShape))
#define MOTO_SHAPE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SHAPE_, MotoShapeClass))
#define MOTO_IS_SHAPE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SHAPE))
#define MOTO_IS_SHAPE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SHAPE))
#define MOTO_SHAPE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SHAPE, MotoShapeClass))

void moto_shape_update(MotoShape *self);

gboolean moto_shape_prepare(MotoShape *self);
gboolean moto_shape_is_struct_the_same(MotoShape *self, MotoShape *other);

void moto_shape_select_more(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
void moto_shape_select_less(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
void moto_shape_select_inverse(MotoShape* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);

struct _MotoShapeSelection
{
    MotoBitmask *verts;
    MotoBitmask *edges;
    MotoBitmask *faces;
};

MotoShapeSelection *moto_shape_selection_new(guint v_num, guint e_num, guint f_num);
MotoShapeSelection *moto_shape_selection_copy(MotoShapeSelection *other);
void moto_shape_selection_copy_smth(MotoShapeSelection *self, MotoShapeSelection *other);
void moto_shape_selection_free(MotoShapeSelection *self);

guint32 moto_shape_selection_get_v_num(MotoShapeSelection *self);
guint32 moto_shape_selection_get_e_num(MotoShapeSelection *self);
guint32 moto_shape_selection_get_f_num(MotoShapeSelection *self);

guint32 moto_shape_selection_get_selected_v_num(MotoShapeSelection *self);
guint32 moto_shape_selection_get_selected_e_num(MotoShapeSelection *self);
guint32 moto_shape_selection_get_selected_f_num(MotoShapeSelection *self);

void moto_shape_selection_select_vertex(MotoShapeSelection *self, guint index);
void moto_shape_selection_select_verts(MotoShapeSelection *self, ...);
void moto_shape_selection_deselect_vertex(MotoShapeSelection *self, guint index);
void moto_shape_selection_deselect_verts(MotoShapeSelection *self, ...);
void moto_shape_selection_deselect_all_verts(MotoShapeSelection *self);
void moto_shape_selection_toggle_vertex_selection(MotoShapeSelection *self, guint index);
gboolean moto_shape_selection_is_vertex_selected(MotoShapeSelection *self, guint index);

void moto_shape_selection_select_edge(MotoShapeSelection *self, guint index);
void moto_shape_selection_select_edges(MotoShapeSelection *self, ...);
void moto_shape_selection_deselect_edge(MotoShapeSelection *self, guint index);
void moto_shape_selection_deselect_edges(MotoShapeSelection *self, ...);
void moto_shape_selection_deselect_all_edges(MotoShapeSelection *self);
void moto_shape_selection_toggle_edge_selection(MotoShapeSelection *self, guint index);
gboolean moto_shape_selection_is_edge_selected(MotoShapeSelection *self, guint index);

void moto_shape_selection_select_face(MotoShapeSelection *self, guint index);
void moto_shape_selection_select_faces(MotoShapeSelection *self, ...);
void moto_shape_selection_deselect_face(MotoShapeSelection *self, guint index);
void moto_shape_selection_deselect_all_faces(MotoShapeSelection *self);
void moto_shape_selection_toggle_face_selection(MotoShapeSelection *self, guint index);
gboolean moto_shape_selection_is_face_selected(MotoShapeSelection *self, guint index);

void moto_shape_selection_deselect_all(MotoShapeSelection *self);

G_END_DECLS

#endif /* __MOTO_SHAPE_H__ */

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

#ifndef __MOTO_SHAPE_NODE_H__
#define __MOTO_SHAPE_NODE_H__

#include "moto-node.h"
#include "moto-bound.h"
#include "moto-shape.h"
#include "moto-enums.h"
#include "moto-transform-info.h"

G_BEGIN_DECLS

typedef struct _MotoShapeNode MotoShapeNode;
typedef struct _MotoShapeNodeClass MotoShapeNodeClass;

typedef MotoBound *(*MotoShapeNodeGetBoundMethod)(MotoShapeNode *self);
typedef void (*MotoShapeNodeDrawMethod)(MotoShapeNode *self, MotoDrawMode draw_mode,
    MotoShapeSelection* selection, MotoSelectionMode selection_mode);

typedef void (*MotoShapeNodeSelectMoreMethod)(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
typedef void (*MotoShapeNodeSelectLessMethod)(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
typedef void (*MotoShapeNodeSelectInverseMethod)(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);

/* class MotoShapeNode */

struct _MotoShapeNode
{
    MotoNode parent;
};

struct _MotoShapeNodeClass
{
    MotoNodeClass parent;

    MotoShapeNodeGetBoundMethod get_bound;
    MotoShapeNodeDrawMethod draw;
    MotoShapeNodeSelectMoreMethod select_more;
    MotoShapeNodeSelectLessMethod select_less;
    MotoShapeNodeSelectInverseMethod select_inverse;
};

GType moto_shape_node_get_type(void);

#define MOTO_TYPE_SHAPE_NODE (moto_shape_node_get_type())
#define MOTO_SHAPE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SHAPE_NODE, MotoShapeNode))
#define MOTO_SHAPE_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SHAPE_NODE, MotoShapeNodeClass))
#define MOTO_IS_SHAPE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SHAPE_NODE))
#define MOTO_IS_SHAPE_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SHAPE_NODE))
#define MOTO_SHAPE_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SHAPE_NODE, MotoShapeNodeClass))

MotoBound *moto_shape_node_get_bound(MotoShapeNode *self);

MotoShape* moto_shape_node_get_shape(MotoShapeNode* self);

void moto_shape_node_draw(MotoShapeNode* self, MotoDrawMode draw_mode,
    MotoShapeSelection* selection, MotoSelectionMode selection_mode);

void moto_shape_node_select_more(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
void moto_shape_node_select_less(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);
void moto_shape_node_select_inverse(MotoShapeNode* self,
    MotoShapeSelection* selection, MotoSelectionMode mode);

gboolean
moto_shape_node_select(MotoShapeNode *self,
        MotoShapeSelection* selection, MotoSelectionMode mode,
        gint x, gint y, gint width, gint height,
        MotoRay *ray, MotoTransformInfo *tinfo);

G_END_DECLS

#endif /* __MOTO_SHAPE_NODE_H__ */

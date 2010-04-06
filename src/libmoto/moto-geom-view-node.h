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

#ifndef __MOTO_SHAPE_VIEW_NODE_H__
#define __MOTO_SHAPE_VIEW_NODE_H__

#include <GL/glew.h>
#include <GL/glu.h>

#include "moto-ray.h"
#include "moto-transform-info.h"
#include "moto-shape-node.h"

G_BEGIN_DECLS

typedef struct _MotoShapeViewNode MotoShapeViewNode;
typedef struct _MotoShapeViewNodeClass MotoShapeViewNodeClass;
typedef struct _MotoShapeViewNodePriv MotoShapeViewNodePriv;

typedef void (*MotoShapeViewNodeDrawMethod)(MotoShapeViewNode *self);
typedef void (*MotoShapeViewNodePrepareForDrawMethod)(MotoShapeViewNode *self);
typedef gboolean (*MotoShapeViewNodeSelectMethod)(MotoShapeViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
typedef MotoShapeNode *(*MotoShapeViewNodeGetShapeMethod)(MotoShapeViewNode *self);
typedef void (*MotoShapeViewNodeGrowSelectionMethod)(MotoShapeViewNode *self);
typedef void (*MotoShapeViewNodeSelectLessMethod)(MotoShapeViewNode *self);
typedef void (*MotoShapeViewNodeInvertSelectionMethod)(MotoShapeViewNode *self);

typedef struct _MotoShapeViewState MotoShapeViewState;
typedef struct _MotoShapeViewStateClass MotoShapeViewStateClass;
typedef struct _MotoShapeViewStatePriv MotoShapeViewStatePriv;

typedef void (*MotoShapeViewStateDrawFunc)(MotoShapeViewState *self, MotoShapeViewNode *geom);
typedef gboolean (*MotoShapeViewStateSelectFunc)(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
typedef void (*MotoShapeViewStateGrowSelectionFunc)(MotoShapeViewState *self, MotoShapeViewNode *geom);
typedef void (*MotoShapeViewStateSelectLessFunc)(MotoShapeViewState *self, MotoShapeViewNode *geom);
typedef void (*MotoShapeViewStateInvertSelectionFunc)(MotoShapeViewState *self, MotoShapeViewNode *geom);
typedef void (*MotoShapeViewStateLeaveFunc)(MotoShapeViewState *self, MotoShapeViewNode *geom);


/* Drawing modes.
 *
 * These modes may be set global for all drawable objects in world
 * and can be overriden for particular object.
 *
 * */

/* MotoShapeViewNode */

struct _MotoShapeViewNode
{
    MotoNode parent;

    MotoShapeViewNodePriv *priv;
};

struct _MotoShapeViewNodeClass
{
    MotoNodeClass parent;

    MotoShapeViewNodeDrawMethod draw;
    MotoShapeViewNodePrepareForDrawMethod prepare_for_draw;
    MotoShapeViewNodeSelectMethod select;
    MotoShapeViewNodeGetShapeMethod get_shape;
    MotoShapeViewNodeGrowSelectionMethod grow_selection;
    MotoShapeViewNodeSelectLessMethod select_less;
    MotoShapeViewNodeInvertSelectionMethod invert_selection;

    GSList *states;

    /* signals */
    guint before_draw_signal_id;
    guint after_draw_signal_id;
    guint before_prepare_for_draw_signal_id;
    guint after_prepare_for_draw_signal_id;

    guint button_press_signal_id;
    guint button_release_signal_id;
    guint motion_notify_signal_id;
};

GType moto_shape_view_node_get_type(void);

#define MOTO_TYPE_GEOM_VIEW_NODE (moto_shape_view_node_get_type())
#define MOTO_SHAPE_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOM_VIEW_NODE, MotoShapeViewNode))
#define MOTO_SHAPE_VIEW_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOM_VIEW_NODE, MotoShapeViewNodeClass))
#define MOTO_IS_GEOM_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOM_VIEW_NODE))
#define MOTO_IS_GEOM_VIEW_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOM_VIEW_NODE))
#define MOTO_SHAPE_VIEW_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOM_VIEW_NODE, MotoShapeViewNodeClass))

void moto_shape_view_node_draw(MotoShapeViewNode *self);
void moto_shape_view_node_prepare_for_draw(MotoShapeViewNode *self);
gboolean moto_shape_view_node_select(MotoShapeViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);

gboolean moto_shape_view_node_get_prepared(MotoShapeViewNode *self);
void moto_shape_view_node_set_prepared(MotoShapeViewNode *self, gboolean status);

MotoShapeViewState *moto_shape_view_node_get_state(MotoShapeViewNode *self);
void moto_shape_view_node_set_state(MotoShapeViewNode *self, const gchar *state_name);
GSList *moto_shape_view_node_get_state_list(MotoShapeViewNode *self);
void moto_shape_view_node_goto_next_state(MotoShapeViewNode *self);

MotoShapeNode *moto_shape_view_node_get_shape(MotoShapeViewNode *self);

void moto_shape_view_node_grow_selection(MotoShapeViewNode *self);
void moto_shape_view_node_select_less(MotoShapeViewNode *self);
void moto_shape_view_node_invert_selection(MotoShapeViewNode *self);

gboolean moto_shape_view_node_process_button_press(MotoShapeViewNode *self,
    gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
gboolean moto_shape_view_node_process_button_release(MotoShapeViewNode *self,
    gint x, gint y, gint width, gint height);
gboolean moto_shape_view_node_process_motion(MotoShapeViewNode *self,
    gint x, gint y, gint width, gint height);

/* class MotoShapeViewState */

struct _MotoShapeViewState
{
    GObject parent;

    MotoShapeViewStatePriv *priv;
};

struct _MotoShapeViewStateClass
{
    GObjectClass parent;
};

GType moto_shape_view_state_get_type(void);

#define MOTO_TYPE_GEOM_VIEW_STATE (moto_shape_view_state_get_type())
#define MOTO_SHAPE_VIEW_STATE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOM_VIEW_STATE, MotoShapeViewState))
#define MOTO_SHAPE_VIEW_STATE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOM_VIEW_STATE, MotoShapeViewStateClass))
#define MOTO_IS_GEOM_VIEW_STATE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOM_VIEW_STATE))
#define MOTO_IS_GEOM_VIEW_STATE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOM_VIEW_STATE))
#define MOTO_SHAPE_VIEW_STATE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOM_VIEW_STATE, MotoShapeViewStateClass))

MotoShapeViewState *
moto_shape_view_state_new(const gchar *name, const gchar *title,
        MotoShapeViewStateDrawFunc draw, MotoShapeViewStateSelectFunc select,
        MotoShapeViewStateGrowSelectionFunc grow_selection,
        MotoShapeViewStateSelectLessFunc select_less,
        MotoShapeViewStateInvertSelectionFunc invert_selection,
        MotoShapeViewStateLeaveFunc leave);

const gchar *moto_shape_view_state_get_name(MotoShapeViewState *self);
const gchar *moto_shape_view_state_get_title(MotoShapeViewState *self);

void moto_shape_view_state_draw(MotoShapeViewState *self, MotoShapeViewNode *geom);
gboolean moto_shape_view_state_select(MotoShapeViewState *self, MotoShapeViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);

void moto_shape_view_state_grow_selection(MotoShapeViewState *self, MotoShapeViewNode *geom);
void moto_shape_view_state_select_less(MotoShapeViewState *self, MotoShapeViewNode *geom);
void moto_shape_view_state_invert_selection(MotoShapeViewState *self, MotoShapeViewNode *geom);
void moto_shape_view_state_leave(MotoShapeViewState *self, MotoShapeViewNode *geom);

G_END_DECLS

#endif /* __MOTO_SHAPE_VIEW_NODE_H__ */

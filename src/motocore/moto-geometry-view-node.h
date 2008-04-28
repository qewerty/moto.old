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

#ifndef MOTO_GEOMETRY_VIEW_NODE_H
#define MOTO_GEOMETRY_VIEW_NODE_H

#include "moto-geometry-node.h"

typedef struct _MotoGeometryViewNode MotoGeometryViewNode;
typedef struct _MotoGeometryViewNodeClass MotoGeometryViewNodeClass;
typedef struct _MotoGeometryViewNodePriv MotoGeometryViewNodePriv;

typedef void (*MotoGeometryViewNodeDrawMethod)(MotoGeometryViewNode *self);
typedef void (*MotoGeometryViewNodePrepareForDrawMethod)(MotoGeometryViewNode *self);
typedef MotoGeometryNode *(*MotoGeometryViewNodeGetGeometryMethod)(MotoGeometryViewNode *self);

typedef struct _MotoGeometryViewState MotoGeometryViewState;
typedef struct _MotoGeometryViewStateClass MotoGeometryViewStateClass;
typedef struct _MotoGeometryViewStatePriv MotoGeometryViewStatePriv;

typedef void (*MotoGeometryViewStateDrawFunc)(MotoGeometryViewState *self, MotoGeometryViewNode *geom);

/* class MotoGeometryViewNode */

struct _MotoGeometryViewNode
{
    MotoNode parent;

    MotoGeometryViewNodePriv *priv;
};

struct _MotoGeometryViewNodeClass
{
    MotoNodeClass parent;

    MotoGeometryViewNodeDrawMethod draw;
    MotoGeometryViewNodePrepareForDrawMethod prepare_for_draw;
    MotoGeometryViewNodeGetGeometryMethod get_geometry;

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

GType moto_geometry_view_node_get_type(void);

#define MOTO_TYPE_GEOMETRY_VIEW_NODE (moto_geometry_view_node_get_type())
#define MOTO_GEOMETRY_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeometryViewNode))
#define MOTO_GEOMETRY_VIEW_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeometryViewNodeClass))
#define MOTO_IS_GEOMETRY_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOMETRY_VIEW_NODE))
#define MOTO_IS_GEOMETRY_VIEW_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOMETRY_VIEW_NODE))
#define MOTO_GEOMETRY_VIEW_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeometryViewNodeClass))

void moto_geometry_view_node_draw(MotoGeometryViewNode *self);
void moto_geometry_view_node_prepare_for_draw(MotoGeometryViewNode *self);

gboolean moto_geometry_view_node_get_prepared(MotoGeometryViewNode *self);
void moto_geometry_view_node_set_prepared(MotoGeometryViewNode *self, gboolean status);

MotoGeometryViewState *moto_geometry_view_node_get_state(MotoGeometryViewNode *self);
void moto_geometry_view_node_set_state(MotoGeometryViewNode *self, const gchar *state_name);
GSList *moto_geometry_view_node_get_state_list(MotoGeometryViewNode *self);

MotoGeometryNode *moto_geometry_view_node_get_geometry(MotoGeometryViewNode *self);

gboolean moto_geometry_view_node_process_button_press(MotoGeometryViewNode *self,
    gint x, gint y, gint width, gint height);
gboolean moto_geometry_view_node_process_button_release(MotoGeometryViewNode *self,
    gint x, gint y, gint width, gint height);
gboolean moto_geometry_view_node_process_motion(MotoGeometryViewNode *self,
    gint x, gint y, gint width, gint height);

/* class MotoGeometryViewState */

struct _MotoGeometryViewState
{
    GObject parent;

    MotoGeometryViewStatePriv *priv;
};

struct _MotoGeometryViewStateClass
{
    GObjectClass parent;
};

GType moto_geometry_view_state_get_type(void);

#define MOTO_TYPE_GEOMETRY_VIEW_STATE (moto_geometry_view_state_get_type())
#define MOTO_GEOMETRY_VIEW_STATE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOMETRY_VIEW_STATE, MotoGeometryViewState))
#define MOTO_GEOMETRY_VIEW_STATE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOMETRY_VIEW_STATE, MotoGeometryViewStateClass))
#define MOTO_IS_GEOMETRY_VIEW_STATE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOMETRY_VIEW_STATE))
#define MOTO_IS_GEOMETRY_VIEW_STATE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOMETRY_VIEW_STATE))
#define MOTO_GEOMETRY_VIEW_STATE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOMETRY_VIEW_STATE, MotoGeometryViewStateClass))

MotoGeometryViewState *
moto_geometry_view_state_new(const gchar *name, const gchar *title,
        MotoGeometryViewStateDrawFunc draw);

const gchar *moto_geometry_view_state_get_name(MotoGeometryViewState *self);
const gchar *moto_geometry_view_state_get_title(MotoGeometryViewState *self);

void moto_geometry_view_state_draw(MotoGeometryViewState *self, MotoGeometryViewNode *geom);

#endif /* MOTO_GEOMETRY_VIEW_NODE_H */

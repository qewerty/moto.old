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

#ifndef MOTO_GEOM_VIEW_NODE_H
#define MOTO_GEOM_VIEW_NODE_H

#include <GL/gl.h>
#include <GL/glu.h>

#include "moto-ray.h"
#include "moto-transform-info.h"
#include "moto-geometry-node.h"

typedef struct _MotoGeomViewNode MotoGeomViewNode;
typedef struct _MotoGeomViewNodeClass MotoGeomViewNodeClass;
typedef struct _MotoGeomViewNodePriv MotoGeomViewNodePriv;

typedef void (*MotoGeomViewNodeDrawMethod)(MotoGeomViewNode *self);
typedef void (*MotoGeomViewNodePrepareForDrawMethod)(MotoGeomViewNode *self);
typedef gboolean (*MotoGeomViewNodeSelectMethod)(MotoGeomViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
typedef MotoGeometryNode *(*MotoGeomViewNodeGetGeometryMethod)(MotoGeomViewNode *self);

typedef struct _MotoGeomViewState MotoGeomViewState;
typedef struct _MotoGeomViewStateClass MotoGeomViewStateClass;
typedef struct _MotoGeomViewStatePriv MotoGeomViewStatePriv;

typedef void (*MotoGeomViewStateDrawFunc)(MotoGeomViewState *self, MotoGeomViewNode *geom);
typedef gboolean (*MotoGeomViewStateSelectFunc)(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);


/* Drawing modes.
 *
 * These modes may be set global for all drawable objects in world
 * and can be overriden for particular object.
 *
 * - Bounding box
 * - Wireframe
 * - Textured wireframe (half-transparent)
 * - Solid shaded
 *   Textured solid shaded
 * - Smooth shaded
 *   Textured smooth shaded
 *
 * */

typedef enum
{
    MOTO_GEOM_VIEW_DRAW_MODE_BBOX,
    MOTO_GEOM_VIEW_DRAW_MODE_WIREFRAME,
    MOTO_GEOM_VIEW_DRAW_MODE_WIREFRAME_TEX,
    MOTO_GEOM_VIEW_DRAW_MODE_SOLID,
    MOTO_GEOM_VIEW_DRAW_MODE_SOLID_TEX,
    MOTO_GEOM_VIEW_DRAW_MODE_SMOOTH,
    MOTO_GEOM_VIEW_DRAW_MODE_SMOOTH_TEX
} MotoGeomViewDrawMode;

GType moto_geom_view_draw_mode_get_type(void);
#define MOTO_TYPE_GEOM_VIEW_DRAW_MODE (moto_geom_view_draw_mode_get_type())

/* class MotoGeomViewNode */

struct _MotoGeomViewNode
{
    MotoNode parent;

    MotoGeomViewNodePriv *priv;
};

struct _MotoGeomViewNodeClass
{
    MotoNodeClass parent;

    MotoGeomViewNodeDrawMethod draw;
    MotoGeomViewNodePrepareForDrawMethod prepare_for_draw;
    MotoGeomViewNodeSelectMethod select;
    MotoGeomViewNodeGetGeometryMethod get_geometry;

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

GType moto_geom_view_node_get_type(void);

#define MOTO_TYPE_GEOMETRY_VIEW_NODE (moto_geom_view_node_get_type())
#define MOTO_GEOM_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeomViewNode))
#define MOTO_GEOM_VIEW_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeomViewNodeClass))
#define MOTO_IS_GEOMETRY_VIEW_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOMETRY_VIEW_NODE))
#define MOTO_IS_GEOMETRY_VIEW_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOMETRY_VIEW_NODE))
#define MOTO_GEOM_VIEW_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOMETRY_VIEW_NODE, MotoGeomViewNodeClass))

void moto_geom_view_node_draw(MotoGeomViewNode *self);
void moto_geom_view_node_prepare_for_draw(MotoGeomViewNode *self);
gboolean moto_geom_view_node_select(MotoGeomViewNode *self,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);

gboolean moto_geom_view_node_get_prepared(MotoGeomViewNode *self);
void moto_geom_view_node_set_prepared(MotoGeomViewNode *self, gboolean status);

MotoGeomViewState *moto_geom_view_node_get_state(MotoGeomViewNode *self);
void moto_geom_view_node_set_state(MotoGeomViewNode *self, const gchar *state_name);
GSList *moto_geom_view_node_get_state_list(MotoGeomViewNode *self);

MotoGeometryNode *moto_geom_view_node_get_geometry(MotoGeomViewNode *self);

gboolean moto_geom_view_node_process_button_press(MotoGeomViewNode *self,
    gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);
gboolean moto_geom_view_node_process_button_release(MotoGeomViewNode *self,
    gint x, gint y, gint width, gint height);
gboolean moto_geom_view_node_process_motion(MotoGeomViewNode *self,
    gint x, gint y, gint width, gint height);

/* class MotoGeomViewState */

struct _MotoGeomViewState
{
    GObject parent;

    MotoGeomViewStatePriv *priv;
};

struct _MotoGeomViewStateClass
{
    GObjectClass parent;
};

GType moto_geom_view_state_get_type(void);

#define MOTO_TYPE_GEOMETRY_VIEW_STATE (moto_geom_view_state_get_type())
#define MOTO_GEOM_VIEW_STATE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_GEOMETRY_VIEW_STATE, MotoGeomViewState))
#define MOTO_GEOM_VIEW_STATE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_GEOMETRY_VIEW_STATE, MotoGeomViewStateClass))
#define MOTO_IS_GEOMETRY_VIEW_STATE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_GEOMETRY_VIEW_STATE))
#define MOTO_IS_GEOMETRY_VIEW_STATE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_GEOMETRY_VIEW_STATE))
#define MOTO_GEOM_VIEW_STATE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_GEOMETRY_VIEW_STATE, MotoGeomViewStateClass))

MotoGeomViewState *
moto_geom_view_state_new(const gchar *name, const gchar *title,
        MotoGeomViewStateDrawFunc draw, MotoGeomViewStateSelectFunc select);

const gchar *moto_geom_view_state_get_name(MotoGeomViewState *self);
const gchar *moto_geom_view_state_get_title(MotoGeomViewState *self);

void moto_geom_view_state_draw(MotoGeomViewState *self, MotoGeomViewNode *geom);
gboolean moto_geom_view_state_select(MotoGeomViewState *self, MotoGeomViewNode *geom,
        gint x, gint y, gint width, gint height, MotoRay *ray, MotoTransformInfo *tinfo);

#endif /* MOTO_GEOM_VIEW_NODE_H */

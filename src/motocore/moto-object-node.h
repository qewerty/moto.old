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

#ifndef MOTO_OBJECT_NODE_H
#define MOTO_OBJECT_NODE_H

#include "moto-node.h"
#include "moto-bound.h"
#include "moto-geometry-view-node.h"

typedef struct _MotoObjectNodePriv MotoObjectNodePriv;

typedef struct _MotoObjectNodeFactory MotoObjectNodeFactory;
typedef struct _MotoObjectNodeFactoryClass MotoObjectNodeFactoryClass;
typedef struct _MotoObjectNodeFactoryPriv MotoObjectNodeFactoryPriv;

typedef const MotoBound *(*MotoObjectNodeGetBoundMethod)(MotoObjectNode *self);

typedef enum
{
    MOTO_TRANSFORM_ORDER_TRS, MOTO_TRANSFORM_ORDER_TSR,
    MOTO_TRANSFORM_ORDER_RTS, MOTO_TRANSFORM_ORDER_RST,
    MOTO_TRANSFORM_ORDER_STR, MOTO_TRANSFORM_ORDER_SRT
} MotoTransformOrder;

typedef enum
{
    MOTO_ROTATE_ORDER_XYZ, MOTO_ROTATE_ORDER_XZY,
    MOTO_ROTATE_ORDER_YXZ, MOTO_ROTATE_ORDER_YZX,
    MOTO_ROTATE_ORDER_ZXY, MOTO_ROTATE_ORDER_ZYX
} MotoRotateOrder;

typedef enum
{
    MOTO_TRANSFORM_STRATEGY_SOFTWARE,
    MOTO_TRANSFORM_STRATEGY_HARDWARE
} MotoTransformStrategy;

/* class MotoObjectNode */

struct _MotoObjectNode
{
    MotoNode parent;

    MotoObjectNodePriv *priv;
};

struct _MotoObjectNodeClass
{
    MotoNodeClass parent;
};

GType moto_object_node_get_type(void);

#define MOTO_TYPE_OBJECT_NODE (moto_object_node_get_type())
#define MOTO_OBJECT_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_OBJECT_NODE, MotoObjectNode))
#define MOTO_OBJECT_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_OBJECT_NODE, MotoObjectNodeClass))
#define MOTO_IS_OBJECT_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_OBJECT_NODE))
#define MOTO_IS_OBJECT_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_OBJECT_NODE))
#define MOTO_OBJECT_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_OBJECT_NODE, MotoObjectNodeClass))

MotoObjectNode *moto_object_node_new(const gchar *name);

const MotoBound *moto_object_node_get_bound(MotoObjectNode *self);
void moto_object_node_draw(MotoObjectNode *self);

void moto_object_node_look_at(MotoObjectNode *self, gfloat eye[3], gfloat look[3], gfloat up[3]);
void moto_object_node_slide(MotoObjectNode *self,
        gfloat dx, gfloat dy, gfloat dz);
void moto_object_node_zoom(MotoObjectNode *self, gfloat val);
void moto_object_node_thumble(MotoObjectNode *self, gfloat da);
void moto_object_node_roll(MotoObjectNode *self, gfloat da);
void moto_object_node_pitch(MotoObjectNode *self, gfloat da);
void moto_object_node_yaw(MotoObjectNode *self, gfloat da);
void moto_object_node_apply_camera_transform(MotoObjectNode *self, gint width, gint height);

/* class MotoNodeFactory */

struct _MotoObjectNodeFactory
{
    MotoNodeFactory parent;
};

struct _MotoObjectNodeFactoryClass
{
    MotoNodeFactoryClass parent;
};

GType moto_object_node_factory_get_type(void);

#define MOTO_TYPE_OBJECT_NODE_FACTORY (moto_object_node_factory_get_type())
#define MOTO_OBJECT_NODE_FACTORY(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_OBJECT_NODE_FACTORY, MotoObjectNodeFactory))
#define MOTO_OBJECT_NODE_FACTORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_OBJECT_NODE_FACTORY, MotoObjectNodeFactoryClass))
#define MOTO_IS_OBJECT_NODE_FACTORY(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_OBJECT_NODE_FACTORY))
#define MOTO_IS_OBJECT_NODE_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_OBJECT_NODE_FACTORY))
#define MOTO_OBJECT_NODE_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_OBJECT_NODE_FACTORY, MotoObjectNodeFactoryClass))

MotoNodeFactory *moto_object_node_factory_new();

#endif /* MOTO_OBJECT_NODE_H */


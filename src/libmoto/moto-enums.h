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

#ifndef __MOTO_ENUMS__
#define __MOTO_ENUMS__

#include <glib-object.h>

G_BEGIN_DECLS

typedef enum _MotoAxis
{
    MOTO_AXIS_X,
    MOTO_AXIS_Y,
    MOTO_AXIS_Z
} MotoAxis;

GType moto_axis_get_type(void);

#define MOTO_TYPE_AXIS (moto_axis_get_type())

typedef enum
{
    MOTO_DRAW_MODE_WIREFRAME_BBOX,
    MOTO_DRAW_MODE_SOLID_BBOX,
    MOTO_DRAW_MODE_WIREFRAME,
    MOTO_DRAW_MODE_SOLID,
    MOTO_DRAW_MODE_SMOOTH,
    MOTO_DRAW_MODE_SHADED
} MotoDrawMode;

GType moto_draw_mode_get_type(void);
#define MOTO_TYPE_DRAW_MODE (moto_draw_mode_get_type())

typedef enum
{
    MOTO_SELECTION_MODE_OBJECT,
    MOTO_SELECTION_MODE_VERTEX,
    MOTO_SELECTION_MODE_EDGE,
    MOTO_SELECTION_MODE_FACE,
} MotoSelectionMode;

GType moto_selection_mode_get_type(void);
#define MOTO_TYPE_SELECTION_MODE (moto_selection_mode_get_type())

typedef enum
{
    MOTO_CULL_FACE_MODE_NONE,
    MOTO_CULL_FACE_MODE_BACK,
    MOTO_CULL_FACE_MODE_FRONT,
    MOTO_CULL_FACE_MODE_FRONT_AND_BACK,
} MotoCullFaceMode;

GType moto_cull_face_mode_get_type(void);
#define MOTO_TYPE_CULL_FACE_MODE (moto_cull_face_mode_get_type())

typedef enum _MotoExtrudeMode
{
    MOTO_EXTRUDE_MODE_VERTS,
    MOTO_EXTRUDE_MODE_EDGES,
    MOTO_EXTRUDE_MODE_FACES,
    MOTO_EXTRUDE_MODE_REGION,
} MotoExtrudeMode;

GType moto_extrude_mode_get_type(void);

#define MOTO_TYPE_EXTRUDE_MODE (moto_extrude_mode_get_type())

typedef enum _MotoArrayMode
{
    MOTO_ARRAY_MODE_LOCAL,
    MOTO_ARRAY_MODE_GLOBAL,
    MOTO_ARRAY_MODE_WEIGHT,
    MOTO_ARRAY_MODE_RELATIVE,
} MotoArrayMode;

GType moto_array_mode_get_type(void);

#define MOTO_TYPE_ARRAY_MODE (moto_array_mode_get_type())

G_END_DECLS

#endif /* __MOTO_ENUMS__ */

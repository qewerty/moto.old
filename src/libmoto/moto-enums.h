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
    MOTO_DRAW_MODE_NONE,
    MOTO_DRAW_MODE_BBOX_WIREFRAME,
    MOTO_DRAW_MODE_BBOX_SOLID,
    MOTO_DRAW_MODE_WIREFRAME,
    MOTO_DRAW_MODE_WIREFRAME_TEX,
    MOTO_DRAW_MODE_SOLID,
    MOTO_DRAW_MODE_SOLID_TEX,
    MOTO_DRAW_MODE_SMOOTH,
    MOTO_DRAW_MODE_SMOOTH_TEX,
    MOTO_DRAW_MODE_SHADED
} MotoDrawMode;

GType moto_draw_mode_get_type(void);
#define MOTO_TYPE_DRAW_MODE (moto_draw_mode_get_type())

G_END_DECLS

#endif /* __MOTO_ENUMS__ */

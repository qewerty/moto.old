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

/* Drawing simple primitives with OpenGL. */

#ifndef __MOTO_DRAW_H__
#define __MOTO_DRAW_H__

#ifdef __cplusplus
extern "C" {
#endif

inline void moto_draw_circle(float radius);
inline void moto_draw_arc(float radius, float start_angle, float end_angle);

inline void moto_draw_bezier_segment();

#ifdef __cplusplus
}
#endif

#endif /* __MOTO_DRAW_H__ */

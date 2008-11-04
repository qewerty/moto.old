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

#ifndef __MOTO_TRANSFORM_INFO_H__
#define __MOTO_TRANSFORM_INFO_H__

#include <glib.h>
#include <GL/glew.h>

G_BEGIN_DECLS

typedef struct _MotoTransformInfo MotoTransformInfo;

struct _MotoTransformInfo
{
    GLdouble model[16];
    GLdouble proj[16];
    GLint    view[4];
};

void moto_transform_info_copy(MotoTransformInfo *self, MotoTransformInfo *other);
void moto_transform_info_set_model(MotoTransformInfo *self, GLdouble model[16]);
void moto_transform_info_set_proj(MotoTransformInfo *self, GLdouble proj[16]);
void moto_transform_info_set_view(MotoTransformInfo *self, GLint view[4]);

G_END_DECLS

#endif /* __MOTO_TRANSFORM_INFO_H__ */

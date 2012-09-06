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

#ifndef __MOTO_FORWARD_H__
#define __MOTO_FORWARD_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MotoSystem MotoSystem;
typedef struct _MotoSystemClass MotoSystemClass;
typedef struct _MotoSystemPriv MotoSystemPriv;

typedef struct _MotoSceneNode MotoSceneNode;
typedef struct _MotoSceneNodeClass MotoSceneNodeClass;
typedef struct _MotoSceneNodePriv MotoSceneNodePriv;

typedef struct _MotoObjectNode MotoObjectNode;
typedef struct _MotoObjectNodeClass MotoObjectNodeClass;

typedef struct _MotoNode MotoNode;
typedef struct _MotoNodeClass MotoNodeClass;
typedef void (*MotoNodeUpdateMethod)(MotoNode *self);

typedef struct _MotoVariation MotoVariation;
typedef struct _MotoVariationClass MotoVariationClass;

typedef struct _MotoParam MotoParam;
typedef struct _MotoParamClass MotoParamClass;

typedef struct _MotoManipulator MotoManipulator;
typedef struct _MotoManipulatorClass MotoManipulatorClass;

typedef struct _MotoHandle MotoHandle;
typedef struct _MotoHandleClass MotoHandleClass;

typedef struct _MotoLibrary MotoLibrary;
typedef struct _MotoLibraryClass MotoLibraryClass;

G_END_DECLS

#endif /* __MOTO_FORWARD_H__ */

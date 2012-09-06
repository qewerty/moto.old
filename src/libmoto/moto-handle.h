/* ##################################################################################
#
#  Moto Animation System (http://motoanim.sf.net)
#  Copyleft (C) 2012 Konstantin Evdokimenko a.k.a Qew[erty] (qewerty@gmail.com)
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

#ifndef __MOTO_HANDLE_H__
#define __MOTO_HANDLE_H__

#include "moto-forward.h"

G_BEGIN_DECLS

typedef void (*MotoHandleSetNodeFunc)(MotoHandle*, MotoNode*);
typedef void (*MotoHandleDrawFunc)(MotoHandle*);

struct _MotoHandle
{
    GObject parent;
};

struct _MotoHandleClass
{
    GObjectClass parent;
    MotoHandleSetNodeFunc set_node;
    MotoHandleDrawFunc draw;
};

GType moto_handle_get_type(void);

#define MOTO_TYPE_HANDLE (moto_handle_get_type())
#define MOTO_HANDLE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_HANDLE, MotoHandle))
#define MOTO_HANDLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_HANDLE, MotoHandleClass))
#define MOTO_IS_HANDLE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_HANDLE))
#define MOTO_IS_HANDLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_HANDLE))
#define MOTO_HANDLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_HANDLE, MotoHandleClass))

MotoHandle* moto_handle_new(MotoNode* node);
MotoNode* moto_handle_get_node(MotoHandle* self);
void moto_handle_set_node(MotoHandle* self, MotoNode* node);
void moto_handle_draw(MotoHandle* self);

/* MotoObjectHandle */

typedef struct _MotoObjectHandle MotoObjectHandle;
typedef struct _MotoObjectHandleClass MotoObjectHandleClass;

struct _MotoObjectHandle
{
    MotoHandle parent;
};

struct _MotoObjectHandleClass
{
    MotoHandleClass parent;
};

MotoHandle* moto_object_handle_new(MotoNode* node);

G_END_DECLS

#endif /* __MOTO_HANDLE_H__ */

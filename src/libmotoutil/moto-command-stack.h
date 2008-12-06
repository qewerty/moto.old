
/* ##################################################################################
#
#  Moto - small node based image compositor with 32bit floating point processing
#  of data (http://motocompose.sf.net)
#  Copyleft (C) 2006 Konstantin Evdokimenko a.k.a Qew (qewerty@gmail.com)
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

#ifndef __MOTO_COMMAND_H__
#define __MOTO_COMMAND_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* MotoCommand */

typedef struct _MotoCommand MotoCommand;
typedef struct _MotoCommandClass MotoCommandClass;

typedef void (*MotoCommandDoMethod)(MotoCommand *self);
typedef void (*MotoCommandUndoMethod)(MotoCommand *self);

struct _MotoCommand
{
    GObject parent;
};

struct _MotoCommandClass
{
    GObjectClass parent;

    MotoCommandDoMethod do_command;
    MotoCommandDoMethod undo_command;
};

GType moto_command_get_type(void);

#define MOTO_TYPE_COMMAND (moto_command_get_type())
#define MOTO_COMMAND(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COMMAND, MotoCommand))
#define MOTO_COMMAND_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_COMMAND, MotoCommandClass))
#define MOTO_IS_COMMAND(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COMMAND))
#define MOTO_IS_COMMAND_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_COMMAND))
#define MOTO_COMMAND_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_COMMAND, MotoCommandClass))

/* methods */

gchar *moto_command_get_name(MotoCommand *self);

void moto_command_do(MotoCommand *self);
void moto_command_undo(MotoCommand *self);

/* MotoCommandStack */

typedef struct _MotoCommandStack MotoCommandStack;
typedef struct _MotoCommandStackClass MotoCommandStackClass;

struct _MotoCommandStack
{
    GObject parent;
};

struct _MotoCommandStackClass
{
    GObjectClass parent;
};

GType moto_command_stack_get_type(void);

#define MOTO_TYPE_COMMAND_STACK (moto_command_stack_get_type())
#define MOTO_COMMAND_STACK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COMMAND_STACK, MotoCommandStack))
#define MOTO_COMMAND_STACK_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_COMMAND_STACK, MotoCommandStackClass))
#define MOTO_IS_COMMAND_STACK(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COMMAND_STACK))
#define MOTO_IS_COMMAND_STACK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_COMMAND_STACK))
#define MOTO_COMMAND_STACK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_COMMAND_STACK, MotoCommandStackClass))

/* methods */

MotoCommandStack *moto_command_stack_new(void);

void moto_command_stack_do(MotoCommandStack *self,
                           MotoCommand *command);
void moto_command_stack_undo(MotoCommandStack *self);
void moto_command_stack_redo(MotoCommandStack *self);

void moto_command_stack_begin(MotoCommandStack *self);
void moto_command_stack_rollback(MotoCommandStack *self);
void moto_command_stack_commit(MotoCommandStack *self);

G_END_DECLS

#endif /* __MOTO_COMMAND_H__ */

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

#ifndef __MOTO_TASK_H__
#define __MOTO_TASK_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _MotoTask MotoTask;
typedef struct _MotoTaskPool MotoTaskPool;
typedef struct _MotoTaskQueue MotoTaskQueue;
typedef void (*MotoTaskFunc)(MotoTask *task, gpointer data);
typedef void (*MotoTaskFreeDataFunc)(MotoTask *task, gpointer data);

/* MotoTask */

struct _MotoTask
{
    gpointer data;
    MotoTaskFunc func;
    MotoTaskFreeDataFunc free_data_func;

    guint deps_num;
    GList *children;
};

MotoTask *
moto_task_new(MotoTaskFunc func,
              MotoTaskFreeDataFunc free_data_func,
              gpointer data);
void moto_task_free(MotoTask *self);

void moto_task_add_child(MotoTask *self, MotoTask *child);

/* MotoTaskPool */

struct _MotoTaskPool
{
    GList *tasks;
    MotoTaskQueue *queue;
};

MotoTaskPool moto_task_pool_new();
void moto_task_pool_free(MotoTaskPool *self);

void moto_task_pool_add_task(MotoTaskPool *self, MotoTask *task);

void moto_task_pool_do(MotoTaskPool *self);

/* MotoTaskQueue */

struct _MotoTaskQueue
{
    GQueue *tasks;
    GMutex *tasks_mutex;
};

MotoTaskQueue *moto_task_queue_new();
void moto_task_queue_free(MotoTaskQueue *self);

void moto_task_queue_push(MotoTaskQueue *self, MotoTask *task);

MotoTask *moto_task_queue_pop(MotoTaskQueue *self);

G_END_DECLS

#endif /* __MOTO_TASK_H__ */

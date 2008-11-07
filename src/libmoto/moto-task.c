#include "moto-task.h"

/* MotoTask */

MotoTask *
moto_task_new(MotoTaskFunc func,
              MotoTaskFreeDataFunc free_data_func,
              gpointer data)
{
    MotoTask *self = g_slice_new(MotoTask);

    self->func = func;
    self->free_data_func = free_data_func;
    self->data = data;

    self->deps_num = 0;
    self->children = NULL;

    return self;
}

void moto_task_free(MotoTask *self)
{
    GList *ch = g_list_first(self->children);
    for(; ch; ch = g_list_next(ch))
        ((MotoTask *)ch->data)->deps_num--;
    g_list_free(self->children);

    if(self->free_data_func)
        self->free_data_func(self, self->data);
    g_slice_free(MotoTask, self);
}

void moto_task_add_child(MotoTask *self, MotoTask *child)
{
    child->deps_num++;
    self->children = g_list_append(self->children, child);
}

/* MotoTaskQueue */

MotoTaskQueue *moto_task_queue_new()
{
    MotoTaskQueue *self = g_slice_new(MotoTaskQueue);

    self->tasks = g_queue_new();

    return self;
}

static void
__free_task(MotoTask *task, gpointer user_data)
{
    moto_task_free(task);
}

void moto_task_queue_free(MotoTaskQueue *self)
{
    g_queue_foreach(self->tasks, (GFunc)__free_task, NULL);
    g_queue_free(self->tasks);
    g_mutex_free(self->tasks_mutex);
    g_slice_free(MotoTaskQueue, self);
}

void moto_task_queue_push(MotoTaskQueue *self, MotoTask *task)
{
    g_mutex_lock(self->tasks_mutex);
    g_queue_push_head(self->tasks, task);
    g_mutex_unlock(self->tasks_mutex);
}

MotoTask *moto_task_queue_pop(MotoTaskQueue *self)
{
    g_mutex_lock(self->tasks_mutex);
    MotoTask *task = (MotoTask *)g_queue_pop_tail(self->tasks);
    g_mutex_unlock(self->tasks_mutex);
    return task;
}

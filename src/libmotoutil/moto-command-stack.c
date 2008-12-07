#include "moto-command-stack.h"

/* Utils */

static void
unref_command(gpointer data,
              gpointer user_data)
{
    g_object_unref(data);
}

/* MotoCommand */

typedef struct _MotoCommandPriv MotoCommandPriv;
#define MOTO_COMMAND_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_COMMAND, MotoCommandPriv)

static GObjectClass *command_parent_class = NULL;

struct _MotoCommandPriv
{
    gboolean disposed;

    GString *name;
};

static void
moto_command_dispose(GObject *obj)
{
    MotoCommandPriv *priv = MOTO_COMMAND_GET_PRIVATE(obj);

    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    g_string_free(priv->name, TRUE);

    command_parent_class->dispose(obj);
}

static void
moto_command_finalize(GObject *obj)
{
   command_parent_class->finalize(obj);
}

static void
moto_command_init(MotoCommand *self)
{
    MotoCommandPriv *priv = MOTO_COMMAND_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->name = g_string_new("NoName");
}

static void
moto_command_class_init(MotoCommandClass *klass)
{
    command_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose  = moto_command_dispose;
    gobject_class->finalize = moto_command_finalize;

    klass->do_command = NULL;
    klass->undo_command = NULL;

    g_type_class_add_private(klass, sizeof(MotoCommandPriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoCommand, moto_command, G_TYPE_OBJECT);

/* Methods of class MotoCommand */

gchar *moto_command_get_name(MotoCommand *self)
{
    MotoCommandPriv *priv = MOTO_COMMAND_GET_PRIVATE(self);
    return priv->name->str;
}

gboolean moto_command_do(MotoCommand *self)
{
    MotoCommandClass *klass = MOTO_COMMAND_GET_CLASS(self);

    if(klass->do_command)
        return klass->do_command(self);

    return FALSE;
}

gboolean moto_command_undo(MotoCommand *self)
{
    MotoCommandClass *klass = MOTO_COMMAND_GET_CLASS(self);

    if(klass->undo_command)
        return klass->undo_command(self);

    return FALSE;
}

/* MotoCommandTransaction */

typedef struct _MotoCommandTransaction MotoCommandTransaction;

struct _MotoCommandTransaction
{
    GList *begin;
    GList *end;

    GList *commands;
};

void moto_command_transaction_init(MotoCommandTransaction *self)
{
    self->commands = NULL;
}

MotoCommandTransaction *moto_command_transaction_new(void)
{
    MotoCommandTransaction *self = g_slice_new(MotoCommandTransaction);
    moto_command_transaction_init(self);
    return self;
}

void moto_command_transaction_free(MotoCommandTransaction *self)
{
    g_list_foreach(self->commands,
                   unref_command, NULL);
    g_list_free(self->commands);

    g_slice_free(MotoCommandTransaction, self);
}

void moto_command_transaction_next_command(MotoCommandTransaction *self,
                                           MotoCommand *command)
{
    moto_command_do(command);
    self->commands = g_list_append(self->commands, command);
}

void moto_command_transaction_do(MotoCommandTransaction *self)
{
    GList *l = g_list_first(self->commands);
    for(; l; l = g_list_next(l))
        moto_command_do(MOTO_COMMAND(l->data));
}

void moto_command_transaction_undo(MotoCommandTransaction *self)
{
    GList *l = g_list_last(self->commands);
    for(; l; l = g_list_previous(l))
        moto_command_undo(MOTO_COMMAND(l->data));
}

/* MotoCommandStack */

typedef struct _MotoCommandStackPriv MotoCommandStackPriv;
#define MOTO_COMMAND_STACK_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_COMMAND_STACK, MotoCommandStackPriv)

static GObjectClass *stack_parent_class = NULL;

struct _MotoCommandStackPriv
{
    gboolean disposed;

    // Transactions
    GList *undoable;
    GList *redoable;

    MotoCommandTransaction *current_transaction;
};

static void
moto_command_stack_dispose(GObject *obj)
{
    MotoCommandStackPriv *priv = MOTO_COMMAND_STACK_GET_PRIVATE(obj);

    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    g_list_foreach(priv->undoable,
                   unref_command, NULL);
    g_list_free(priv->undoable);
    g_list_foreach(priv->redoable,
                   unref_command, NULL);
    g_list_free(priv->redoable);

    stack_parent_class->dispose(obj);
}

static void
moto_command_stack_finalize(GObject *obj)
{
   stack_parent_class->finalize(obj);
}

static void
moto_command_stack_init(MotoCommandStack *self)
{
    MotoCommandStackPriv *priv = MOTO_COMMAND_STACK_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->undoable = NULL;
    priv->redoable = NULL;

    priv->current_transaction = NULL;
}

static void
moto_command_stack_class_init(MotoCommandStackClass *klass)
{
    stack_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose  = moto_command_stack_dispose;
    gobject_class->finalize = moto_command_stack_finalize;

    g_type_class_add_private(klass, sizeof(MotoCommandStackPriv));
}

G_DEFINE_TYPE(MotoCommandStack, moto_command_stack, G_TYPE_OBJECT);

/* Methods of class MotoCommandStack */

MotoCommandStack *moto_command_stack_new(void)
{
    return (MotoCommandStack *)g_object_new(MOTO_TYPE_COMMAND_STACK, NULL);
}

void moto_command_stack_do(MotoCommandStack *self,
                           MotoCommand *command)
{
    MotoCommandStackPriv *priv = MOTO_COMMAND_STACK_GET_PRIVATE(self);

    moto_command_do(command);
    priv->undoable = \
        g_list_append(priv->undoable, command);

    /* Free redo list */
    g_list_foreach(priv->redoable,
                   unref_command, NULL);
    g_list_free(priv->redoable);
    priv->redoable = NULL;
}

void moto_command_stack_undo(MotoCommandStack *self)
{
    MotoCommandStackPriv *priv = MOTO_COMMAND_STACK_GET_PRIVATE(self);

    GList *command = g_list_last(priv->undoable);

    if(command)
    {
        moto_command_undo(MOTO_COMMAND(command->data));
        priv->redoable = \
            g_list_append(priv->redoable, command->data);
        priv->undoable = g_list_remove(priv->undoable, command->data);
    }
}

void moto_command_stack_redo(MotoCommandStack *self)
{
    MotoCommandStackPriv *priv = MOTO_COMMAND_STACK_GET_PRIVATE(self);

    GList *command = g_list_last(priv->redoable);

    if(command)
    {
        moto_command_do(MOTO_COMMAND(command->data));

        priv->undoable =\
            g_list_append(priv->undoable, command->data);
        priv->redoable = g_list_remove(priv->redoable, command->data);
    }
}

void moto_command_stack_begin(MotoCommandStack *self)
{
    MotoCommandStackPriv *priv = MOTO_COMMAND_STACK_GET_PRIVATE(self);

    // Makes autocommit.
    priv->current_transaction = moto_command_transaction_new();
}

void moto_command_stack_rollback(MotoCommandStack *self)
{
    MotoCommandStackPriv *priv = MOTO_COMMAND_STACK_GET_PRIVATE(self);

    if(priv->current_transaction)
    {
        moto_command_transaction_undo(priv->current_transaction);
        moto_command_transaction_free(priv->current_transaction);
        priv->current_transaction = NULL;
    }
}

void moto_command_stack_commit(MotoCommandStack *self)
{
    MotoCommandStackPriv *priv = MOTO_COMMAND_STACK_GET_PRIVATE(self);
    priv->current_transaction = NULL;
}


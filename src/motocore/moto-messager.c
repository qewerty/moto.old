#include "moto-messager.h"

/* class Messager */

static GObjectClass *messager_parent_class = NULL;

struct _MotoMessagerPriv
{
    guint msg_count;
    guint info_count;
    guint warning_count;
    guint error_count;
};

static void info_inc()
{
    MotoMessager *msger = moto_messager_singleton();
    msger->priv->msg_count++;
    msger->priv->info_count++;
}

static void warning_inc()
{
    MotoMessager *msger = moto_messager_singleton();
    msger->priv->msg_count++;
    msger->priv->warning_count++;
}

static void error_inc()
{
    MotoMessager *msger = moto_messager_singleton();
    msger->priv->msg_count++;
    msger->priv->error_count++;
}

static void
moto_messager_dispose(GObject *obj)
{
    MotoMessager *self = (MotoMessager *)obj;

    g_slice_free(MotoMessagerPriv, self->priv);

    messager_parent_class->dispose(obj);
}

static void
moto_messager_finalize(GObject *obj)
{
    messager_parent_class->finalize(obj);
}

static void
moto_messager_init(MotoMessager *self)
{
    self->priv = g_slice_new(MotoMessagerPriv);

    self->priv->msg_count       = 0;
    self->priv->info_count      = 0;
    self->priv->warning_count   = 0;
    self->priv->error_count     = 0;
}

static void
moto_messager_class_init(MotoMessagerClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    messager_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose = moto_messager_dispose;
    goclass->finalize = moto_messager_finalize;

    klass->info_message_signal_id = g_signal_newv ("info-message",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->warning_message_signal_id = g_signal_newv ("warning-message",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->error_message_signal_id = g_signal_newv ("error-message",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

}

G_DEFINE_TYPE(MotoMessager, moto_messager, G_TYPE_OBJECT);

static MotoMessager *messager = NULL;

MotoMessager *moto_messager_singleton()
{
    if(messager == NULL)
        messager = (MotoMessager *)g_object_new(MOTO_TYPE_MESSAGER, NULL);

    return messager;
}

void moto_info(const gchar *msg)
{
    g_print("Info: %s\n", msg);
    info_inc();

    MotoMessager *self = moto_messager_singleton();
    MotoMessagerClass *klass = MOTO_MESSAGER_GET_CLASS(self);
    g_signal_emit(self, klass->info_message_signal_id, 0, NULL);
}

void moto_warning(const gchar *msg)
{
    g_print("Warning: %s\n", msg);
    warning_inc();

    MotoMessager *self = moto_messager_singleton();
    MotoMessagerClass *klass = MOTO_MESSAGER_GET_CLASS(self);
    g_signal_emit(self, klass->warning_message_signal_id, 0, NULL);
}

void moto_error(const gchar *msg)
{
    g_print("Error: %s\n", msg);
    error_inc();

    MotoMessager *self = moto_messager_singleton();
    MotoMessagerClass *klass = MOTO_MESSAGER_GET_CLASS(self);
    g_signal_emit(self, klass->error_message_signal_id, 0, NULL);
}

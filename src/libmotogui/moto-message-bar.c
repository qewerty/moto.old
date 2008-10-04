#include "moto-message-bar.h"

/* class MotoMessageBar */

static GObjectClass *message_bar_parent_class = NULL;

struct _MotoMessageBarPriv
{
    gboolean disposed;

    MotoMessager *messager;
};

static void
moto_message_bar_dispose(GObject *obj)
{
    MotoMessageBar *self = (MotoMessageBar *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    message_bar_parent_class->dispose(obj);
}

static void
moto_message_bar_finalize(GObject *obj)
{
    g_slice_free(MotoMessageBarPriv, ((MotoMessageBar *)obj)->priv);
    message_bar_parent_class->finalize(obj);
}

static void
moto_message_bar_init(MotoMessageBar *self)
{
    self->priv = g_slice_new(MotoMessageBarPriv);
    self->priv->disposed = FALSE;

    gtk_widget_set_size_request((GtkWidget *)self, 100, 22);

    self->priv->messager = NULL;
}

static void
moto_message_bar_class_init(MotoMessageBarClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    message_bar_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_message_bar_dispose;
    goclass->finalize   = moto_message_bar_finalize;
}

G_DEFINE_TYPE(MotoMessageBar, moto_message_bar, GTK_TYPE_STATUSBAR);

static void on_info_message(MotoMessager *messager, const gchar *msg, MotoMessageBar *msgbar)
{
    GString *msg_str = g_string_new("[Info] ");
    g_string_append(msg_str, msg);
    gtk_statusbar_push((GtkStatusbar *)msgbar, 0, msg_str->str);
    g_string_free(msg_str, TRUE);
}

static void on_warning_message(MotoMessager *messager, const gchar *msg, MotoMessageBar *msgbar)
{
    GString *msg_str = g_string_new("[Warning] ");
    g_string_append(msg_str, msg);
    gtk_statusbar_push((GtkStatusbar *)msgbar, 0, msg_str->str);
    g_string_free(msg_str, TRUE);
}

static void on_error_message(MotoMessager *messager, const gchar *msg, MotoMessageBar *msgbar)
{
    GString *msg_str = g_string_new("[Error] ");
    g_string_append(msg_str, msg);
    gtk_statusbar_push((GtkStatusbar *)msgbar, 0, msg_str->str);
    g_string_free(msg_str, TRUE);
}

GtkWidget *moto_message_bar_new(MotoMessager *messager)
{
    MotoMessageBar *self = (MotoMessageBar *)g_object_new(MOTO_TYPE_MESSAGE_BAR, NULL);

    if(messager)
    {
        self->priv->messager = messager;

        g_signal_connect(G_OBJECT(messager), "info-message", G_CALLBACK(on_info_message), self);
        g_signal_connect(G_OBJECT(messager), "warning-message", G_CALLBACK(on_warning_message), self);
        g_signal_connect(G_OBJECT(messager), "error-message", G_CALLBACK(on_error_message), self);

        moto_messager_set_print_messages(FALSE);
    }

    moto_info("OK");

    return (GtkWidget *)self;
}

#include "moto-param-editor.h"

/* class MotoParamEditor */

static GObjectClass *param_editor_parent_class = NULL;

struct _MotoParamEditorPriv
{
    gboolean disposed;

    MotoNode *node;
    guint num;
};

static void
moto_param_editor_dispose(GObject *obj)
{
    MotoParamEditor *self = (MotoParamEditor *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    param_editor_parent_class->dispose(obj);
}

static void
moto_param_editor_finalize(GObject *obj)
{
    g_slice_free(MotoParamEditorPriv, ((MotoParamEditor *)obj)->priv);
    param_editor_parent_class->finalize(obj);
}

static void
moto_param_editor_init(MotoParamEditor *self)
{
    self->priv = g_slice_new(MotoParamEditorPriv);
    self->priv->disposed = FALSE;

    gtk_widget_set_size_request((GtkWidget *)self, 240, 36);
    gtk_table_set_homogeneous((GtkTable *)self, FALSE);

    self->priv->node = NULL;
    self->priv->num = 0;
}

static void
moto_param_editor_class_init(MotoParamEditorClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    param_editor_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_param_editor_dispose;
    goclass->finalize   = moto_param_editor_finalize;
}

G_DEFINE_TYPE(MotoParamEditor, moto_param_editor, GTK_TYPE_TABLE);

GtkWidget *moto_param_editor_new()
{
    MotoParamEditor *self = (MotoParamEditor *)g_object_new(MOTO_TYPE_PARAM_EDITOR, NULL);

    return (GtkWidget *)self;
}

static void node_delete_notify(MotoParamEditor *pe, GObject *where_the_object_was)
{
    moto_param_editor_update(pe, NULL);
}

static GtkWidget *create_widget_for_param(MotoParam *param)
{
    GtkWidget *widget = NULL;
    switch(moto_param_get_value_type(param))
    {
        case G_TYPE_INT:
            widget = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
            gtk_entry_set_editable((GtkEntry *)widget, TRUE);
            gtk_spin_button_set_value((GtkSpinButton *)widget, moto_param_get_int(param));
        break;
        case G_TYPE_UINT:
            widget = gtk_spin_button_new_with_range(0, 1000000, 1);
            gtk_entry_set_editable((GtkEntry *)widget, TRUE);
            gtk_spin_button_set_value((GtkSpinButton *)widget, moto_param_get_uint(param));
        break;
        case G_TYPE_BOOLEAN:
            widget = gtk_check_button_new();
            gtk_toggle_button_set_active((GtkToggleButton *)widget, moto_param_get_boolean(param));
        break;
        case G_TYPE_FLOAT:
            widget = gtk_spin_button_new_with_range(-1000000, 1000000, 0.01);
            gtk_entry_set_editable((GtkEntry *)widget, TRUE);
            gtk_spin_button_set_value((GtkSpinButton *)widget, moto_param_get_float(param));
        break;
    }
    return widget;
}

static void add_param_widget(MotoNode *node, MotoParam *param, MotoParamEditor *pe)
{
    // g_print("pname: %s\n", moto_param_get_name(param));

    GtkWidget *label = gtk_label_new(moto_param_get_title(param));
    gtk_label_set_selectable((GtkLabel *)label, TRUE);
    gtk_misc_set_alignment((GtkMisc *)label, 1, 0.5);
    // gtk_widget_set_size_request(label, 240, 36);
    // gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_RIGHT);
    // gtk_label_set_max_width_chars((GtkLabel *)label, 40);

    gtk_table_attach((GtkTable *)pe,
            label,
            0, 1, pe->priv->num, pe->priv->num + 1,
            GTK_SHRINK | GTK_FILL, GTK_SHRINK, 4, 0);

    GtkWidget *pwidget = create_widget_for_param(param);
    if(pwidget)
    {
        gtk_table_attach((GtkTable *)pe,
                pwidget,
                1, 2, pe->priv->num, pe->priv->num + 1,
                GTK_EXPAND | GTK_FILL, GTK_SHRINK, 2, 0);
    }

    pe->priv->num++;
}

void moto_param_editor_update(MotoParamEditor *self, MotoNode *node)
{
    if(node == self->priv->node)
        return;

    if(self->priv->node)
    {
        g_object_weak_unref(G_OBJECT(self->priv->node), (GWeakNotify)node_delete_notify, self);
        self->priv->node = NULL;
        self->priv->num = 0;

        GList *ch = gtk_container_get_children(GTK_CONTAINER(self));
        for(; ch; ch = g_list_next(ch))
            gtk_container_remove((GtkContainer *)self, (GtkWidget *)ch->data);
    }
    if( ! node)
        return;

    g_object_weak_ref(G_OBJECT(node), (GWeakNotify)node_delete_notify, self);
    self->priv->node = node;

    moto_node_foreach_param(node, (MotoNodeForeachParamFunc)add_param_widget, self);

    gtk_widget_show_all((GtkWidget *)self);
}

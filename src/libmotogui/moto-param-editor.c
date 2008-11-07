#include "libmoto/moto-filename.h"
#include "moto-param-editor.h"

/*  */

static GtkMenuBar *create_menu_bar(MotoParamEditor *pe);

static void __moto_param_editor_update(MotoParamEditor *self, MotoNode *node);

static void __on_button_prev(GtkButton* button, MotoParamEditor *tb);
static void __on_button_next(GtkButton* button, MotoParamEditor *tb);
static void __on_node_destroy(MotoParamEditor *pe, MotoNode *where_the_object_was);

/* class MotoParamEditor */

static GObjectClass *param_editor_parent_class = NULL;

struct _MotoParamEditorPriv
{
    gboolean disposed;

    GtkBox *box;
    GtkBox *gbox;
    GtkMenuBar *menu_bar;
    GtkBox *bbox;

    MotoTestWindow *window;
    GtkWidget* button_prev;
    GtkWidget* button_next;
    guint num;
    guint gnum;

    MotoNode *node;
    GList *prev_nodes;
    GList *next_nodes;
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

    self->priv->box         = (GtkBox *)gtk_vbox_new(FALSE, 0);
    self->priv->menu_bar    = create_menu_bar(self);
    self->priv->gbox        = (GtkBox *)gtk_vbox_new(FALSE, 0);
    self->priv->bbox        = (GtkBox *)gtk_hbox_new(FALSE, 0);

    GtkWidget *button_back = self->priv->button_prev = \
        (GtkWidget *)gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
    GtkWidget *button_forw = self->priv->button_next = \
        (GtkWidget *)gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
    gtk_widget_set_sensitive(self->priv->button_prev, FALSE);
    gtk_widget_set_sensitive(self->priv->button_next, FALSE);

    g_signal_connect(G_OBJECT(button_back), "clicked", G_CALLBACK(__on_button_prev), self);
    g_signal_connect(G_OBJECT(button_forw), "clicked", G_CALLBACK(__on_button_next), self);

    GtkWidget *button_in    = (GtkWidget *)gtk_tool_button_new(NULL, "IN");
    GtkWidget *button_out   = (GtkWidget *)gtk_tool_button_new(NULL, "OUT");
    GtkWidget *button_inout = (GtkWidget *)gtk_tool_button_new(NULL, "INOUT");
    gtk_widget_set_sensitive(button_in, FALSE);
    gtk_widget_set_sensitive(button_out, FALSE);
    gtk_widget_set_sensitive(button_inout, FALSE);

    gtk_box_pack_start(self->priv->bbox, button_in, FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_out, FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_inout, FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_back, FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_forw, FALSE, FALSE, 0);

    gtk_container_add((GtkContainer *)self, (GtkWidget *)self->priv->box);
    gtk_box_pack_start(self->priv->box,     (GtkWidget *)self->priv->menu_bar,  FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->box,     (GtkWidget *)self->priv->bbox,     FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->box,     (GtkWidget *)gtk_hseparator_new(),  FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->box,     (GtkWidget *)self->priv->gbox,     TRUE, TRUE, 4);

    self->priv->window = NULL;
    self->priv->num = 0;
    self->priv->gnum = 0;

    self->priv->node = NULL;
    self->priv->prev_nodes = NULL;
    self->priv->next_nodes = NULL;
}

static void
moto_param_editor_class_init(MotoParamEditorClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    param_editor_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_param_editor_dispose;
    goclass->finalize   = moto_param_editor_finalize;
}

G_DEFINE_TYPE(MotoParamEditor, moto_param_editor, GTK_TYPE_VBOX);

GtkWidget *moto_param_editor_new(MotoTestWindow *window)
{
    MotoParamEditor *self = (MotoParamEditor *)g_object_new(MOTO_TYPE_PARAM_EDITOR, NULL);

    self->priv->window = window;

    return (GtkWidget *)self;
}

typedef struct _OnChangedData
{
    MotoParam *param;
    MotoTestWindow *window;
    GtkWidget *widget;
    gulong handler_id;
    gulong param_handler_id;
} OnChangedData;

static void node_delete_notify(MotoParamEditor *pe, GObject *where_the_object_was)
{
    moto_param_editor_set_node(pe, NULL);
}

static void widget_delete_notify(OnChangedData *data, GObject *where_the_object_was)
{
    if(data->param_handler_id)
        g_signal_handler_disconnect(data->param, data->param_handler_id);
    g_slice_free(OnChangedData, data);
}

void on_float_changed(GtkSpinButton *spinbutton,
                      OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    moto_param_set_float(data->param, value);
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_float_param_changed(MotoParam *param, OnChangedData *data)
{
    g_signal_handler_block(data->widget, data->handler_id);
    gtk_spin_button_set_value((GtkSpinButton *)data->widget, moto_param_get_float(param));
    g_signal_handler_unblock(data->widget, data->handler_id);
}

void on_int_changed(GtkSpinButton *spinbutton,
                    OnChangedData *data)
{
    gint value = gtk_spin_button_get_value_as_int(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    moto_param_set_int(data->param, value);
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_int_param_changed(MotoParam *param, OnChangedData *data)
{
    g_signal_handler_block(data->widget, data->handler_id);
    gtk_spin_button_set_value((GtkSpinButton *)data->widget, moto_param_get_int(param));
    g_signal_handler_unblock(data->widget, data->handler_id);
}

void on_uint_changed(GtkSpinButton *spinbutton,
                     OnChangedData *data)
{
    guint value = gtk_spin_button_get_value_as_int(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    moto_param_set_uint(data->param, value);
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_uint_param_changed(MotoParam *param, OnChangedData *data)
{
    g_signal_handler_block(data->widget, data->handler_id);
    gtk_spin_button_set_value((GtkSpinButton *)data->widget, moto_param_get_uint(param));
    g_signal_handler_unblock(data->widget, data->handler_id);
}

void on_boolean_changed(GtkToggleButton *togglebutton,
                        OnChangedData *data)
{
    gboolean value = gtk_toggle_button_get_active(togglebutton);

    g_signal_handler_block(togglebutton, data->handler_id);
    moto_param_set_boolean(data->param, value);
    g_signal_handler_unblock(togglebutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_boolean_param_changed(MotoParam *param, OnChangedData *data)
{
    g_signal_handler_block(data->widget, data->handler_id);
    gtk_toggle_button_set_active((GtkToggleButton *)data->widget, moto_param_get_boolean(param));
    g_signal_handler_unblock(data->widget, data->handler_id);
}

void on_string_changed(GtkEditable *editable,
                        OnChangedData *data)
{
    const gchar* value = gtk_entry_get_text((GtkEntry *)editable);

    g_signal_handler_block(editable, data->handler_id);
    moto_param_set_string(data->param, value);
    g_signal_handler_unblock(editable, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

typedef struct _FilenameDialogData
{
    GtkEntry *entry;
} FilenameDialogData;

void on_filename_dialog_response(GtkDialog *dialog, gint response_id, FilenameDialogData *data)
{
    if(GTK_RESPONSE_OK == response_id)
    {
        gtk_entry_set_text(data->entry,
                gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
    }

    // gtk_object_destroy((GtkObject *)dialog);
    gtk_widget_hide(GTK_WIDGET(dialog));
}

void on_filename_clicked(GtkButton *button, gpointer user_data)
{
    static GtkFileChooserDialog *dialog = NULL;
    if( ! dialog)
    {
        dialog = (GtkFileChooserDialog *)gtk_file_chooser_dialog_new("Select a file",
                    NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                    GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                    NULL);
    }

    g_signal_connect(dialog, "response", G_CALLBACK(on_filename_dialog_response), user_data);

    gtk_widget_show_all((GtkWidget *)dialog);
}

void destroy_filename_data(gpointer data, GClosure *closure)
{
    g_slice_free(FilenameDialogData, data);
}

static GtkWidget *create_widget_for_param(MotoParamEditor *pe, MotoParam *param)
{
    GtkWidget *widget = NULL;
    OnChangedData *data;
    GType ptype = moto_param_get_value_type(param);
    if(G_TYPE_INT == ptype)
    {
        widget = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_spin_button_set_value((GtkSpinButton *)widget, moto_param_get_int(param));
        gtk_editable_set_editable((GtkEditable *)widget, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)widget, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = widget;
        g_object_weak_ref(G_OBJECT(widget), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(on_int_changed), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_int_param_changed), data);
    }
    else if(G_TYPE_UINT == ptype)
    {
        widget = gtk_spin_button_new_with_range(0, 1000000, 1);
        gtk_spin_button_set_value((GtkSpinButton *)widget, moto_param_get_uint(param));
        gtk_editable_set_editable((GtkEditable *)widget, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)widget, FALSE);

        GParamSpecUInt *ps = (GParamSpecUInt *)moto_param_get_spec(param);
        if(ps)
            gtk_spin_button_set_range((GtkSpinButton *)widget, ps->minimum, ps->maximum);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = widget;
        g_object_weak_ref(G_OBJECT(widget), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(on_uint_changed), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_uint_param_changed), data);
    }
    else if(G_TYPE_BOOLEAN == ptype)
    {
        widget = gtk_check_button_new();
        gtk_toggle_button_set_active((GtkToggleButton *)widget, moto_param_get_boolean(param));

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = widget;
        g_object_weak_ref(G_OBJECT(widget), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(widget), "toggled", G_CALLBACK(on_boolean_changed), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);
    }
    else if(G_TYPE_FLOAT == ptype)
    {
        widget = gtk_spin_button_new_with_range(-1000000, 1000000, 0.01);
        gtk_spin_button_set_value((GtkSpinButton *)widget, moto_param_get_float(param));
        gtk_editable_set_editable((GtkEditable *)widget, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)widget, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)widget, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = widget;
        g_object_weak_ref(G_OBJECT(widget), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(on_float_changed), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);
    }
    else if(G_TYPE_STRING == ptype)
    {
        widget = gtk_entry_new();
        gtk_entry_set_text((GtkEntry *)widget, moto_param_get_string(param));
        gtk_editable_set_editable((GtkEditable *)widget, TRUE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = widget;
        g_object_weak_ref(G_OBJECT(widget), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(widget), "changed", G_CALLBACK(on_string_changed), data);
        data->param_handler_id = 0;
        //     g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);
    }
    else if(MOTO_TYPE_FILENAME == ptype)
    {
        widget = gtk_hbox_new(FALSE, 0);
        GtkWidget *entry  = gtk_entry_new();
        GtkWidget *button = gtk_button_new();

        FilenameDialogData *fdd = g_slice_new(FilenameDialogData);
        fdd->entry = (GtkEntry *)entry;
        g_signal_connect_data(button, "clicked", G_CALLBACK(on_filename_clicked), fdd, 
                destroy_filename_data, G_CONNECT_AFTER);

        GtkWidget *im = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);
        gtk_button_set_image((GtkButton *)button, im);

        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_box_pack_start((GtkBox *)widget, button, FALSE, FALSE, 0);

        gtk_entry_set_text((GtkEntry *)entry, moto_param_get_string(param));
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(on_string_changed), data);
        data->param_handler_id = 0;
        //     g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);
    }
    return widget;
}

typedef struct _AddWidgetData
{
    MotoParamEditor *pe;
    GtkTable *table;
} AddWidgetData;

static void add_param_widget(MotoNode *node, const gchar *group, MotoParam *param, AddWidgetData *data)
{
    GtkWidget *label = gtk_label_new(moto_param_get_title(param));
    gtk_misc_set_alignment((GtkMisc *)label, 1, 0.5);

    gtk_table_attach((GtkTable *)data->table,
            label,
            0, 1, data->pe->priv->num, data->pe->priv->num + 1,
            GTK_FILL, GTK_SHRINK, 8, 1);

    GtkWidget *pwidget = create_widget_for_param(data->pe, param);
    if(pwidget)
    {
        gtk_table_attach((GtkTable *)data->table,
                pwidget,
                1, 2, data->pe->priv->num, data->pe->priv->num + 1,
                GTK_EXPAND | GTK_FILL, GTK_SHRINK, 2, 0);

        GtkWidget *button = gtk_button_new();
        gtk_button_set_relief((GtkButton *)button, GTK_RELIEF_NONE);
        GtkWidget *im = gtk_image_new_from_stock(GTK_STOCK_CONNECT, GTK_ICON_SIZE_MENU);
        gtk_button_set_image((GtkButton *)button, im);

        gtk_table_attach((GtkTable *)data->table,
                button,
                2, 3, data->pe->priv->num, data->pe->priv->num + 1,
                GTK_SHRINK, GTK_SHRINK, 0, 0);
    }

    data->pe->priv->num++;
}

static void make_group(MotoNode *node, const gchar *group, MotoParamEditor *pe)
{
    GtkExpander *exp = (GtkExpander *)gtk_expander_new(group);
    if(pe->priv->gnum < 4)
        gtk_expander_set_expanded(exp, TRUE);

    GtkTable *table = (GtkTable *)gtk_table_new(5, 3, FALSE);
    gtk_container_add((GtkContainer *)exp, (GtkWidget *)table);
    gtk_box_pack_start(pe->priv->gbox, (GtkWidget *)exp, FALSE, FALSE, 2);

    AddWidgetData data = {pe, table};
    moto_node_foreach_param_in_group(node, group, (MotoNodeForeachParamInGroupFunc)add_param_widget, & data);

    pe->priv->gnum++;
}

void moto_param_editor_set_node(MotoParamEditor *self, MotoNode *node)
{
    if(self->priv->node)
    {
        self->priv->prev_nodes = g_list_prepend(self->priv->prev_nodes, self->priv->node);
        gtk_widget_set_sensitive(self->priv->button_prev, TRUE);
    }

    if( ! g_list_find(self->priv->prev_nodes, node) && ! g_list_find(self->priv->next_nodes, node))
    {
        g_object_weak_ref(G_OBJECT(node), (GWeakNotify)__on_node_destroy, self);
    }

    g_list_free(self->priv->next_nodes);
    self->priv->next_nodes = NULL;
    gtk_widget_set_sensitive(self->priv->button_next, FALSE);

    __moto_param_editor_update(self, node);
}

static void __moto_param_editor_update(MotoParamEditor *self, MotoNode *node)
{
    if(node == self->priv->node)
        return;

    if(self->priv->node)
    {
        g_object_weak_unref(G_OBJECT(self->priv->node), (GWeakNotify)node_delete_notify, self);
        self->priv->node = NULL;
        self->priv->num = 0;
        self->priv->gnum = 0;

        GList *ch = gtk_container_get_children(GTK_CONTAINER(self->priv->gbox));
        for(; ch; ch = g_list_next(ch))
            gtk_container_remove((GtkContainer *)self->priv->gbox, (GtkWidget *)ch->data);
    }
    if( ! node)
        return;

    g_object_weak_ref(G_OBJECT(node), (GWeakNotify)node_delete_notify, self);
    self->priv->node = node;

    moto_node_foreach_group(node, (MotoNodeForeachGroupFunc)make_group, self);

    gtk_widget_show_all((GtkWidget *)self);
}

static GtkMenuBar *create_menu_bar(MotoParamEditor *pe)
{
    GtkMenuBar *mb = (GtkMenuBar *)gtk_menu_bar_new();
    GtkMenu *menu;
    GtkWidget *item;

    menu = (GtkMenu *)gtk_menu_new();
    item = gtk_menu_item_new_with_label("Go");
    gtk_menu_item_set_submenu((GtkMenuItem *)item, (GtkWidget *)menu);
    gtk_menu_shell_append((GtkMenuShell *)mb, (GtkWidget *)item);

    item = gtk_image_menu_item_new_from_stock(GTK_STOCK_GO_BACK, NULL);
    gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);
    item = gtk_image_menu_item_new_from_stock(GTK_STOCK_GO_FORWARD, NULL);
    gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

    menu = (GtkMenu *)gtk_menu_new();
    item = gtk_menu_item_new_with_label("View");
    gtk_menu_item_set_submenu((GtkMenuItem *)item, (GtkWidget *)menu);
    gtk_menu_shell_append((GtkMenuShell *)mb, (GtkWidget *)item);

    item = gtk_menu_item_new_with_label("INOUT");
    gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);
    item = gtk_menu_item_new_with_label("Only IN");
    gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);
    item = gtk_menu_item_new_with_label("Only OUT");
    gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

    menu = (GtkMenu *)gtk_menu_new();
    item = gtk_menu_item_new_with_label("Param");
    gtk_menu_item_set_submenu((GtkMenuItem *)item, (GtkWidget *)menu);
    gtk_menu_shell_append((GtkMenuShell *)mb, (GtkWidget *)item);

    item = gtk_menu_item_new_with_label("Add new");
    gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

    return mb;
}

gboolean moto_param_editor_has_prev_node(MotoParamEditor *self)
{
    return NULL != self->priv->prev_nodes;
}

gboolean moto_param_editor_has_next_node(MotoParamEditor *self)
{
    return NULL != self->priv->next_nodes;
}

void moto_param_editor_goto_prev_node(MotoParamEditor *self)
{
    GList *first = g_list_first(self->priv->prev_nodes);
    if( ! first)
        return;

    MotoNode *node = MOTO_NODE(first->data);

    self->priv->prev_nodes = g_list_delete_link(self->priv->prev_nodes, first);
    if(self->priv->node)
        self->priv->next_nodes = g_list_prepend(self->priv->next_nodes, self->priv->node);

    if( ! moto_param_editor_has_prev_node(self))
        gtk_widget_set_sensitive(self->priv->button_prev, FALSE);
    gtk_widget_set_sensitive(self->priv->button_next, TRUE);

    __moto_param_editor_update(self, node);
}

void moto_param_editor_goto_next_node(MotoParamEditor *self)
{
    GList *first = g_list_first(self->priv->next_nodes);
    if( ! first)
        return;

    MotoNode *node = MOTO_NODE(first->data);

    self->priv->next_nodes = g_list_delete_link(self->priv->next_nodes, first);
    if(self->priv->node)
        self->priv->prev_nodes = g_list_prepend(self->priv->prev_nodes, self->priv->node);

    if( ! moto_param_editor_has_next_node(self))
        gtk_widget_set_sensitive(self->priv->button_next, FALSE);
    gtk_widget_set_sensitive(self->priv->button_prev, TRUE);

    __moto_param_editor_update(self, node);
}

void moto_param_editor_goto_first_node(MotoParamEditor *self)
{
    if( ! moto_param_editor_has_prev_node(self))
        return;
}

void moto_param_editor_goto_last_node(MotoParamEditor *self)
{
    if( ! moto_param_editor_has_next_node(self))
        return;
}

static void __on_button_prev(GtkButton* button, MotoParamEditor *tb)
{
    moto_param_editor_goto_prev_node(tb);
}

static void __on_button_next(GtkButton* button, MotoParamEditor *tb)
{
    moto_param_editor_goto_next_node(tb);
}

static void __on_node_destroy(MotoParamEditor *pe, MotoNode *where_the_object_was)
{
    pe->priv->prev_nodes = g_list_remove_all(pe->priv->prev_nodes, where_the_object_was);
    pe->priv->next_nodes = g_list_remove_all(pe->priv->next_nodes, where_the_object_was);

    if(pe->priv->node == where_the_object_was)
    {
        if(moto_param_editor_has_prev_node(pe))
        {
            moto_param_editor_goto_prev_node(pe);
        }
        else
        {
            moto_param_editor_goto_next_node(pe);
        }
    }
}

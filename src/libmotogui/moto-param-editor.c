#include <gdk/gdkkeysyms.h>

#include "libmoto/moto-types.h"
#include "libmoto/moto-param-spec.h"
#include "libmoto/moto-filename.h"
#include "moto-param-editor.h"

/*  */

static GtkMenuBar *create_menu_bar(MotoParamEditor *pe);

static void __moto_param_editor_update_full(MotoParamEditor *self, MotoNode *node, gboolean force);
#define __moto_param_editor_update(self, node) __moto_param_editor_update_full(self, node, FALSE);

static void __moto_param_editor_set_node_buttons(MotoParamEditor *self, gboolean sensitive);
static void __moto_param_editor_set_param_mode(MotoParamEditor *self, MotoParamMode mode);

static void __on_button_prev(GtkButton* button, MotoParamEditor *pe);
static void __on_button_next(GtkButton* button, MotoParamEditor *pe);
static void __on_node_destroy(MotoParamEditor *pe, MotoNode *where_the_object_was);

static void __on_button_in(GtkButton* button, MotoParamEditor *pe);
static void __on_button_out(GtkButton* button, MotoParamEditor *pe);
static void __on_button_inout(GtkButton* button, MotoParamEditor *pe);
static void __on_button_all(GtkButton* button, MotoParamEditor *pe);

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

    GtkWidget *button_in;
    GtkWidget *button_out;
    GtkWidget *button_inout;
    GtkWidget *button_all;

    MotoNode *node;
    GList *prev_nodes;
    GList *next_nodes;

    MotoParamMode param_mode;
    gboolean show_all;
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

    GtkWidget *button_in    = self->priv->button_in    = (GtkWidget *)gtk_tool_button_new(NULL, "IN");
    GtkWidget *button_out   = self->priv->button_out   = (GtkWidget *)gtk_tool_button_new(NULL, "OUT");
    GtkWidget *button_inout = self->priv->button_inout = (GtkWidget *)gtk_tool_button_new(NULL, "INOUT");
    GtkWidget *button_all   = self->priv->button_all   = (GtkWidget *)gtk_tool_button_new(NULL, "ALL");
    gtk_widget_set_sensitive(button_in,    FALSE);
    gtk_widget_set_sensitive(button_out,   FALSE);
    gtk_widget_set_sensitive(button_inout, FALSE);
    gtk_widget_set_sensitive(button_all,   FALSE);
    g_signal_connect(G_OBJECT(button_in),    "clicked", G_CALLBACK(__on_button_in),    self);
    g_signal_connect(G_OBJECT(button_out),   "clicked", G_CALLBACK(__on_button_out),   self);
    g_signal_connect(G_OBJECT(button_inout), "clicked", G_CALLBACK(__on_button_inout), self);
    g_signal_connect(G_OBJECT(button_all),   "clicked", G_CALLBACK(__on_button_all),   self);

    gtk_box_pack_start(self->priv->bbox, button_back,  FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_forw,  FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_in,    FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_out,   FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_inout, FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->bbox, button_all,   FALSE, FALSE, 0);

    gtk_container_add((GtkContainer *)self, (GtkWidget *)self->priv->box);
    gtk_box_pack_start(self->priv->box,     (GtkWidget *)self->priv->menu_bar,  FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->box,     (GtkWidget *)self->priv->bbox,     FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->box,     gtk_hseparator_new(),  FALSE, FALSE, 0);
    gtk_box_pack_start(self->priv->box,     (GtkWidget *)self->priv->gbox,     TRUE, TRUE, 4);

    self->priv->window = NULL;
    self->priv->num = 0;
    self->priv->gnum = 0;

    self->priv->node = NULL;
    self->priv->prev_nodes = NULL;
    self->priv->next_nodes = NULL;

    self->priv->param_mode = MOTO_PARAM_MODE_IN;
    self->priv->show_all = FALSE;
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

// int2

void on_int_2_changed_0(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_2 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[0] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_int_2_changed_1(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_2 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[1] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

// int3

void on_int_3_changed_0(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[0] = (gint)value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_int_3_changed_1(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[1] = (gint)value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_int_3_changed_2(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[2] = (gint)value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

// int4

void on_int_4_changed_0(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_4 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[0] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_int_4_changed_1(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_4 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[1] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_int_4_changed_2(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_4 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[2] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_int_4_changed_3(GtkSpinButton *spinbutton,
                        OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_int_4 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gint *vec = g_value_peek_pointer(v);
    vec[3] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

// float

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

// float2

void on_float_2_changed_0(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[0] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_float_2_changed_1(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[1] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

// float3

void on_float_3_changed_0(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[0] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_float_3_changed_1(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[1] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_float_3_changed_2(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[2] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

// float4

void on_float_4_changed_0(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[0] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_float_4_changed_1(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[1] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_float_4_changed_2(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[2] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_float_4_changed_3(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gfloat value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_float_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gfloat *vec = g_value_peek_pointer(v);
    vec[3] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

// int

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

// boolean

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

// boolean2

void on_boolean_2_changed_0(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[0] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_boolean_2_changed_1(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[1] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

// boolean3

void on_boolean_3_changed_0(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[0] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_boolean_3_changed_1(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[1] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_boolean_3_changed_2(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[2] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

// boolean4

void on_boolean_4_changed_0(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[0] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_boolean_4_changed_1(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[1] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_boolean_4_changed_2(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[2] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

void on_boolean_4_changed_3(GtkSpinButton *spinbutton,
                          OnChangedData *data)
{
    gboolean value = gtk_spin_button_get_value(spinbutton);

    g_signal_handler_block(spinbutton, data->handler_id);
    // FIXME: Rewrite with moto_value_*et_boolean_3 when it will be implemented!
    GValue *v = moto_param_get_value(data->param);
    gboolean *vec = g_value_peek_pointer(v);
    vec[3] = value;
    g_signal_handler_unblock(spinbutton, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

static
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

static
void on_enum_changed(GtkComboBox *combo_box,
                     OnChangedData *data)
{
    gint value = gtk_combo_box_get_active((GtkEntry *)combo_box);

    g_signal_handler_block(combo_box, data->handler_id);
    moto_param_set_enum(data->param, value);
    g_signal_handler_unblock(combo_box, data->handler_id);

    moto_node_update(moto_param_get_node(data->param));
    moto_test_window_redraw_3dview(data->window);
}

static gboolean
on_expression_key_press_event(GtkEditable *editable,
                              GdkEventKey *event,
                              OnChangedData *data)
{
    if(GDK_Return == event->keyval)
    {
        const gchar* value = gtk_entry_get_text((GtkEntry *)editable);

        if(g_utf8_collate(value, moto_param_get_expression(data->param)) != 0)
        {
            moto_param_set_expression(data->param, value);
            if( ! moto_param_eval(data->param))
            {
                // FIXME: Replace with moto_param_get_full_name(...) when this function is implemented.
                moto_error("Expression of '%s.%s' parameter is invalid",
                    moto_node_get_name(moto_param_get_node(data->param)),
                    moto_param_get_name(data->param));
            }
            else
            {
                // FIXME: Replace with moto_param_get_full_name(...) when this function is implemented.
                moto_info("Expression of '%s.%s' parameter successfully evaluated",
                    moto_node_get_name(moto_param_get_node(data->param)),
                    moto_param_get_name(data->param));
            }

            moto_node_update(moto_param_get_node(data->param));
            moto_test_window_redraw_3dview(data->window);
        }
    }

    return FALSE;
}

static gboolean
on_expression_focus_out_event(GtkEditable *editable,
                              GdkEventFocus *event,
                              OnChangedData *data)
{
    // TODO: Add this parameter in global Moto configuration.
    gboolean accept_expr_on_focus_out = TRUE;
    if(accept_expr_on_focus_out)
    {
        const gchar* value = gtk_entry_get_text((GtkEntry *)editable);

        if(g_utf8_collate(value, moto_param_get_expression(data->param)) != 0)
        {
            moto_param_set_expression(data->param, value);
            if( ! moto_param_eval(data->param))
            {
                // FIXME: Replace with moto_param_get_full_name(...) when this function is implemented.
                moto_error("Expression of '%s.%s' parameter is invalid",
                    moto_node_get_name(moto_param_get_node(data->param)),
                    moto_param_get_name(data->param));
            }
            else
            {
                // FIXME: Replace with moto_param_get_full_name(...) when this function is implemented.
                moto_info("Expression of '%s.%s' parameter successfully evaluated",
                    moto_node_get_name(moto_param_get_node(data->param)),
                    moto_param_get_name(data->param));
            }

            moto_node_update(moto_param_get_node(data->param));
            moto_test_window_redraw_3dview(data->window);
        }
    }
    else
    {
        gtk_entry_set_text((GtkEntry *)editable, moto_param_get_expression(data->param));
    }

    return FALSE;
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

    if(moto_param_get_use_expression(param))
    {
        widget = gtk_entry_new();
        gtk_widget_add_events(widget, GDK_FOCUS_CHANGE_MASK);

        gtk_entry_set_text((GtkEntry *)widget, moto_param_get_expression(param));

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = widget;
        g_object_weak_ref(G_OBJECT(widget), (GWeakNotify)widget_delete_notify, data);
        g_signal_connect(G_OBJECT(widget), "key-press-event", G_CALLBACK(on_expression_key_press_event), data);
        data->handler_id = \
            g_signal_connect(G_OBJECT(widget), "focus-out-event", G_CALLBACK(on_expression_focus_out_event), data);
        data->param_handler_id = 0;

        return widget;
    }

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
        data->handler_id = \
            g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(on_int_changed), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_int_param_changed), data);
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
        MotoParamSpecFloat *pspec = MOTO_PARAM_SPEC_FLOAT(moto_param_get_spec(param));
        if(pspec)
            widget = gtk_spin_button_new_with_range(pspec->min, pspec->max, pspec->step);
        else
            widget = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
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
    else if(MOTO_TYPE_BOOLEAN_2 == ptype)
    {
        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_boolean_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gboolean *vec = (gboolean *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_2_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);

        // 2
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_2_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);
    }
    else if(MOTO_TYPE_BOOLEAN_3 == ptype)
    {
        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_boolean_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gboolean *vec = (gboolean *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_3_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);

        // 2
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_3_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);

        // 3
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[2]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_3_changed_2), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);
    }
    else if(MOTO_TYPE_BOOLEAN_4 == ptype)
    {
        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_boolean_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gboolean *vec = (gboolean *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_4_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);

        // 2
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_4_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);

        // 3
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[2]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_4_changed_2), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);

        // 4
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[3]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_boolean_4_changed_2), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_boolean_param_changed), data);
    }
    else if(MOTO_TYPE_INT_2 == ptype)
    {
        MotoParamSpecInt_2 *pspec = \
            MOTO_PARAM_SPEC_INT_2(moto_param_get_spec(param));

        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_float_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gint *vec = (gint *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[0], pspec->max[0], pspec->step[0]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_2_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 2
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[1], pspec->max[1], pspec->step[1]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_2_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);
    }
    else if(MOTO_TYPE_INT_3 == ptype)
    {
        MotoParamSpecInt_3 *pspec = \
            MOTO_PARAM_SPEC_INT_3(moto_param_get_spec(param));

        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_float_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gint *vec = (gint *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[0], pspec->max[0], pspec->step[0]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_3_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 2
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[1], pspec->max[1], pspec->step[1]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_3_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 3
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[2], pspec->max[2], pspec->step[2]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[2]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_3_changed_2), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);
    }
    else if(MOTO_TYPE_INT_4 == ptype)
    {
        MotoParamSpecInt_4 *pspec = \
            MOTO_PARAM_SPEC_INT_4(moto_param_get_spec(param));

        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_float_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gint *vec = (gint *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[0], pspec->max[0], pspec->step[0]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_4_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 2
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[1], pspec->max[1], pspec->step[1]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_4_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 3
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[2], pspec->max[2], pspec->step[2]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[2]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_4_changed_2), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 4
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[3], pspec->max[3], pspec->step[3]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[3]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_int_4_changed_3), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);
    }
    else if(MOTO_TYPE_FLOAT_2 == ptype)
    {
        MotoParamSpecInt_2 *pspec = \
            MOTO_PARAM_SPEC_FLOAT_2(moto_param_get_spec(param));

        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_float_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gfloat *vec = (gfloat *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[0], pspec->max[0], pspec->step[0]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_2_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 2
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[1], pspec->max[1], pspec->step[1]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_2_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);
    }
    else if(MOTO_TYPE_FLOAT_3 == ptype)
    {
        MotoParamSpecInt_3 *pspec = \
            MOTO_PARAM_SPEC_FLOAT_3(moto_param_get_spec(param));

        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_float_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gfloat *vec = (gfloat *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[0], pspec->max[0], pspec->step[0]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_3_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 2
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[1], pspec->max[1], pspec->step[1]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_3_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 3
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[2], pspec->max[2], pspec->step[2]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[2]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_3_changed_2), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);
    }
    else if(MOTO_TYPE_FLOAT_4 == ptype)
    {
        MotoParamSpecInt_4 *pspec = \
            MOTO_PARAM_SPEC_FLOAT_4(moto_param_get_spec(param));

        GtkWidget *entry;

        widget = gtk_hbox_new(TRUE, 0);

        // FIXME: Rewrite with moto_value_get_float_3 when it will be implemented!
        GValue *value = moto_param_get_value(param);
        gfloat *vec = (gfloat *)g_value_peek_pointer(value);

        gint w = 32, h = -1;

        // 1
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[0], pspec->max[0], pspec->step[0]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[0]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_4_changed_0), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 2
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[1], pspec->max[1], pspec->step[1]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[1]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_4_changed_1), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 3
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[2], pspec->max[2], pspec->step[2]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[2]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_4_changed_2), data);
        data->param_handler_id = \
            g_signal_connect(G_OBJECT(param), "value-changed", G_CALLBACK(on_float_param_changed), data);

        // 4
        if(pspec)
            entry = gtk_spin_button_new_with_range(pspec->min[3], pspec->max[3], pspec->step[3]);
        else
            entry = gtk_spin_button_new_with_range(-1000000, 1000000, 0.1);
        gtk_widget_set_size_request(entry, w, h);
        gtk_box_pack_start((GtkBox *)widget, entry, TRUE, TRUE, 0);
        gtk_spin_button_set_value((GtkSpinButton *)entry, vec[3]);
        gtk_editable_set_editable((GtkEditable *)entry, TRUE);
        gtk_spin_button_set_numeric((GtkSpinButton *)entry, FALSE);
        gtk_spin_button_set_digits((GtkSpinButton *)entry, 3);

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        data->widget = entry;
        g_object_weak_ref(G_OBJECT(entry), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(entry), "value-changed", G_CALLBACK(on_float_4_changed_2), data);
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
    else if(G_TYPE_IS_ENUM(ptype))
    {
        widget = gtk_combo_box_new_text();
        GtkComboBox *combo_box = (GtkComboBox*)widget;

        GEnumClass *ec = (GEnumClass *)g_type_class_ref(ptype);
        guint i;
        for(i = 0; i < ec->n_values; i++)
        {
            gtk_combo_box_append_text(combo_box, ec->values[i].value_nick);
        }
        // TODO: Reimplement. Enum value may not match combo box index.
        gtk_combo_box_set_active(combo_box, moto_param_get_enum(param));

        data = g_slice_new(OnChangedData);
        data->param = param;
        data->window = pe->priv->window;
        g_object_weak_ref(G_OBJECT(widget), (GWeakNotify)widget_delete_notify, data);
        data->handler_id = g_signal_connect(G_OBJECT(combo_box), "changed", G_CALLBACK(on_enum_changed), data);
        data->param_handler_id = 0;

        g_type_class_unref(ec);
    }
    return widget;
}

typedef struct _AddWidgetData
{
    MotoParamEditor *pe;
    GtkTable *table;
} AddWidgetData;

typedef struct _OnLabelButtonPressData
{
    MotoParamEditor *pe;
    MotoParam *param;
} OnLabelButtonPressData;

gboolean on_label_button_press_event(GtkLabel       *label,
                                     GdkEventButton *event,
                                     OnLabelButtonPressData *data)
{
    if(1 == event->button)
    {
        moto_param_set_use_expression(data->param, ! moto_param_get_use_expression(data->param));

        __moto_param_editor_update_full(data->pe, data->pe->priv->node, TRUE);
    }

    return TRUE;
}

static void add_param_widget(MotoNode *node, const gchar *group, MotoParam *param, AddWidgetData *data)
{
    if(moto_param_get_mode(param) != data->pe->priv->param_mode && ! data->pe->priv->show_all)
        return;

    const gchar *title = moto_param_get_title(param);
    GtkWidget *label = gtk_label_new(title);
    gtk_misc_set_alignment((GtkMisc *)label, 1, 0.5);
    if(moto_param_get_use_expression(param))
    {
        gchar *markup = g_markup_printf_escaped("<span color=\"#AA1100\">%s</span>", title);
        gtk_label_set_markup((GtkLabel *)label, markup);
        g_free(markup);
    }

    OnLabelButtonPressData *on_label_button_press_data = g_slice_new(OnLabelButtonPressData);
    on_label_button_press_data->param = param;
    on_label_button_press_data->pe = data->pe;

    gtk_label_set_selectable((GtkLabel *)label, TRUE);
    gtk_widget_add_events(label, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(label), "button-press-event",
        G_CALLBACK(on_label_button_press_event), on_label_button_press_data);


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

static void __moto_param_editor_update_full(MotoParamEditor *self, MotoNode *node, gboolean force)
{
    if(node == self->priv->node && ! force)
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
    {
        __moto_param_editor_set_node_buttons(self, FALSE);
        return;
    }
    __moto_param_editor_set_node_buttons(self, TRUE);

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

    menu = (GtkMenu *)gtk_menu_new();
    item = gtk_menu_item_new_with_label("Action");
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

static void __on_button_prev(GtkButton* button, MotoParamEditor *pe)
{
    moto_param_editor_goto_prev_node(pe);
}

static void __on_button_next(GtkButton* button, MotoParamEditor *pe)
{
    moto_param_editor_goto_next_node(pe);
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

static void __moto_param_editor_set_param_mode(MotoParamEditor *self, MotoParamMode mode)
{
    self->priv->param_mode = mode;
    __moto_param_editor_update_full(self, self->priv->node, TRUE);
}

static void __on_button_in(GtkButton* button, MotoParamEditor *pe)
{
    pe->priv->show_all = FALSE;
    __moto_param_editor_set_param_mode(pe, MOTO_PARAM_MODE_IN);
}

static void __on_button_out(GtkButton* button, MotoParamEditor *pe)
{
    pe->priv->show_all = FALSE;
    __moto_param_editor_set_param_mode(pe, MOTO_PARAM_MODE_OUT);
}

static void __on_button_inout(GtkButton* button, MotoParamEditor *pe)
{
    pe->priv->show_all = FALSE;
    __moto_param_editor_set_param_mode(pe, MOTO_PARAM_MODE_INOUT);
}

static void __on_button_all(GtkButton* button, MotoParamEditor *pe)
{
    pe->priv->show_all = TRUE;
    __moto_param_editor_set_param_mode(pe, MOTO_PARAM_MODE_INOUT);
}

static void __moto_param_editor_set_node_buttons(MotoParamEditor *self, gboolean sensitive)
{
    gtk_widget_set_sensitive(self->priv->button_in,    sensitive);
    gtk_widget_set_sensitive(self->priv->button_out,   sensitive);
    gtk_widget_set_sensitive(self->priv->button_inout, sensitive);
    gtk_widget_set_sensitive(self->priv->button_all,   sensitive);
}

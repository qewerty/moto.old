#include "moto-outliner.h"
#include "motocore/moto-world.h"

#include "moto-outliner.h"
#include "moto-test-window.h"

/* class MotoOutliner */

typedef struct _MotoOutlinerPriv MotoOutlinerPriv;

#define MOTO_OUTLINER_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_OUTLINER, MotoOutlinerPriv)

static GObjectClass *outliner_parent_class = NULL;

struct _MotoOutlinerPriv
{
    MotoWorld *world;

    GtkListStore *ls;
    GtkTreeView *tv;
};

/*
static void
moto_outliner_dispose(GObject *obj)
{
    outliner_parent_class->dispose(obj);
}
*/

static void
moto_outliner_finalize(GObject *obj)
{
    MotoOutlinerPriv *priv = MOTO_OUTLINER_GET_PRIVATE(obj);

    g_object_remove_weak_pointer(G_OBJECT(priv->world), (gpointer *) & priv->world);
    g_object_unref(priv->tv);
    g_object_unref(priv->ls);

    outliner_parent_class->finalize(obj);
}

static void
moto_outliner_init(MotoOutliner *self)
{
    MotoOutlinerPriv *priv = MOTO_OUTLINER_GET_PRIVATE(self);

    priv->world = NULL;

    priv->ls = (GtkListStore *)gtk_list_store_new(2, G_TYPE_STRING, MOTO_TYPE_NODE);
    g_object_ref_sink(priv->ls);

    priv->tv = (GtkTreeView *)gtk_tree_view_new_with_model(GTK_TREE_MODEL(priv->ls));
    g_object_ref_sink(priv->tv);

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(priv->tv, -1,
                                                "Name",
                                                renderer,
                                                "text", 0,
                                                NULL);

    gtk_box_pack_start(GTK_BOX(self), GTK_WIDGET(priv->tv), TRUE, TRUE, 0);
}

static void
moto_outliner_class_init(MotoOutlinerClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    outliner_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    // goclass->dispose    = moto_outliner_dispose;
    goclass->finalize   = moto_outliner_finalize;

    g_type_class_add_private(klass, sizeof(MotoOutlinerPriv));
}

G_DEFINE_TYPE(MotoOutliner, moto_outliner, GTK_TYPE_VBOX);

GtkWidget *moto_outliner_new(MotoWorld *world)
{
    g_return_val_if_fail(world, NULL);

    MotoOutliner *self = (MotoOutliner *)g_object_new(MOTO_TYPE_OUTLINER, NULL);
    MotoOutlinerPriv *priv = MOTO_OUTLINER_GET_PRIVATE(self);

    priv->world = world;
    g_object_add_weak_pointer(G_OBJECT(world), (gpointer *) & priv->world);

    moto_outliner_update(self);

    return (GtkWidget *)self;
}

static gboolean
append_node(MotoWorld *world, MotoNode *node, MotoOutlinerPriv *priv)
{
    GtkTreeIter iter;

    gtk_list_store_append(priv->ls, & iter);

    GString *text = g_string_new("");
    g_string_printf(text, "%s (%s)", moto_node_get_name(node), g_type_name(G_TYPE_FROM_INSTANCE(node)));

    gtk_list_store_set(priv->ls, & iter,
            0, text->str,
            1, node, -1);

    g_string_free(text, TRUE);

    return TRUE;
}

void moto_outliner_update(MotoOutliner *self)
{
    MotoOutlinerPriv *priv = MOTO_OUTLINER_GET_PRIVATE(self);

    if( ! priv->world)
        return;

    gtk_list_store_clear(priv->ls);
    moto_world_foreach_node(priv->world, MOTO_TYPE_NODE, append_node, priv);
}

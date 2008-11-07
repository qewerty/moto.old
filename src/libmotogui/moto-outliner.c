#include "moto-outliner.h"
#include "libmoto/moto-world.h"

#include "moto-outliner.h"
#include "moto-test-window.h"

static  void 
__on_row_activated(GtkTreeView       *tree_view,
                   GtkTreePath       *path,
                   GtkTreeViewColumn *column,
                   MotoOutliner      *outliner);

/* class MotoOutliner */

typedef struct _MotoOutlinerPriv MotoOutlinerPriv;

#define MOTO_OUTLINER_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_OUTLINER, MotoOutlinerPriv)

static GObjectClass *outliner_parent_class = NULL;

struct _MotoOutlinerPriv
{
    MotoWorld *world;
    MotoTestWindow *window;

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
    priv->window = NULL;

    priv->ls = (GtkListStore *)gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, MOTO_TYPE_NODE);

    priv->tv = (GtkTreeView *)gtk_tree_view_new_with_model(GTK_TREE_MODEL(priv->ls));
    gtk_tree_view_set_rules_hint(priv->tv, TRUE);
    g_object_ref_sink(priv->tv);

    g_signal_connect(G_OBJECT(priv->tv), "row-activated", G_CALLBACK(__on_row_activated), self);

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(priv->tv, -1,
                                                "Name",
                                                renderer,
                                                "text", 0,
                                                NULL);

    // renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(priv->tv, -1,
                                                "Type",
                                                renderer,
                                                "text", 1,
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

GtkWidget *moto_outliner_new(MotoTestWindow *window, MotoWorld *world)
{
    g_return_val_if_fail(world, NULL);


    MotoOutliner *self = (MotoOutliner *)g_object_new(MOTO_TYPE_OUTLINER, NULL);
    MotoOutlinerPriv *priv = MOTO_OUTLINER_GET_PRIVATE(self);

    priv->world = world;
    g_object_add_weak_pointer(G_OBJECT(world), (gpointer *) & priv->world);

    priv->window = window;

    moto_outliner_update(self);

    return (GtkWidget *)self;
}

static gboolean
__append_node(MotoWorld *world, MotoNode *node, MotoOutlinerPriv *priv)
{
    GtkTreeIter iter;

    gtk_list_store_append(priv->ls, & iter);
    gtk_list_store_set(priv->ls, & iter,
            0, moto_node_get_name(node),
            1, g_type_name(G_TYPE_FROM_INSTANCE(node)),
            2, node, -1);

    return TRUE;
}

void moto_outliner_update(MotoOutliner *self)
{
    MotoOutlinerPriv *priv = MOTO_OUTLINER_GET_PRIVATE(self);

    gtk_list_store_clear(priv->ls);

    if( ! priv->world)
        return;

    moto_world_foreach_node(priv->world, MOTO_TYPE_NODE,
            (MotoWorldForeachNodeFunc)__append_node, priv);
}

static  void 
__on_row_activated(GtkTreeView       *tree_view,
                   GtkTreePath       *path,
                   GtkTreeViewColumn *column,
                   MotoOutliner      *outliner)
{
    MotoOutlinerPriv *priv = MOTO_OUTLINER_GET_PRIVATE(outliner);

    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, & iter, path);

    MotoNode *node = NULL;
    gtk_tree_model_get(model, & iter, 2, & node, -1);
    moto_test_window_update_param_editor_full(priv->window, node);
}

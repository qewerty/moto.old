
#include "moto-test_window.h"
#include "moto-object-node.h"
#include "moto-time-node.h"

/* class TestWindow */

static GObjectClass *test_window_parent_class = NULL;

struct _MotoTestWindowPriv
{
    GString *filename;
    gboolean changed;

    GList *nodes;
    GList *selected_nodes;
    MotoNode *current_node;

    MotoObjectNode *root;
    MotoTimeNode *time;

    MotoLibrary *library;
};

static void
moto_test_window_dispose(GObject *obj)
{
    MotoTestWindow *self = (MotoTestWindow *)obj;

    g_string_free(self->priv->filename, TRUE);
    g_slice_free(MotoTestWindowPriv, self->priv);

    G_OBJECT_CLASS(test_window_parent_class)->dispose(obj);
}

static void
moto_test_window_finalize(GObject *obj)
{
    test_window_parent_class->finalize(obj);
}

static void
moto_test_window_init(MotoTestWindow *self)
{
    self->priv = g_slice_new(MotoTestWindowPriv);

    self->priv->filename = g_string_new("");
    self->priv->changed = FALSE;
}

static void
moto_test_window_class_init(MotoTestWindowClass *klass)
{
    GObejctClass *goclass = G_OBJECT_CLASS(klass);

    test_window_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_test_window_dispose;
    goclass->finalize   = moto_test_window_finalize;
}

G_DEFINE_TYPE(MotoTestWindow, moto_test_window, G_TYPE_OBJECT);

/* methods of class TestWindow */

GtkWindow *moto_test_window_new()
{
    GtkWindow *self = (GtkWindow *)g_object_new(MOTO_TYPE_TEST_WINDOW, NULL);
    MotoTestWindow *twin = (MotoTestWindow *)self;

    return self;
}


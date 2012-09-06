#include "moto-handle.h"

/* class MotoHandle */

typedef struct _MotoHandlePriv MotoHandlePriv;

struct _MotoHandlePriv
{
    gboolean disposed;
    MotoNode* node;
};

#define MOTO_HANDLE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_HANDLE, MotoHandlePriv)

static GObjectClass *handle_parent_class = NULL;

static void
moto_handle_dispose(GObject *obj)
{
    MotoHandlePriv *priv = MOTO_HANDLE_GET_PRIVATE(obj);
    if(priv->disposed)
        return;
    priv->disposed = TRUE;
    handle_parent_class->dispose(obj);
}

static void
moto_handle_finalize(GObject *obj)
{
    handle_parent_class->finalize(obj);
}

static void
moto_handle_init(MotoHandle *self)
{
    MotoHandlePriv* priv = MOTO_HANDLE_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->node = NULL;
}

static void
moto_handle_class_init(MotoHandleClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    handle_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose  = moto_handle_dispose;
    goclass->finalize = moto_handle_finalize;

    klass->set_node = NULL;
    klass->draw     = NULL;

    g_type_class_add_private(goclass, sizeof(MotoHandlePriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoHandle, moto_handle, G_TYPE_OBJECT);

MotoNode* moto_handle_get_node(MotoHandle* self)
{
    MotoHandlePriv *priv = MOTO_HANDLE_GET_PRIVATE(self);
    return priv->node;
}

void moto_handle_set_node(MotoHandle* self, MotoNode* node)
{
    MotoHandleClass *klass = MOTO_HANDLE_GET_CLASS(self);
    if(klass->set_node)
        klass->set_node(self, node);
}

void moto_handle_draw(MotoHandle* self)
{
    MotoHandleClass *klass = MOTO_HANDLE_GET_CLASS(self);
    if(klass->draw)
        klass->draw(self);
}

/* class MotoObjectHandle */

static void moto_object_handle_set_node(MotoHandle* self, MotoNode* node);
static void moto_object_handle_draw(MotoHandle* self);

typedef struct _MotoObjectHandlePriv MotoObjectHandlePriv;

struct _MotoObjectHandlePriv
{
    gboolean disposed;
};

#define MOTO_OBJECT_HANDLE_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_HANDLE, MotoObjectHandlePriv)

static GObjectClass *object_handle_parent_class = NULL;

static void
moto_object_handle_dispose(GObject *obj)
{
    MotoHandlePriv *priv = MOTO_OBJECT_HANDLE_GET_PRIVATE(obj);
    if(priv->disposed)
        return;
    priv->disposed = TRUE;
    object_handle_parent_class->dispose(obj);
}

static void
moto_object_handle_finalize(GObject *obj)
{
    object_handle_parent_class->finalize(obj);
}

static void
moto_object_handle_init(MotoObjectHandle *self)
{
    MotoHandlePriv* priv = MOTO_OBJECT_HANDLE_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->node = NULL;
}

static void
moto_object_handle_class_init(MotoObjectHandleClass *klass)
{
    GObjectClass *goclass = (GObjectClass*)klass;
    MotoHandleClass* hclass = (MotoHandleClass*)klass;

    object_handle_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose  = moto_object_handle_dispose;
    goclass->finalize = moto_object_handle_finalize;

    hclass->set_node = moto_object_handle_set_node;
    hclass->draw     = moto_object_handle_draw;

    g_type_class_add_private(goclass, sizeof(MotoObjectHandlePriv));
}

G_DEFINE_TYPE(MotoObjectHandle, moto_object_handle, MOTO_TYPE_HANDLE);

MotoHandle* moto_object_handle_new(MotoNode* node)
{
    
}

static void moto_object_handle_set_node(MotoHandle* self, MotoNode* node)
{}

static void moto_object_handle_draw(MotoHandle* self)
{}

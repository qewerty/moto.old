#include "math.h"

#include "GL/gl.h"
#include "GL/glu.h"

#include "moto-camera-node.h"
#include "moto-messager.h"

/* class CameraNode */

static GObjectClass *camera_node_parent_class = NULL;

struct _MotoCameraNodePriv
{
    gboolean prepared;
    GLuint dlist;
};

static void
moto_camera_node_dispose(GObject *obj)
{
    MotoCameraNode *self = (MotoCameraNode *)obj;

    g_slice_free(MotoCameraNodePriv, self->priv);

    G_OBJECT_CLASS(camera_node_parent_class)->dispose(obj);
}

static void
moto_camera_node_finalize(GObject *obj)
{
    camera_node_parent_class->finalize(obj);
}

static void
moto_camera_node_init(MotoCameraNode *self)
{
    self->priv = g_slice_new(MotoCameraNodePriv);

    self->priv->prepared = FALSE;
    self->priv->dlist = 0;
}

static void
moto_camera_node_class_init(MotoCameraNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    camera_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_camera_node_dispose;
    goclass->finalize   = moto_camera_node_finalize;
}

G_DEFINE_TYPE(MotoCameraNode, moto_camera_node, MOTO_TYPE_NODE);

/* methods of class CameraNode */

static gpointer get_camera(MotoParam *param)
{
    return moto_param_get_node(param);
}

MotoCameraNode *moto_camera_node_new(const gchar *name)
{
    MotoCameraNode *self = (MotoCameraNode *)g_object_new(MOTO_TYPE_CAMERA_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    /* params */

    /*
    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("camera", "Camera", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_camera_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, get_camera, NULL);
    */

    return self;
}

void moto_camera_node_apply(MotoCameraNode *self, gint width, gint height)
{
    
}

/* class CameraNodeFactory */

GType moto_camera_node_factory_get_node_type(MotoNodeFactory *self);

MotoNode *
moto_camera_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name);

static GObjectClass *camera_node_factory_parent_class = NULL;

static void
moto_camera_node_factory_dispose(GObject *obj)
{
    G_OBJECT_CLASS(camera_node_factory_parent_class)->dispose(obj);
}

static void
moto_camera_node_factory_finalize(GObject *obj)
{
    camera_node_factory_parent_class->finalize(obj);
}

static void
moto_camera_node_factory_init(MotoCameraNodeFactory *self)
{}

static void
moto_camera_node_factory_class_init(MotoCameraNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeFactoryClass *nfclass = (MotoNodeFactoryClass *)klass;

    camera_node_factory_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_camera_node_factory_dispose;
    goclass->finalize   = moto_camera_node_factory_finalize;

    nfclass->get_node_type  = moto_camera_node_factory_get_node_type;
    nfclass->create_node    = moto_camera_node_factory_create_node;
}

G_DEFINE_TYPE(MotoCameraNodeFactory, moto_camera_node_factory, MOTO_TYPE_NODE_FACTORY);

/* methods of class CameraNodeFactory */

static MotoNodeFactory *camera_node_factory = NULL;

MotoNodeFactory *moto_camera_node_factory_new()
{
    if( ! camera_node_factory)
        camera_node_factory = \
            (MotoNodeFactory *)g_object_new(MOTO_TYPE_CAMERA_NODE_FACTORY, NULL);

    return camera_node_factory;
}

GType moto_camera_node_factory_get_node_type(MotoNodeFactory *self)
{
    return MOTO_TYPE_CAMERA_NODE;
}

MotoNode *moto_camera_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name)
{
    return (MotoNode *)moto_camera_node_new(name);
}

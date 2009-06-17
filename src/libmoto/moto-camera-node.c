#include "math.h"

#include "GL/glew.h"
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

/* Methods of class CameraNode */

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


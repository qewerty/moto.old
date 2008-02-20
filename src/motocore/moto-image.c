#include "moto-image.h"

/* class Image */

static GObjectClass *image_parent_class = NULL;

static void
moto_image_dispose(GObject *obj)
{
    MotoImage *self = (MotoImage *)obj;

    g_string_free(self->priv->filename, TRUE);
    g_slice_free(MotoImagePriv, self->priv);

    G_OBJECT_CLASS(image_parent_class)->dispose(obj);
}

static void
moto_image_finalize(GObject *obj)
{
    image_parent_class->finalize(obj);
}

static void
moto_image_init(MotoImage *self)
{
    self->uint8_data = NULL;
}

static void
moto_image_class_init(MotoImageClass *klass)
{
    image_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    image_parent_class->dispose = moto_image_dispose;
    image_parent_class->finalize = moto_image_finalize;
}

G_DEFINE_TYPE(MotoImage, moto_image, G_TYPE_OBJECT);

/* methods of class Image */

MotoImage *moto_image_new(MotoImagePrecision precision, MotoImageMode mode,
        guchar chnum, guint width, guint height)
{
    MotoImage *self = (MotoImage *)g_object_new(MOTO_TYPE_IMAGE, NULL);

    self->precision = precision;
    self->mode = mode;
    self->chnum = chnum;
    self->width = width;
    self->height = height;

    guint pixel_size;
    switch(precision)
    {
        case MOTO_IMAGE_PRECISION_UINT8:
            pixel_size = 8*chnum;
        break;
        case MOTO_IMAGE_PRECISION_UINT16:
            pixel_size = 16*chnum;
        break;
        case MOTO_IMAGE_PRECISION_FLOAT32:
            pixel_size = 32*chnum;
        break;
    }

    self->uint8_data = (guint8 *)g_try_malloc(pixel_size * width * height);
    if(self->uint8_data == NULL)
    {
        /* TODO: Warning! */
        self->valid = FALSE;
    }
    else
    {
        self->valid = TRUE;
    }

    return self;
}


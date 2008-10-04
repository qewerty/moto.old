#include "moto-messager.h"
#include "moto-image.h"

/* class Image */

static GObjectClass *image_parent_class = NULL;

static void __free_channel(gpointer data, gpointer user_data)
{
    g_string_free(((MotoImageArbitraryChannel *)data)->name, TRUE);
    g_free(((MotoImageArbitraryChannel *)data)->data);
    g_slice_free(MotoImageArbitraryChannel, data);
}

static void
moto_image_dispose(GObject *obj)
{
    MotoImage *self = (MotoImage *)obj;

    g_free(self->uint8_data);

    g_slist_foreach(self->arbitrary_channels, __free_channel, NULL);

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
    self->arbitrary_channels = NULL;
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

MotoImage *moto_image_new(MotoImagePrecision precision,
        MotoImageMode mode, guint width, guint height)
{
    MotoImage *self = (MotoImage *)g_object_new(MOTO_TYPE_IMAGE, NULL);

    self->precision = precision;
    self->mode = mode;
    self->width = width;
    self->height = height;

    switch(mode)
    {
        case MOTO_IMAGE_MODE_RGB:
            self->chnum = 3;
        break;
        case MOTO_IMAGE_MODE_RGBA:
            self->chnum = 4;
        break;
        case MOTO_IMAGE_MODE_GRAYSCALE:
            self->chnum = 1;
        break;
        case MOTO_IMAGE_MODE_ARBITRARY:
            self->chnum = 0;
        break;
    }

    guint pixel_size;
    switch(precision)
    {
        case MOTO_IMAGE_PRECISION_UINT8:
            pixel_size = 8 * self->chnum;
        break;
        case MOTO_IMAGE_PRECISION_UINT16:
            pixel_size = 16 * self->chnum;
        break;
        case MOTO_IMAGE_PRECISION_FLOAT32:
            pixel_size = 32 * self->chnum;
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

/* Only for MOTO_IMAGE_MODE_ARBITRARY */

MotoImageArbitraryChannel *moto_image_get_channel(MotoImage *self, const gchar *name)
{
    GSList *ch = self->arbitrary_channels;
    for(; ch; ch = g_slist_next(ch))
    {
        if(g_utf8_collate(name, ((MotoImageArbitraryChannel *)ch)->name->str) == 0)
            return (MotoImageArbitraryChannel *)ch;
    }
    return NULL;
}

void moto_image_add_channel(MotoImage *self, const gchar *name, gsize size)
{
    if(self->mode != MOTO_IMAGE_MODE_ARBITRARY)
    {
        GString *msg = g_string_new("Image mode is not IMAGE_MODE_ARBITRARY. I won't add a new channel.");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    if(moto_image_get_channel(self, name))
    {
        GString *msg = g_string_new("");
        g_string_printf(msg, "Image already has channel '%s'. I won't add it.", name);
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    MotoImageArbitraryChannel *ach = g_slice_new(MotoImageArbitraryChannel);
    ach->name = g_string_new(name);
    ach->size = size;
    gsize buffer_size = size * self->width * self->height;
    ach->data = g_try_malloc(buffer_size);

    /* Clearing data buffer. */
    gsize i;
    for(i = 0; i < buffer_size; i++)
        ((gchar *)ach->data)[i] = 0;

    self->arbitrary_channels = \
        g_slist_append(self->arbitrary_channels, ach);
}


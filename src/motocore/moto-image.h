/* ##################################################################################
#
#  Moto Animation System (http://motoanim.sf.net)
#  Copyleft (C) 2008 Konstantin Evdokimenko a.k.a Qew[erty] (qewerty@gmail.com)
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
################################################################################## */

#ifndef MOTO_IMAGE_H
#define MOTO_IMAGE_H

#include "moto-image.h"

typedef struct _MotoImage MotoImage;
typedef struct _MotoImageClass MotoImageClass;

/* class MotoImage */

typedef enum
{
    MOTO_IMAGE_PRECISION_UINT8,
    MOTO_IMAGE_PRECISION_UINT16,
    MOTO_IMAGE_PRECISION_FLOAT32 /* 32-bit IEEE float */
} MotoImagePrecision;

typedef enum
{
    MOTO_IMAGE_MODE_RGB,
    MOTO_IMAGE_MODE_RGBA
} MotoImageMode;

struct _MotoImage
{
    GObject parent;

    MotoImagePrecision precision;
    MotoImageMode mode;
    guchar chnum;
    guint width, height;

    gboolean valid;

    union
    {
        guint8  *uint8_data;
        guint16 *uint16_data;
        gfloat  *float_data;
    };
};

struct _MotoImageClass
{
    GObjectClass parent;
};

GType moto_image_get_type(void);

#define MOTO_TYPE_IMAGE (moto_image_get_type())
#define MOTO_IMAGE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_IMAGE, MotoImage))
#define MOTO_IMAGE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_IMAGE, MotoImageClass))
#define MOTO_IS_IMAGE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_IMAGE))
#define MOTO_IS_IMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_IMAGE))
#define MOTO_IMAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_IMAGE, MotoImageClass))

MotoImage *moto_image_new(MotoImagePrecision precision, MotoImageMode mode,
        guchar chnum, guint width, guint height);

#endif /* MOTO_IMAGE_H */



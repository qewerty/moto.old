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

#ifndef MOTO_IMAGE_LOADER_H
#define MOTO_IMAGE_LOADER_H

#include "moto-image.h"

typedef struct _MotoImageLoader MotoImageLoader;
typedef struct _MotoImageLoaderClass MotoImageLoaderClass;
typedef struct _MotoImageLoaderPriv MotoImageLoaderPriv;

typedef MotoImage *(*MotoImageLoaderLoadMethod)(MotoImageLoader *self, const gchar *filename);

/* class MotoImageLoader */

struct _MotoImageLoader
{
    GObject parent;

    MotoImageLoaderPriv *priv;
};

struct _MotoImageLoaderClass
{
    GObjectClass parent;

    MotoImageLoaderLoadMethod load;
};

GType moto_image_loader_get_type(void);

#define MOTO_TYPE_IMAGE_LOADER (moto_image_loader_get_type())
#define MOTO_IMAGE_LOADER(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_IMAGE_LOADER, MotoImageLoader))
#define MOTO_IMAGE_LOADER_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_IMAGE_LOADER, MotoImageLoaderClass))
#define MOTO_IS_IMAGE_LOADER(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_IMAGE_LOADER))
#define MOTO_IS_IMAGE_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_IMAGE_LOADER))
#define MOTO_IMAGE_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_IMAGE_LOADER, MotoImageLoaderClass))

MotoImage *moto_image_loader_load(MotoImageLoader *self, const gchar *filename);

#endif /* MOTO_IMAGE_LOADER_H */


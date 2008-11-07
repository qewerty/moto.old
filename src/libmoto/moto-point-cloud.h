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

#ifndef __MOTO_POINT_CLOUD_H__
#define __MOTO_POINT_CLOUD_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MotoPointCloud MotoPointCloud;
typedef struct _MotoPointCloudIface MotoPointCloudIface;

typedef void (*MotoPointCloudForeachPointFunc)(MotoPointCloud *ptc,
        gfloat point[3], gfloat normal[3], gpointer user_data);
typedef void (*MotoPointCloudForeachPointMethod)(MotoPointCloud *self,
        MotoPointCloudForeachPointFunc func, gpointer user_data);
typedef gboolean (*MotoPointCloudCanProvidePlainDataMethod)(MotoPointCloud *self);
typedef float *(*MotoPointCloudGetPlainPointDataMethod)(MotoPointCloud *self, gsize *size);
typedef float *(*MotoPointCloudGetPlainNormalDataMethod)(MotoPointCloud *self, gsize *size);

struct _MotoPointCloudIface
{
    GTypeInterface parent;

    MotoPointCloudForeachPointMethod foreach_point;

    MotoPointCloudCanProvidePlainDataMethod can_provide_plain_data;
    MotoPointCloudGetPlainPointDataMethod get_plain_point_data;
    MotoPointCloudGetPlainNormalDataMethod get_plain_normal_data;
};

GType moto_point_cloud_get_type(void);

#define MOTO_TYPE_POINT_CLOUD           (moto_point_cloud_get_type())
#define MOTO_POINT_CLOUD(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_POINT_CLOUD, MotoPointCloud))
#define MOTO_IS_POINT_CLOUD(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_POINT_CLOUD))
#define MOTO_POINT_CLOUD_GET_INTERFACE(inst)   (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MOTO_TYPE_POINT_CLOUD, MotoPointCloudIface))

void moto_point_cloud_foreach_point(MotoPointCloud *self, MotoPointCloudForeachPointFunc func, gpointer user_data);

/* Following functions may be used for SSE optimization of point cloud data processing. */
gboolean moto_point_cloud_can_provide_plain_data(MotoPointCloud *self);
gfloat *moto_point_cloud_get_plain_point_data(MotoPointCloud *self, gsize *size);
gfloat *moto_point_cloud_get_plain_normal_data(MotoPointCloud *self, gsize *size);

G_END_DECLS

#endif /* __MOTO_POINT_CLOUD_H__ */

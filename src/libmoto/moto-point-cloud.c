#include "moto-point-cloud.h"
#include "moto-copyable.h"

static void
moto_point_cloud_base_init(gpointer g_class)
{
  static gboolean initialized = FALSE;

  if (!initialized) {
    /* create interface signals here. */
    initialized = TRUE;
  }
}

GType
moto_point_cloud_get_type (void)
{
  static GType type = 0;
  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (MotoPointCloudIface),
      moto_point_cloud_base_init,   /* base_init */
      NULL,   /* base_finalize */
      NULL,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      0,
      0,      /* n_preallocs */
      NULL    /* instance_init */
    };
    type = g_type_register_static(G_TYPE_INTERFACE, "MotoPointCloud", &info, 0);
    g_type_interface_add_prerequisite(type, MOTO_TYPE_COPYABLE);
  }
  return type;
}

void moto_point_cloud_foreach_point(MotoPointCloud *self,
        MotoPointCloudForeachPointFunc func, gpointer user_data)
{
    MotoPointCloudIface *iface = MOTO_POINT_CLOUD_GET_INTERFACE(self);

    if(iface->foreach_point)
        iface->foreach_point(self, func, user_data);
}

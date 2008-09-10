#include "moto-copyable.h"

static void
moto_copyable_base_init(gpointer g_class)
{
  static gboolean initialized = FALSE;

  if (!initialized) {
    /* create interface signals here. */
    initialized = TRUE;
  }
}

GType
moto_copyable_get_type (void)
{
  static GType type = 0;
  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (MotoCopyableIface),
      moto_copyable_base_init,   /* base_init */
      NULL,   /* base_finalize */
      NULL,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      0,
      0,      /* n_preallocs */
      NULL    /* instance_init */
    };
    type = g_type_register_static(G_TYPE_INTERFACE, "MotoCopyable", &info, 0);
  }
  return type;
}

MotoCopyable *moto_copyable_copy(MotoCopyable *self)
{
    MotoCopyableIface *iface = MOTO_COPYABLE_GET_INTERFACE(self);

    if(iface->copy)
        return iface->copy(self);

    return NULL;
}

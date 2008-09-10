#include "moto-filename.h"

GType moto_filename_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        GTypeInfo info = \
        {
            0,          /* class_size */
            NULL,       /* base_init */
            NULL,       /* base_destroy */
            NULL,       /* class_init */
            NULL,       /* class_destroy */
            NULL,       /* class_data */
            0,          /* instance_size */
            0,          /* n_preallocs */
            NULL,       /* instance_init */
            NULL,       /* value_table */
        };
        type = g_type_register_static(G_TYPE_STRING, "MotoFilename", & info, 0);
    }
    return type;
};

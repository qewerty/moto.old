#include "moto-enums.h"

GType moto_axis_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_AXIS_X, "AXIS_X", "X"},
            {MOTO_AXIS_Y, "AXIS_Y", "Y"},
            {MOTO_AXIS_Z, "AXIS_Z", "Z"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoAxis", values);
    }
    return type;
}


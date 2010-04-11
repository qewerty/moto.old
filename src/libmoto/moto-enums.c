#include "moto-enums.h"

/* MotoAxis */

GType moto_axis_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_AXIS_X, "AXIS_X", "oX"},
            {MOTO_AXIS_Y, "AXIS_Y", "oY"},
            {MOTO_AXIS_Z, "AXIS_Z", "oZ"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoAxis", values);
    }
    return type;
}

/* MotoDrawMode */

GType moto_draw_mode_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_DRAW_MODE_WIREFRAME_BBOX,
                "DRAW_MODE_WIREFRAME_BBOX", "Wireframe Bounding Box"},
            {MOTO_DRAW_MODE_SOLID_BBOX,
                "DRAW_MODE_SOLID_BBOX", "Solid Bounding Box"},
            {MOTO_DRAW_MODE_WIREFRAME,
                "DRAW_MODE_WIREFRAME", "Wireframe"},
            {MOTO_DRAW_MODE_SOLID,
                "DRAW_MODE_SOLID", "Solid"},
            {MOTO_DRAW_MODE_SMOOTH,
                "DRAW_MODE_SMOOTH", "Smooth"},
            {MOTO_DRAW_MODE_SHADED,
                "DRAW_MODE_SHADED", "Shaded"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoDrawMode", values);
    }
    return type;
}

/* MotoSelectionMode */

GType moto_selection_mode_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_SELECTION_MODE_OBJECT, "SELECTION_MODE_OBJECT", "Object"},
            {MOTO_SELECTION_MODE_VERTEX, "SELECTION_MODE_VERTEX", "Vertex"},
            {MOTO_SELECTION_MODE_EDGE, "SELECTION_MODE_EDGE", "Edge"},
            {MOTO_SELECTION_MODE_FACE, "SELECTION_MODE_FACE", "Face"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoSelectionMode", values);
    }
    return type;
}

/* MotoSelectionMode */

GType moto_cull_face_mode_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_CULL_FACE_MODE_NONE, "CULL_FACE_MODE_NONE", "None"},
            {MOTO_CULL_FACE_MODE_BACK, "CULL_FACE_MODE_BACK", "Back"},
            {MOTO_CULL_FACE_MODE_FRONT, "CULL_FACE_MODE_FRONT", "Front"},
            {MOTO_CULL_FACE_MODE_FRONT_AND_BACK, "CULL_FACE_MODE_FRONT_AND_BACK", "Front and Back"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoCullFaceMode", values);
    }
    return type;
}

/* MotoExtrudeMode */

GType moto_extrude_mode_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_EXTRUDE_MODE_VERTS,  "EXTRUDE_MODE_VERTS",  "Verts"},
            {MOTO_EXTRUDE_MODE_EDGES,  "EXTRUDE_MODE_EDGES",  "Edges"},
            {MOTO_EXTRUDE_MODE_FACES,  "EXTRUDE_MODE_FACES",  "Faces"},
            {MOTO_EXTRUDE_MODE_REGION, "EXTRUDE_MODE_REGION", "Region"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoExtrudeMode", values);
    }
    return type;
}


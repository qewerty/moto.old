#include <stdarg.h>
#include <math.h>
#include <glib/gprintf.h>
#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-rman-node.h"
#include "moto-world.h"
#include "moto-mesh.h"
#include "moto-object-node.h"
#include "libmotoutil/xform.h"
#include "libmotoutil/numdef.h"

/* forwards */

static gboolean moto_rman_node_render(MotoRenderNode *self);

/* MotoRManTarget */

GType moto_rman_target_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_RMAN_TARGET_3DELIGHT, "MOTO_RMAN_TARGET_3DELIGHT", "3Delight"},
            {MOTO_RMAN_TARGET_AQSIS, "MOTO_RMAN_TARGET_AQSIS", "Aqsis"},
            {MOTO_RMAN_TARGET_PIXIE, "MOTO_RMAN_TARGET_PIXIE", "Pixie"},
            {MOTO_RMAN_TARGET_PRMAN, "MOTO_RMAN_TARGET_PRMAN", "PRMan"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoRManTarget", values);
    }
    return type;
}

/* class MotoRManNode */

static GObjectClass *rman_node_parent_class = NULL;

static void
moto_rman_node_init(MotoRManNode *self)
{
    MotoNode *node = (MotoNode *)self;

    /* params */

    moto_node_add_params(node,
            "target", "Target", MOTO_TYPE_RMAN_TARGET, MOTO_PARAM_MODE_INOUT, MOTO_RMAN_TARGET_3DELIGHT, NULL, "Arguments",
            // "command", "Command", MOTO_TYPE_STRING, MOTO_PARAM_MODE_INOUT, "", NULL, "Arguments",
            NULL);
}

static void
moto_rman_node_class_init(MotoRManNodeClass *klass)
{
    rman_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    MotoRenderNodeClass *rclass = (MotoRenderNodeClass*)klass;
    rclass->render = moto_rman_node_render;

    /* Initializing functions for exporting geometry  */
    // moto_register_geom_processor(MOTO_TYPE_MESH,  moto_rman_export_mesh);
    // moto_register_geom_processor(MOTO_TYPE_CURVE, moto_rman_export_curve);
    // moto_register_geom_processor(MOTO_TYPE_NURBS, moto_rman_export_nurbs);
    // moto_register_geom_processor(MOTO_TYPE_SPHERE_NODE, MOTO_TYPE_MESH, moto_rman_export_mesh);
}

G_DEFINE_TYPE(MotoRManNode, moto_rman_node, MOTO_TYPE_RENDER_NODE);

/* methods of class MotoRManNode */

MotoRManNode *moto_rman_node_new(const gchar *name)
{
    MotoRManNode *self = (MotoRManNode *)g_object_new(MOTO_TYPE_RMAN_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static FILE* out = NULL;

void moto_rman_node_write(MotoRManNode *self, guint indent_num, const gchar *fmt, ...)
{
    if(!out)
        return;

    GString *indent = g_string_new("");
    guint i;
    for(i = 0; i < indent_num; ++i)
        g_string_append(indent, "    ");

    g_fprintf(out, "%s", indent->str);
    g_string_free(indent, TRUE);

    va_list ap;
    va_start(ap, fmt);
    g_vfprintf(out, fmt, ap);
    va_end(ap);
}

void moto_rman_node_writeln(MotoRManNode *self, guint indent_num, const gchar *fmt, ...)
{
    if(!out)
        return;

    GString *indent = g_string_new("");
    guint i;
    for(i = 0; i < indent_num; ++i)
        g_string_append(indent, "    ");

    g_fprintf(out, "%s", indent->str);
    g_string_free(indent, TRUE);

    va_list ap;
    va_start(ap, fmt);
    g_vfprintf(out, fmt, ap);
    va_end(ap);

    g_fprintf(out, "\n");
}

static gboolean export_object(MotoWorld *world, MotoNode *node, MotoRManNode *render)
{
    if(!moto_render_node_check_time((MotoRenderNode*)render,
                                    moto_node_get_last_modified(node)))
    {
        ;//return TRUE;
    }

    MotoParam* param = moto_node_get_param(node, "view");
    if(!param)
        return TRUE;

    MotoParam* source = moto_param_get_source(param);
    if(!source)
        return TRUE;

    MotoNode* source_node = moto_param_get_node(source);
    if(!source_node)
        return TRUE;

    MotoMesh* mesh = NULL;
    if(!moto_node_get_param_object(source_node, "mesh", (GObject**)&mesh))
        return TRUE;
    if(!mesh)
        return TRUE;

    moto_rman_node_writeln(render, 0, "# Node '%s' of type '%s'",
        moto_node_get_name(node), moto_node_get_type_name(node));
    moto_rman_node_writeln(render, 0, "AttributeBegin");

    const gfloat* mm = moto_object_node_get_matrix((MotoObjectNode*)node, TRUE);
    gfloat m[16];
    // matrix44_transpose(m, mm);
    matrix44_copy(m, mm);

    moto_rman_node_writeln(render, 1, "Surface \"plastic\"");
    moto_rman_node_writeln(render, 1,
        "ConcatTransform\n    [%f %f %f %f\n     %f %f %f %f\n     %f %f %f %f\n     %f %f %f %f]",
            m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);

    /*
    moto_rman_node_writeln(render, 1, "Surface \"%s\"");
    moto_rman_node_writeln(render, 1, "Displacement \"%s\"");
    moto_rman_node_writeln(render, 1, "Atmosphere \"%s\"");
    moto_rman_node_writeln(render, 1, "Interior \"%s\"");
    moto_rman_node_writeln(render, 1, "Exterior \"%s\"");
    moto_rman_node_writeln(render, 1, "ReadArchive \"%s\"");
    */

    moto_rman_node_write(render, 1, "PointsGeneralPolygons\n    [");

    guint32 i;
    for(i = 0; i < (mesh->f_num - 1); ++i)
        moto_rman_node_write(render, 0, "1 ", 1);
    moto_rman_node_write(render, 0, "1]\n    [");

    guint32 prev_v_offset = 0;
    for(i = 0; i < (mesh->f_num - 1); ++i)
    {
        guint32 v_offset = mesh->f_data16[i].v_offset;
        moto_rman_node_write(render, 0, "%d ", v_offset - prev_v_offset);
        prev_v_offset = v_offset;
    }
    moto_rman_node_write(render, 0, "%d]\n    [", mesh->f_data16[i].v_offset - prev_v_offset);

    prev_v_offset = 0;
    i = 0;
    for(; i < (mesh->f_num - 1); ++i)
    {
        guint32 v_offset = mesh->f_data16[i].v_offset;
        guint32 v_num = v_offset - prev_v_offset;

        guint32 j;
        for(j = 0; j < v_num; ++j)
        {
            moto_rman_node_write(render, 0, "%d ", mesh->f_verts16[prev_v_offset + j]);
        }
        prev_v_offset = v_offset;
    }

    {
        guint32 v_offset = mesh->f_data16[i].v_offset;
        guint32 v_num = v_offset - prev_v_offset;

        guint32 j;
        for(j = 0; j < v_num; ++j)
        {
            moto_rman_node_write(render, 0, "%d ", mesh->f_verts16[prev_v_offset + j]);
        }
        moto_rman_node_write(render, 0, "]\n    \"P\" [");
    }

    // Points
    for(i = 0; i < (mesh->v_num - 1); ++i)
    {
        moto_rman_node_write(render, 0, "%f %f %f ",
            mesh->v_coords[i].x, mesh->v_coords[i].y, mesh->v_coords[i].z);
    }
    moto_rman_node_write(render, 0, "%f %f %f ]\n    \"N\" [",
        mesh->v_coords[i].x, mesh->v_coords[i].y, mesh->v_coords[i].z);

    // Normals
    for(i = 0; i < (mesh->v_num - 1); ++i)
    {
        moto_rman_node_write(render, 0, "%f %f %f ",
            mesh->v_normals[i].x, mesh->v_normals[i].y, mesh->v_normals[i].z);
    }
    moto_rman_node_write(render, 0, "%f %f %f]\n",
        mesh->v_normals[i].x, mesh->v_normals[i].y, mesh->v_normals[i].z);

    moto_rman_node_writeln(render, 0, "AttributeEnd");

    return TRUE;
}

static gboolean moto_rman_node_render(MotoRenderNode *self)
{
    MotoWorld *world = moto_node_get_world(MOTO_NODE(self));
    if( ! world)
        return FALSE;

    out = fopen("last-render.rib", "wb");

    moto_rman_node_writeln(self, 0, "# Moto last render");

    moto_rman_node_writeln(self, 0, "Display \"last-render.tiff\" \"file\" \"rgb\"");
    moto_rman_node_writeln(self, 0, "Display \"+last-render\" \"framebuffer\" \"rgb\"");
    moto_rman_node_writeln(self, 0, "Format 640 480 1");
    moto_rman_node_writeln(self, 0, "PixelSamples 4 4");
    moto_rman_node_writeln(self, 0, "PixelFilter \"catmull-rom\" 1 1");
    moto_rman_node_writeln(self, 0, "Projection \"perspective\" \"fov\" 65");

    moto_rman_node_writeln(self, 0, "LightSource \"distantlight\" 3 \"lightcolor\" [1 1 1] \"intensity\" [.8]");

    MotoObjectNode* camera = moto_world_get_camera(world);
    moto_rman_node_writeln(self, 0, "# Camera object inverse");
    moto_rman_node_writeln(self, 0, "Identity");
    moto_rman_node_writeln(self, 0, "Scale 1 1 -1");
    if(camera)
    {
        const gfloat* mm = moto_object_node_get_matrix(camera, TRUE);
        gfloat m[16], im[16], ambuf[16], detbuf;
        matrix44_inverse(im, mm, ambuf, detbuf);
        // matrix44_transpose(m, im);
        matrix44_copy(m, im);

        moto_rman_node_writeln(self, 0,
            "ConcatTransform\n[%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f]",
                m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7],
                m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
    }

    moto_rman_node_writeln(self, 0, "FrameBegin 1");
    moto_rman_node_writeln(self, 0, "WorldBegin");

    moto_world_foreach_node(world, MOTO_TYPE_OBJECT_NODE,
        (MotoWorldForeachNodeFunc)export_object, self);

    moto_rman_node_writeln(self, 0, "WorldEnd");
    moto_rman_node_writeln(self, 0, "FrameEnd");

    fclose(out);
    out = NULL;

    // moto_render_node_update_last_render_time(self);

    return FALSE;
}

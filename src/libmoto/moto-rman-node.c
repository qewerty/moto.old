#include <stdarg.h>
#include <math.h>
#include <glib/gprintf.h>
#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-copyable.h"
#include "moto-point-cloud.h"
#include "moto-rman-node.h"
#include "moto-world.h"
#include "moto-object-node.h"
#include "libmotoutil/matrix.h"
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

void moto_rman_node_write(MotoRManNode *self, guint indent_num, const gchar *fmt, ...)
{
    GString *indent = g_string_new("");
    guint i;
    for(i = 0; i < indent_num; ++i)
        g_string_append(indent, "    ");

    g_print("%s", indent->str);
    g_string_free(indent, TRUE);

    va_list ap;
    va_start(ap, fmt);
    g_vprintf(fmt, ap);
    va_end(ap);

    g_print("\n");
}

static gboolean export_object(MotoWorld *world, MotoNode *node, MotoRManNode *render)
{
    if( ! moto_render_node_check_time((MotoRenderNode*)render,
                                      moto_node_get_last_modified(node)))
    {
        return TRUE;
    }

    moto_rman_node_write(render, 0, "# Node '%s' of type '%s'",
        moto_node_get_name(node), moto_node_get_type_name(node));
    moto_rman_node_write(render, 0, "AttributeBegin");
    moto_rman_node_write(render, 1, "Surface \"%s\"");
    moto_rman_node_write(render, 1, "Displacement \"%s\"");
    moto_rman_node_write(render, 1, "Atmosphere \"%s\"");
    moto_rman_node_write(render, 1, "Interior \"%s\"");
    moto_rman_node_write(render, 1, "Exterior \"%s\"");
    moto_rman_node_write(render, 1, "ReadArchive \"%s\"");
    moto_rman_node_write(render, 0, "AttributeEnd");

    return TRUE;
}

static gboolean moto_rman_node_render(MotoRenderNode *self)
{
    MotoWorld *world = moto_node_get_world(MOTO_NODE(self));
    if( ! world)
        return FALSE;

    moto_world_foreach_node(world, MOTO_TYPE_OBJECT_NODE,
        (MotoWorldForeachNodeFunc)export_object, self);

    // moto_render_node_update_last_render_time(self);

    return FALSE;
}

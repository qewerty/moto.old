#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "moto-types.h"
#include "moto-cylinder-node.h"
#include "moto-mesh.h"
#include "moto-enums.h"
#include "libmotoutil/numdef.h"
#include "libmotoutil/xform.h"

/* forwards */

static void moto_cylinder_node_update(MotoNode *self);

/* class CylinderNode */

static GObjectClass *cylinder_node_parent_class = NULL;

struct _MotoCylinderNodePriv
{
    MotoMesh *mesh;
};

static void
moto_cylinder_node_dispose(GObject *obj)
{
    MotoCylinderNode *self = (MotoCylinderNode *)obj;

    g_slice_free(MotoCylinderNodePriv, self->priv);

    G_OBJECT_CLASS(cylinder_node_parent_class)->dispose(obj);
}

static void
moto_cylinder_node_finalize(GObject *obj)
{
    cylinder_node_parent_class->finalize(obj);
}

static void
moto_cylinder_node_init(MotoCylinderNode *self)
{
    MotoNode *node = (MotoNode *)self;

    self->priv = g_slice_new(MotoCylinderNodePriv);

    self->priv->mesh = NULL;

    gfloat radius[4] = {1, 1, 1, 1};
    gint divs[2] = {3, 10};
    gint caps[2] = {TRUE, TRUE};
    gint round_caps[2] = {FALSE, FALSE};
    gint cap_divs[2] = {0, 0};
    gfloat cap_offsets[2] = {0, 0};

    MotoParamSpec *divs_spec = moto_param_spec_int2_new(3, 1, 1000000, 1, 10,
                                                         10, 3, 1000000, 1, 10);

    MotoParamSpec *cap_divs_spec = moto_param_spec_int2_new(0, 0, 1000000, 1, 2,
                                                             0, 0, 1000000, 1, 2);

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "height", "Height", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 2.0f, pspec, "Body",
            "radius", "Radius", MOTO_TYPE_FLOAT4, MOTO_PARAM_MODE_INOUT, radius, pspec, "Body",
            "divs", "Divisions",     MOTO_TYPE_INT2, MOTO_PARAM_MODE_INOUT, divs, divs_spec, "Body",
            "radius_mul", "Radius Multiplier", MOTO_TYPE_FLOAT_ARRAY, MOTO_PARAM_MODE_INOUT, NULL, pspec, "Body",
            "div0_mode", "Division 0 Mode",     MOTO_TYPE_ARRAY_MODE, MOTO_PARAM_MODE_INOUT, MOTO_ARRAY_MODE_WEIGHT, NULL, "Body",
            "div0_ratio", "Division 0 Ratio", MOTO_TYPE_FLOAT_ARRAY, MOTO_PARAM_MODE_INOUT, NULL, NULL, "Body",
            "div1_mode", "Division 1 Mode",     MOTO_TYPE_ARRAY_MODE, MOTO_PARAM_MODE_INOUT, MOTO_ARRAY_MODE_WEIGHT, NULL, "Body",
            "div1_ratio", "Division 1 Ratio", MOTO_TYPE_FLOAT_ARRAY, MOTO_PARAM_MODE_INOUT, NULL, NULL, "Body",
            "orientation", "Orientation",  MOTO_TYPE_AXIS, MOTO_PARAM_MODE_INOUT, MOTO_AXIS_Z, pspec, "Body",
            "caps", "Caps", MOTO_TYPE_BOOL2, MOTO_PARAM_MODE_INOUT, caps, pspec, "Caps",
            "cap_divs", "Cap Divisions", MOTO_TYPE_INT2, MOTO_PARAM_MODE_INOUT, cap_divs, cap_divs_spec, "Caps",
            "cap_offsets", "Cap Offsets", MOTO_TYPE_FLOAT2, MOTO_PARAM_MODE_INOUT, cap_offsets, pspec, "Caps",
            "round_caps", "Round Caps", MOTO_TYPE_BOOL2, MOTO_PARAM_MODE_INOUT, round_caps, pspec, "Caps",
            "screw", "Screw", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Screw",
            "screw_symmetry", "Screw Symmetry", MOTO_TYPE_BOOL, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Screw",
            "uv", "Generate UV", MOTO_TYPE_BOOL, MOTO_PARAM_MODE_INOUT, FALSE, pspec, "Texture Mapping",
            NULL);
}

static void
moto_cylinder_node_class_init(MotoCylinderNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoShapeNodeClass *gnclass = (MotoShapeNodeClass *)klass;

    cylinder_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_cylinder_node_dispose;
    goclass->finalize   = moto_cylinder_node_finalize;

    nclass->update = moto_cylinder_node_update;
}

G_DEFINE_TYPE(MotoCylinderNode, moto_cylinder_node, MOTO_TYPE_SHAPE_NODE);

/* Methods of class CylinderNode */

MotoCylinderNode *moto_cylinder_node_new(const gchar *name)
{
    MotoCylinderNode *self = (MotoCylinderNode *)g_object_new(MOTO_TYPE_CYLINDER_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    moto_node_set_name(node, name);

    /* params */

    return self;
}

#define get_v(r, c) ((r)*cols + (c))

static void moto_cylinder_node_update_mesh(MotoCylinderNode *self)
{
    MotoNode *node = (MotoNode*)self;

    gfloat radius_x_f, radius_y_f, radius_x_s, radius_y_s;
    moto_node_get_param_4f(node, "radius", &radius_x_f, &radius_y_f, &radius_x_s, &radius_y_s);

    gfloat height;
    moto_node_get_param_float((MotoNode *)self, "height", &height);

    gfloat screw;
    moto_node_get_param_float((MotoNode *)self, "screw", &screw);

    gboolean screw_s;
    moto_node_get_param_boolean((MotoNode *)self, "screw_symmetry", &screw_s);

    gboolean cap0, cap1;
    moto_node_get_param_2b((MotoNode *)self, "caps", &cap0, &cap1);

    gint cap0_divs, cap1_divs;
    moto_node_get_param_2i((MotoNode *)self, "cap_divs", &cap0_divs, &cap1_divs);

    gfloat cap0_offset, cap1_offset;
    moto_node_get_param_2f((MotoNode *)self, "cap_offsets", &cap0_offset, &cap1_offset);

    gint rows, cols;
    moto_node_get_param_2i(node, "divs", &rows, &cols);
    ++rows;

    MotoArrayMode div0_mode = MOTO_ARRAY_MODE_WEIGHT;
    moto_node_get_param_enum(node, "div0_mode", &div0_mode);

    g_print("div0_mode: %d\n", div0_mode);

    rows = (rows < 2) ? 2 : rows;
    cols = (cols < 3) ? 3 : cols;

    GValue* radius_mul_value = moto_node_get_param_value(node, "radius_mul");
    gfloat radius_mul[rows + 1];
    {
        gint i;
        for(i = 0; i < rows + 1; ++i)
        {
            radius_mul[i] = 1;
        }

        gsize size;
        const gfloat* radius_mul_array = \
            moto_value_get_float_array(radius_mul_value, &size);

        memcpy(radius_mul, radius_mul_array,
            sizeof(gfloat)*min(size, rows + 1));
    }

    GValue*  div0_ratio_value = moto_node_get_param_value(node, "div0_ratio");
    gsize div0_ratio_size = 0;
    const gfloat* div0_ratio = \
        moto_value_get_float_array(div0_ratio_value, &div0_ratio_size);
    gfloat div0_ratios[rows];
    gfloat div0_ratio_sum = 0;
    // Parsing div0_ratio
    {
        g_print("---\n");
        size_t i;

        switch(div0_mode)
        {
            case MOTO_ARRAY_MODE_LOCAL:
            {
                for(i = 0; i < rows; ++i)
                    div0_ratios[i] = height / (rows - 1);
            }
            break;
            case MOTO_ARRAY_MODE_GLOBAL:
            {
                for(i = 0; i < rows; ++i)
                    div0_ratios[i] = height / (rows - 1); // TODO: Implement global mode.
            }
            break;
            case MOTO_ARRAY_MODE_WEIGHT:
            {
                for(i = 0; i < rows; ++i)
                    div0_ratios[i] = 1;
            }
        }

        g_print("div0_ratio_size: %u\n", div0_ratio_size);
        g_print("div0_ratio: %p\n", div0_ratio);
        if(div0_ratio_size > 0)
            g_print("div0_ratio[0]: %f\n", div0_ratio[0]);


        memcpy(div0_ratios, div0_ratio,
            sizeof(gfloat)*(min(div0_ratio_size, rows)));

        for(i = 0; i < (rows-1); ++i)
            div0_ratio_sum += div0_ratios[i];
    }

    MotoArrayMode div1_mode = MOTO_ARRAY_MODE_WEIGHT;
    moto_node_get_param_enum(node, "div1_mode", &div1_mode);

    GValue*  div1_ratio_value = moto_node_get_param_value(node, "div1_ratio");
    gsize div1_ratio_size = 0;
    const gfloat* div1_ratio = \
        moto_value_get_float_array(div1_ratio_value, &div1_ratio_size);
    gfloat div1_ratios[cols];
    gfloat div1_ratio_sum = 0;
    {
        size_t i;

        switch(div1_mode)
        {
            case MOTO_ARRAY_MODE_LOCAL:
            {
                for(i = 0; i < cols; ++i)
                    div1_ratios[i] = PI2/cols;
            }
            break;
            case MOTO_ARRAY_MODE_GLOBAL:
            {
                for(i = 0; i < cols; ++i)
                    div1_ratios[i] = PI2/cols;
            }
            break;
            case MOTO_ARRAY_MODE_WEIGHT:
            {
                for(i = 0; i < cols; ++i)
                    div1_ratios[i] = 1;
            }
        }

        memcpy(div1_ratios, div1_ratio,
            sizeof(gfloat)*(min(div1_ratio_size, cols)));

        for(i = 0; i < cols; ++i)
            div1_ratio_sum += div1_ratios[i];
    }

    MotoAxis orientation;
    moto_node_get_param_enum((MotoNode *)self, "orientation", &orientation);

    guint v_num = rows*cols;// + 2;
    guint e_num = rows*cols + (rows-1)*cols;// + cols*2;
    guint f_num = (rows-1)*cols;// + cols*2;
    guint f_v_num = f_num*4;// - 2*cols;

    if(cap0)
    {
        if(0 == cap0_divs)
        {
            f_num += 1;
            f_v_num += cols;
        }
        else
        {
            v_num += 1;
            e_num += cols;
            f_num += cols;
            f_v_num += cols*3;

            gint divs = cap0_divs - 1;
            v_num += divs*cols;
            e_num += divs*cols*2;
            f_num += divs*cols;
            f_v_num += divs*cols*4;
        }
    }

    if(cap1)
    {
        if(0 == cap1_divs)
        {
            f_num += 1;
            f_v_num += cols;
        }
        else
        {
            v_num += 1;
            e_num += cols;
            f_num += cols;
            f_v_num += cols*3;

            gint divs = cap1_divs - 1;
            v_num += divs*cols;
            e_num += divs*cols*2;
            f_num += divs*cols;
            f_v_num += divs*cols*4;
        }
    }

    gboolean new_mesh = FALSE;
    if(self->priv->mesh)
    {
        if(v_num != self->priv->mesh->v_num || e_num != self->priv->mesh->e_num || f_num != self->priv->mesh->f_num)
        {
            g_object_unref(self->priv->mesh);
            self->priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);
            new_mesh = TRUE;
        }
    }
    else
    {
        self->priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);
        new_mesh = TRUE;
    }

    MotoMesh *mesh = self->priv->mesh;

    guint32 i, j;
    guint32 vi = 0;
    if(MOTO_AXIS_X == orientation)
    {
        for(i = 0; i < rows; i++)
        {
            for(j = 0; j < cols; j++)
            {
                gfloat sa = (screw_s) ? screw*RAD_PER_DEG*((gfloat)i-rows/2)/(rows-1) : screw*RAD_PER_DEG*((gfloat)i)/(rows-1);
                gfloat a = (PI2*j)/cols + sa;
                gfloat s = sin(a);
                gfloat c = cos(a);

                gfloat len = (height*i)/(rows-1);
                mesh->v_coords[vi].z = radius_mul[i] * (c*(radius_x_f*(len/height) + radius_x_s*(1-len/height)));
                mesh->v_coords[vi].y = radius_mul[i] * (s*(radius_y_f*(len/height) + radius_y_s*(1-len/height)));
                mesh->v_coords[vi].x = len - height/2;

                ++vi;
            }
        }
    }
    else if(MOTO_AXIS_Y == orientation)
    {
        for(i = 0; i < rows; i++)
        {
            for(j = 0; j < cols; j++)
            {
                gfloat sa = (screw_s) ? screw*RAD_PER_DEG*((gfloat)i-rows/2)/(rows-1) : screw*RAD_PER_DEG*((gfloat)i)/(rows-1);
                gfloat a = PI2 - (PI2*j)/cols + sa;
                gfloat s = sin(a);
                gfloat c = cos(a);

                gfloat len = (height*i)/(rows-1);
                mesh->v_coords[vi].z = radius_mul[i] * (c*(radius_x_f*(len/height) + radius_x_s*(1-len/height)));
                mesh->v_coords[vi].x = radius_mul[i] * (s*(radius_y_f*(len/height) + radius_y_s*(1-len/height)));
                mesh->v_coords[vi].y = len - height/2;

                ++vi;
            }
        }
    }
    else if(MOTO_AXIS_Z == orientation)
    {
        gfloat len = 0;
        for(i = 0; i < rows; i++)
        {
            gfloat ratio0 = div0_ratios[i]/div0_ratio_sum;
            g_print("ratio0: %f\n", ratio0);

            gfloat a = 0;
            for(j = 0; j < cols; j++)
            {
                gfloat ratio1 = div1_ratios[j]/div1_ratio_sum;

                gfloat sa = (screw_s) ? screw*RAD_PER_DEG*((gfloat)i-rows/2)/(rows-1) : screw*RAD_PER_DEG*((gfloat)i)/(rows-1);
                // gfloat a = (PI2*j)/cols + sa;
                a += PI2*ratio1 + sa;
                gfloat s = sin(a);
                gfloat c = cos(a);

                mesh->v_coords[vi].y = radius_mul[i] * (c*(radius_x_f*(len/height) + radius_mul[i]*radius_x_s*(1-len/height)));
                mesh->v_coords[vi].x = radius_mul[i] * (s*(radius_y_f*(len/height) + radius_mul[i]*radius_y_s*(1-len/height)));
                mesh->v_coords[vi].z = len - height/2;

                ++vi;
            }

            switch(div0_mode)
            {
                case MOTO_ARRAY_MODE_LOCAL:
                    len += div0_ratios[i];
                break;
                case MOTO_ARRAY_MODE_GLOBAL:
                    len += height*ratio0;
                break;
                case MOTO_ARRAY_MODE_WEIGHT:
                    len += height*ratio0;
                break;
            }
        }
    }

    //if(new_mesh)
    {
        guint v_offset = 0;
        guint fi = 0;

        guint32 verts[4];
        for(i = 0; i < rows-1; i++)
        {
            for(j = 0; j < cols; j++)
            {
                guint32 jj = (j == cols-1) ? 0 : j+1;
                verts[0] = get_v(i, j);
                verts[1] = get_v(i+1, j);
                verts[2] = get_v(i+1, jj);
                verts[3] = get_v(i, jj);
                moto_mesh_set_face(mesh, fi, v_offset+4, verts);

                v_offset += 4;
                ++fi;
            }
        }

        {
            guint32 verts[cols];
            if(cap0)
            {
                for(j = 0; j < cols; ++j)
                    verts[j] = get_v(0, j);

                if(0 == cap0_divs)
                {
                    moto_mesh_set_face(mesh, fi, v_offset+cols, verts);
                    v_offset += cols;
                    ++fi;
                }
                else
                {
                    mesh->v_coords[vi].x = 0;
                    mesh->v_coords[vi].y = 0;
                    mesh->v_coords[vi].z = 0;
                    for(j = 0; j < cols; ++j)
                    {
                        mesh->v_coords[vi].x += mesh->v_coords[verts[j]].x;
                        mesh->v_coords[vi].y += mesh->v_coords[verts[j]].y;
                        mesh->v_coords[vi].z += mesh->v_coords[verts[j]].z;
                    }
                    mesh->v_coords[vi].x /= cols;
                    mesh->v_coords[vi].y /= cols;
                    mesh->v_coords[vi].z /= cols;

                    if(MOTO_AXIS_X == orientation)
                        mesh->v_coords[vi].x -= cap0_offset;
                    else if(MOTO_AXIS_Y == orientation)
                        mesh->v_coords[vi].y -= cap0_offset;
                    else if(MOTO_AXIS_Z == orientation)
                        mesh->v_coords[vi].z -= cap0_offset;

                    MotoVector center = mesh->v_coords[vi];

                    guint16 vi0 = vi;
                    ++vi;

                    guint32 loop0[cols];
                    guint32 loop1[cols];
                    for(j = 0; j < cols; ++j)
                        loop0[j] = loop1[j] = verts[j];

                    gint divs = cap0_divs - 1;
                    gint k;
                    for(k = 0; k < divs; ++k)
                    {
                        guint16 v0 = vi;
                        for(j = 0; j < cols; ++j)
                        {
                            guint32 v[4];
                            v[0] = loop0[j];
                            v[1] = loop0[(j + 1) % cols];
                            v[2] = (j == (cols - 1)) ? v0 : vi + 1;
                            v[3] = vi;
                            loop1[j] = vi;

                            gfloat fac = (k+1)/(gfloat)cap0_divs;
                            gfloat one_minus_fac = 1 - fac;
                            mesh->v_coords[vi].x = (one_minus_fac*mesh->v_coords[verts[j]].x + fac*center.x);
                            mesh->v_coords[vi].y = (one_minus_fac*mesh->v_coords[verts[j]].y + fac*center.y);
                            mesh->v_coords[vi].z = (one_minus_fac*mesh->v_coords[verts[j]].z + fac*center.z);

                            moto_mesh_set_face(mesh, fi, v_offset + 4, v);
                            v_offset += 4;
                            ++fi;
                            ++vi;
                        }

                        for(j = 0; j < cols; ++j)
                            loop0[j] = loop1[j];
                    }

                    for(j = 0; j < cols; ++j)
                    {
                        guint32 v[3];
                        v[0] = loop0[j];
                        v[1] = loop0[(j + 1) % cols];
                        v[2] = vi0;
                        moto_mesh_set_face(mesh, fi, v_offset + 3, v);
                        v_offset += 3;
                        ++fi;
                    }
                }
            }

            if(cap1)
            {
                for(j = 0; j < cols; ++j)
                    verts[j] = get_v((rows-1), (cols-j-1));

                if(0 == cap1_divs)
                {
                    moto_mesh_set_face(mesh, fi, v_offset+cols, verts);
                    v_offset += cols;
                    ++fi;
                }
                else
                {
                    mesh->v_coords[vi].x = 0;
                    mesh->v_coords[vi].y = 0;
                    mesh->v_coords[vi].z = 0;
                    for(j = 0; j < cols; ++j)
                    {
                        mesh->v_coords[vi].x += mesh->v_coords[verts[j]].x;
                        mesh->v_coords[vi].y += mesh->v_coords[verts[j]].y;
                        mesh->v_coords[vi].z += mesh->v_coords[verts[j]].z;
                    }
                    mesh->v_coords[vi].x /= cols;
                    mesh->v_coords[vi].y /= cols;
                    mesh->v_coords[vi].z /= cols;

                    if(MOTO_AXIS_X == orientation)
                        mesh->v_coords[vi].x += cap1_offset;
                    else if(MOTO_AXIS_Y == orientation)
                        mesh->v_coords[vi].y += cap1_offset;
                    else if(MOTO_AXIS_Z == orientation)
                        mesh->v_coords[vi].z += cap1_offset;

                    MotoVector center = mesh->v_coords[vi];

                    guint16 vi0 = vi;
                    ++vi;

                    guint32 loop0[cols];
                    guint32 loop1[cols];
                    for(j = 0; j < cols; ++j)
                        loop0[j] = loop1[j] = verts[j];

                    gint divs = cap1_divs - 1;
                    gint k;
                    for(k = 0; k < divs; ++k)
                    {
                        guint16 v0 = vi;
                        for(j = 0; j < cols; ++j)
                        {
                            guint32 v[4];
                            v[0] = loop0[j];
                            v[1] = loop0[(j + 1) % cols];
                            v[2] = (j == (cols - 1)) ? v0 : vi + 1;
                            v[3] = vi;
                            loop1[j] = vi;

                            gfloat fac = (k+1)/(gfloat)cap1_divs;
                            gfloat one_minus_fac = 1 - fac;
                            mesh->v_coords[vi].x = (one_minus_fac*mesh->v_coords[verts[j]].x + fac*center.x);
                            mesh->v_coords[vi].y = (one_minus_fac*mesh->v_coords[verts[j]].y + fac*center.y);
                            mesh->v_coords[vi].z = (one_minus_fac*mesh->v_coords[verts[j]].z + fac*center.z);

                            moto_mesh_set_face(mesh, fi, v_offset + 4, v);
                            v_offset += 4;
                            ++fi;
                            ++vi;
                        }

                        for(j = 0; j < cols; ++j)
                            loop0[j] = loop1[j];
                    }

                    for(j = 0; j < cols; ++j)
                    {
                        guint32 v[3];
                        v[0] = loop0[j];
                        v[1] = loop0[(j + 1) % cols];
                        v[2] = vi0;
                        moto_mesh_set_face(mesh, fi, v_offset + 3, v);
                        v_offset += 3;
                        ++fi;
                    }
                }
            }
        }
    }

    if(!moto_shape_prepare((MotoShape*)mesh))
    {
        g_print("Error while preparing mesh of MotoCylinderNode\n");
    }
    moto_node_set_param_object(node, "out", (GObject*)mesh);
}
#undef get_v

static void moto_cylinder_node_update(MotoNode *self)
{
    ((MotoNodeClass*)cylinder_node_parent_class)->update(self);

    MotoCylinderNode *cylinder = (MotoCylinderNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "out");
    if(param)
        moto_cylinder_node_update_mesh(cylinder);
}

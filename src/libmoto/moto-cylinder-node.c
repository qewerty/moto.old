#include <math.h>

#include "moto-types.h"
#include "moto-cylinder-node.h"
#include "moto-mesh.h"
#include "moto-enums.h"
#include "libmotoutil/numdef.h"

/* forwards */

static void moto_cylinder_node_update(MotoNode *self);
static MotoBound *moto_cylinder_node_get_bound(MotoGeometryNode *self);

/* class CylinderNode */

static GObjectClass *cylinder_node_parent_class = NULL;

struct _MotoCylinderNodePriv
{
    MotoMesh *mesh;

    MotoBound *bound;
    gboolean bound_calculated;
};

static void
moto_cylinder_node_dispose(GObject *obj)
{
    MotoCylinderNode *self = (MotoCylinderNode *)obj;

    g_object_unref(self->priv->bound);
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
    gint   divs[2] = {10, 10};
    gint   caps[2] = {TRUE, TRUE};
    gint   round_caps[2] = {FALSE, FALSE};
    gint   cap_divisions[2] = {0, 0};

    MotoParamSpec *divs_spec = moto_param_spec_int_2_new(10, 1, 1000000, 1, 10,
                                                       10, 3, 1000000, 1, 10);

    MotoParamSpec *cap_divisions_spec = moto_param_spec_int_2_new(0, 0, 1000000, 1, 2,
                                                                  0, 0, 1000000, 1, 2);

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "height", "Height", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 2.0f, pspec, "Basic",
            "radius", "Radius", MOTO_TYPE_FLOAT_4, MOTO_PARAM_MODE_INOUT, radius, pspec, "Basic",
            "divs", "Divisions",     MOTO_TYPE_INT_2, MOTO_PARAM_MODE_INOUT, divs, divs_spec, "Basic",
            "orientation", "Orientation",  MOTO_TYPE_AXIS, MOTO_PARAM_MODE_INOUT, MOTO_AXIS_Z, pspec, "Basic",
            "caps", "Caps", MOTO_TYPE_BOOLEAN_2, MOTO_PARAM_MODE_INOUT, caps, pspec, "Caps",
            "round_caps", "Round Caps", MOTO_TYPE_BOOLEAN_2, MOTO_PARAM_MODE_INOUT, round_caps, pspec, "Caps",
            "cap_divs", "Cap Divisions", MOTO_TYPE_INT_2, MOTO_PARAM_MODE_INOUT, cap_divisions, cap_divisions_spec, "Caps",
            "screw", "Screw", MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Screw",
            "screw_symmetry", "Screw Symmetry", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Screw",
            "uv", "Generate UV", MOTO_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, FALSE, pspec, "Texture Mapping",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, self->priv->mesh, pspec, "Geometry",
            NULL);

    self->priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    self->priv->bound_calculated = FALSE;
}

static void
moto_cylinder_node_class_init(MotoCylinderNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoGeometryNodeClass *gnclass = (MotoGeometryNodeClass *)klass;

    cylinder_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    gnclass->get_bound = moto_cylinder_node_get_bound;

    goclass->dispose    = moto_cylinder_node_dispose;
    goclass->finalize   = moto_cylinder_node_finalize;

    nclass->update = moto_cylinder_node_update;
}

G_DEFINE_TYPE(MotoCylinderNode, moto_cylinder_node, MOTO_TYPE_GEOMETRY_NODE);

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

    gint rows, cols;
    moto_node_get_param_2i(node, "divs", &rows, &cols);

    rows = (rows < 2) ? 2 : rows;
    cols = (cols < 3) ? 3 : cols;

    MotoAxis orientation;
    moto_node_get_param_enum((MotoNode *)self, "orientation", &orientation);

    guint v_num = rows*cols;// + 2;
    guint e_num = rows*cols + (rows-1)*cols;// + cols*2;
    guint f_num = (rows-1)*cols + ((cap0 ? 1 : 0) + (cap1 ? 1 : 0));// + cols*2;
    guint f_v_num = f_num*4 + ((cap0 ? cols : 0) + (cap1 ? cols : 0));// - 2*cols;

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
                mesh->v_coords[vi].z = c*(radius_x_f*(len/height) + radius_x_s*(1-len/height));
                mesh->v_coords[vi].y = s*(radius_y_f*(len/height) + radius_y_s*(1-len/height));
                mesh->v_coords[vi].x = len - height/2;

                vi++;
            }
        }
        /*
        mesh->v_coords[vi].z = 0;
        mesh->v_coords[vi].y = 0;
        mesh->v_coords[vi].x = height/2;
        vi++;
        mesh->v_coords[vi].z = 0;
        mesh->v_coords[vi].y = 0;
        mesh->v_coords[vi].x = -height/2;
        vi++;
        */
    }
    else if(MOTO_AXIS_Y == orientation)
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
                mesh->v_coords[vi].z = -(c*(radius_x_f*(len/height) + radius_x_s*(1-len/height)));
                mesh->v_coords[vi].x = -(s*(radius_y_f*(len/height) + radius_y_s*(1-len/height)));
                mesh->v_coords[vi].y = -(len - height/2);

                vi++;
            }
        }
    }
    else if(MOTO_AXIS_Z == orientation)
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
                mesh->v_coords[vi].y = c*(radius_x_f*(len/height) + radius_x_s*(1-len/height));
                mesh->v_coords[vi].x = s*(radius_y_f*(len/height) + radius_y_s*(1-len/height));
                mesh->v_coords[vi].z = len - height/2;

                vi++;
            }
        }
    }

    if(new_mesh)
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
                fi++;
            }
        }

        guint32 v[cols];
        if(cap0)
        {
            for(j = 0; j < cols; ++j)
            {
                guint32 jj = (j == cols-1) ? 0 : j+1;
                v[j] = get_v(0, jj);
            }
            moto_mesh_set_face(mesh, fi, v_offset+cols, v);
            v_offset += cols;
            ++fi;
        }

        if(cap1)
        {
            for(j = 0; j < cols; ++j)
            {
                guint32 jj = (j == cols-1) ? 0 : j+1;
                v[j] = get_v(rows-1, cols-jj-1);
            }
            moto_mesh_set_face(mesh, fi, v_offset+cols, v);
        }
    }

    self->priv->bound_calculated = FALSE;
    moto_geom_prepare((MotoGeom*)mesh);
    moto_node_set_param_object(node, "mesh", (GObject*)mesh);
}
#undef get_v

static void moto_cylinder_node_update(MotoNode *self)
{
    MotoCylinderNode *cylinder = (MotoCylinderNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && 1)//moto_param_has_dests(param))
        moto_cylinder_node_update_mesh(cylinder);

    /* TODO: Implement NURBS objects =) */
    /*
    param = moto_node_get_param(self, "main", "nurbs");
    if(param && moto_param_has_dests(param))
        moto_cylinder_update_nurbs(cylinder);
    */
}

static void calc_bound(MotoCylinderNode *self)
{
    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *radius0 = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "radius0"));
    gfloat *radius1 = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "radius1"));

    gfloat radius_x      = max(radius0[0], radius1[0]);
    gfloat radius_y      = max(radius0[1], radius1[1]);
    gfloat height;
    moto_node_get_param_float((MotoNode *)self, "height", &height);
    MotoAxis orientation;
    moto_node_get_param_enum((MotoNode *)self, "orientation", &orientation);

    gfloat rsx, rsy, rsz;
    switch(orientation)
    {
        case MOTO_AXIS_X:
            rsx = radius_x;
            rsy = radius_y;
            rsz = height;
        break;
        case MOTO_AXIS_Y:
            rsx = radius_x;
            rsy = radius_y;
            rsz = height;
        break;
        case MOTO_AXIS_Z:
            rsx = radius_x;
            rsy = radius_y;
            rsz = height;
        break;
    }

    self->priv->bound->bound[0] = -rsx;
    self->priv->bound->bound[1] =  rsx;
    self->priv->bound->bound[2] = -rsy;
    self->priv->bound->bound[3] =  rsy;
    self->priv->bound->bound[4] = -rsz;
    self->priv->bound->bound[5] =  rsz;
}

static MotoBound *moto_cylinder_node_get_bound(MotoGeometryNode *self)
{
    MotoCylinderNode *cylinder = (MotoCylinderNode *)self;

    if( ! cylinder->priv->bound_calculated)
    {
        calc_bound(cylinder);
        cylinder->priv->bound_calculated = TRUE;
    }

    return cylinder->priv->bound;
}



#include <math.h>

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
    gfloat *radius_x_f_ptr;
    gfloat *radius_y_f_ptr;
    gfloat *radius_x_s_ptr;
    gfloat *radius_y_s_ptr;
    gfloat *height_ptr;

    gfloat *screw_ptr;
    gboolean *screw_s_ptr;

    guint *rows_ptr;
    guint *cols_ptr;

    MotoAxis *orientation_ptr;

    MotoMesh *mesh;
    MotoMesh **mesh_ptr;

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

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "radius_x_f", "Radius X Begin", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "radius_y_f", "Radius Y Begin", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "radius_x_s", "Radius X End", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "radius_y_s", "Radius Y End", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "height", "Height", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 2.0f, pspec, "Size", "Size",
            "screw", "Screw", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.0f, pspec, "Size", "Size",
            "screw_s", "Screw S", G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Size", "Size",
            "rows", "Rows",     G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 10u, pspec, "Divisions", "Divisions",
            "cols", "Columns",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 10u, pspec, "Divisions", "Divisions",
            "orientation", "Orientation",  MOTO_TYPE_AXIS, MOTO_PARAM_MODE_INOUT, MOTO_AXIS_Z, pspec, "Orientation", "Orientation",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, self->priv->mesh, pspec, "Geometry", "Geometry",
            NULL);

    self->priv->radius_x_f_ptr = moto_node_param_value_pointer(node, "radius_x_f", gfloat);
    self->priv->radius_y_f_ptr = moto_node_param_value_pointer(node, "radius_y_f", gfloat);
    self->priv->radius_x_s_ptr = moto_node_param_value_pointer(node, "radius_x_s", gfloat);
    self->priv->radius_y_s_ptr = moto_node_param_value_pointer(node, "radius_y_s", gfloat);
    self->priv->height_ptr = moto_node_param_value_pointer(node, "height", gfloat);
    self->priv->screw_ptr = moto_node_param_value_pointer(node, "screw", gfloat);
    self->priv->screw_s_ptr = moto_node_param_value_pointer(node, "screw_s", gfloat);

    self->priv->rows_ptr = moto_node_param_value_pointer(node, "rows", guint);
    self->priv->cols_ptr = moto_node_param_value_pointer(node, "cols", guint);

    self->priv->orientation_ptr = moto_node_param_value_pointer(node, "orientation", MotoAxis);

    self->priv->mesh_ptr = moto_node_param_value_pointer(node, "mesh", MotoMesh*);

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

/* methods of class CylinderNode */

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
    gfloat radius_x_f = *(self->priv->radius_x_f_ptr);
    gfloat radius_y_f = *(self->priv->radius_y_f_ptr);
    gfloat radius_x_s = *(self->priv->radius_x_s_ptr);
    gfloat radius_y_s = *(self->priv->radius_y_s_ptr);
    gfloat height = *(self->priv->height_ptr);
    gfloat screw = *(self->priv->screw_ptr);
    gboolean screw_s = *(self->priv->screw_s_ptr);

    guint rows = *(self->priv->rows_ptr);
    guint cols = *(self->priv->cols_ptr);

    rows = (rows < 2) ? 2 : rows;
    cols = (cols < 3) ? 3 : cols;

    MotoAxis orientation = *(self->priv->orientation_ptr);

    guint v_num = rows*cols;// + 2;
    guint e_num = rows*cols + (rows-1)*cols;// + cols*2;
    guint f_num = (rows-1)*cols;// + cols*2;
    guint f_v_num = f_num*4;// - 2*cols;
    g_print("Cylinder: v_num, e_num, f_num, f_v_num: %u, %u, %u, %u\n", v_num, e_num, f_num, f_v_num);

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

    guint32 i, j, v_offset = 0;
    guint32 vi = 0, fi = 0;
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
        /*
        for(j = 0; j < cols; j++)
        {
            guint32 jj = (j == cols-1) ? 0 : j+1;
            verts[0] = get_v(0, j);
            verts[1] = get_v(0, jj);
            verts[2] = rows*cols;
            g_print("v0, v1, v2: %u, %u, %u\n", verts[0], verts[1], verts[2]);
            moto_mesh_set_face(mesh, fi, v_offset+3, verts);
            v_offset += 3;
            fi++;

            verts[0] = get_v(rows-2, j);
            verts[1] = get_v(rows-2, jj);
            verts[2] = rows*cols+1;
            g_print("v0, v1, v2: %u, %u, %u\n", verts[0], verts[1], verts[2]);
            moto_mesh_set_face(mesh, fi, v_offset+3, verts);
            v_offset += 3;
            fi++;
        }
        */
    }

    self->priv->bound_calculated = FALSE;
    moto_mesh_prepare(mesh);
    MotoParam *pm = moto_node_get_param((MotoNode *)self, "mesh");
    g_value_set_object(moto_param_get_value(pm), mesh);
    moto_param_update_dests(pm);
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
    gfloat radius_x = *(self->priv->radius_x_f_ptr);
    gfloat radius_y = *(self->priv->radius_y_f_ptr);
    gfloat radius_x_s = *(self->priv->radius_x_s_ptr);
    gfloat radius_y_s = *(self->priv->radius_y_s_ptr);
    gfloat height   = *(self->priv->height_ptr);
    MotoAxis orientation = *(self->priv->orientation_ptr);

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



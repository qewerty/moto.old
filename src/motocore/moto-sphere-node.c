#include <math.h>

#include "moto-sphere-node.h"
#include "moto-mesh.h"
#include "moto-enums.h"
#include "common/numdef.h"

/* forwards */

static void moto_sphere_node_update(MotoNode *self);
static MotoBound *moto_sphere_node_get_bound(MotoGeometryNode *self);

/* class SphereNode */

static GObjectClass *sphere_node_parent_class = NULL;

struct _MotoSphereNodePriv
{
    gfloat *radius_x_ptr;
    gfloat *radius_y_ptr;
    gfloat *radius_z_ptr;

    guint *rows_ptr;
    guint *cols_ptr;

    MotoAxis *orientation_ptr;

    MotoMesh *mesh;
    MotoMesh **mesh_ptr;

    MotoBound *bound;
    gboolean bound_calculated;
};

static void
moto_sphere_node_dispose(GObject *obj)
{
    MotoSphereNode *self = (MotoSphereNode *)obj;

    g_object_unref(self->priv->bound);
    g_slice_free(MotoSphereNodePriv, self->priv);

    G_OBJECT_CLASS(sphere_node_parent_class)->dispose(obj);
}

static void
moto_sphere_node_finalize(GObject *obj)
{
    sphere_node_parent_class->finalize(obj);
}

static void
moto_sphere_node_init(MotoSphereNode *self)
{
    MotoNode *node = (MotoNode *)self;

    self->priv = g_slice_new(MotoSphereNodePriv);

    self->priv->mesh = NULL;

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "radius_x", "Radius X", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "radius_y", "Radius Y", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "radius_z", "Radius Z", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "rows", "Rows",     G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 10u, pspec, "Divisions", "Divisions",
            "cols", "Columns",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 10u, pspec, "Divisions", "Divisions",
            "orientation", "Orientation",  MOTO_TYPE_AXIS, MOTO_PARAM_MODE_INOUT, MOTO_AXIS_Y, pspec, "Orientation", "Orientation",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, self->priv->mesh, pspec, "Geometry", "Geometry",
            NULL);

    self->priv->radius_x_ptr = moto_node_param_value_pointer(node, "radius_x", gfloat);
    self->priv->radius_y_ptr = moto_node_param_value_pointer(node, "radius_y", gfloat);
    self->priv->radius_z_ptr = moto_node_param_value_pointer(node, "radius_z", gfloat);

    self->priv->rows_ptr = moto_node_param_value_pointer(node, "rows", guint);
    self->priv->cols_ptr = moto_node_param_value_pointer(node, "cols", guint);

    self->priv->orientation_ptr = moto_node_param_value_pointer(node, "orientation", MotoAxis);

    self->priv->mesh_ptr = moto_node_param_value_pointer(node, "mesh", MotoMesh*);

    self->priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    self->priv->bound_calculated = FALSE;
}

static void
moto_sphere_node_class_init(MotoSphereNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoGeometryNodeClass *gnclass = (MotoGeometryNodeClass *)klass;

    sphere_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    gnclass->get_bound = moto_sphere_node_get_bound;

    goclass->dispose    = moto_sphere_node_dispose;
    goclass->finalize   = moto_sphere_node_finalize;

    nclass->update = moto_sphere_node_update;
}

G_DEFINE_TYPE(MotoSphereNode, moto_sphere_node, MOTO_TYPE_GEOMETRY_NODE);

/* methods of class SphereNode */

MotoSphereNode *moto_sphere_node_new(const gchar *name)
{
    MotoSphereNode *self = (MotoSphereNode *)g_object_new(MOTO_TYPE_SPHERE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    moto_node_set_name(node, name);

    /* params */

    return self;
}

#define e_x_num (div_x*(div_y+1))
#define e_y_num (div_y*(div_x+1))

#define e_x(x, y) ((y)*(div_x) + (x))
#define e_y_(x, y) ((x)*(div_y) + (y))
#define e_y(x, y) (e_x_num + e_y_(x, y))

#define get_v(r, c) ((r) ? (((r) != (rows-1)) ? ((r)-1)*cols + (c) + 1 : v_num-1) : 0)

static void moto_sphere_node_update_mesh(MotoSphereNode *self)
{
    gfloat radius_x = *(self->priv->radius_x_ptr);
    gfloat radius_y = *(self->priv->radius_y_ptr);
    gfloat radius_z = *(self->priv->radius_z_ptr);

    guint rows = *(self->priv->rows_ptr);
    guint cols = *(self->priv->cols_ptr);

    rows = (rows < 3) ? 3 : rows;
    cols = (cols < 3) ? 3 : cols;

    MotoAxis orientation = *(self->priv->orientation_ptr);

    guint v_num = (rows-2)*cols + 2;
    guint e_num = (rows-2)*cols + (rows-1)*cols;
    guint f_num = (rows-1)*cols;
    guint f_v_num = (rows-2)*cols*4 + 6*cols;
    g_print("Sphere: v_num, e_num, f_num: %d, %d, %d\n", v_num, e_num, f_num);

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

    new_mesh = TRUE;

    guint32 i, j, v_offset = 0;
    guint32 vi = 0, fi = 0;
    if(MOTO_AXIS_X == orientation)
    {
        mesh->v_coords[vi].z = 0;
        mesh->v_coords[vi].y = 0;
        mesh->v_coords[vi].x = radius_z;
        vi++;
        for(i = 1; i < rows-1; i++)
        {
            for(j = 0; j < cols; j++)
            {
                gfloat u = (PI2*j)/cols;
                gfloat v = (PI*i)/rows - (PI_HALF-(PI_HALF/rows));
                gfloat us = sin(u);
                gfloat uc = cos(u);
                gfloat vs = sin(v);
                gfloat vc = cos(v);

                mesh->v_coords[vi].z = -vc*uc*radius_x;
                mesh->v_coords[vi].y = -vc*us*radius_y;
                mesh->v_coords[vi].x = -vs*radius_z;

                vi++;
            }
        }
        mesh->v_coords[vi].z = 0;
        mesh->v_coords[vi].y = 0;
        mesh->v_coords[vi].x = -radius_z;
        vi++;
    }
    else if(MOTO_AXIS_Y == orientation)
    {
        mesh->v_coords[vi].x = 0;
        mesh->v_coords[vi].z = 0;
        mesh->v_coords[vi].y = radius_z;
        vi++;
        for(i = 1; i < rows-1; i++)
        {
            for(j = 0; j < cols; j++)
            {
                gfloat u = (PI2*j)/cols;
                gfloat v = (PI*i)/rows - (PI_HALF-(PI_HALF/rows));
                gfloat us = sin(u);
                gfloat uc = cos(u);
                gfloat vs = sin(v);
                gfloat vc = cos(v);

                mesh->v_coords[vi].x = -vc*uc*radius_x;
                mesh->v_coords[vi].z = -vc*us*radius_y;
                mesh->v_coords[vi].y = -vs*radius_z;

                vi++;
            }
        }
        mesh->v_coords[vi].x = 0;
        mesh->v_coords[vi].z = 0;
        mesh->v_coords[vi].y = -radius_z;
        vi++;
    }
    else if(MOTO_AXIS_Z == orientation)
    {
        mesh->v_coords[vi].x = 0;
        mesh->v_coords[vi].y = 0;
        mesh->v_coords[vi].z = -radius_z;
        vi++;
        for(i = 1; i < rows-1; i++)
        {
            for(j = 0; j < cols; j++)
            {
                gfloat u = (PI2*j)/cols;
                gfloat v = (PI*i)/rows - (PI_HALF-(PI_HALF/rows));
                gfloat us = sin(u);
                gfloat uc = cos(u);
                gfloat vs = sin(v);
                gfloat vc = cos(v);

                mesh->v_coords[vi].x = vc*uc*radius_x;
                mesh->v_coords[vi].y = vc*us*radius_y;
                mesh->v_coords[vi].z = vs*radius_z;

                vi++;
            }
        }
        mesh->v_coords[vi].x = 0;
        mesh->v_coords[vi].y = 0;
        mesh->v_coords[vi].z = radius_z;
        vi++;
    }

    if(new_mesh)
    {
        guint32 verts[4];
        for(i = 0; i < rows-1; i++)
        {
            for(j = 0; j < cols; j++)
            {
                if(0 == i)
                {
                    guint32 jj = (j == cols-1) ? 0 : j+1;
                    verts[0] = get_v(i, j);
                    verts[1] = get_v(i+1, jj);
                    verts[2] = get_v(i+1, j);
                    moto_mesh_set_face(mesh, fi, v_offset+3, verts);

                    v_offset += 3;
                }
                else if(rows-2 == i)
                {
                    guint32 jj = (j == cols-1) ? 0 : j+1;
                    verts[0] = get_v(i, jj);
                    verts[1] = get_v(i+1, jj);
                    verts[2] = get_v(i, j);
                    moto_mesh_set_face(mesh, fi, v_offset+3, verts);

                    v_offset += 3;
                }
                else
                {
                    guint32 jj = (j == cols-1) ? 0 : j+1;
                    verts[0] = get_v(i, j);
                    verts[3] = get_v(i+1, j);
                    verts[2] = get_v(i+1, jj);
                    verts[1] = get_v(i, jj);
                    moto_mesh_set_face(mesh, fi, v_offset+4, verts);

                    v_offset += 4;
                }

                fi++;
            }
        }
    }

    self->priv->bound_calculated = FALSE;
    moto_mesh_prepare(mesh);
    MotoParam *pm = moto_node_get_param((MotoNode *)self, "mesh");
    g_value_set_object(moto_param_get_value(pm), mesh);
    moto_param_update_dests(pm);
}
#undef e_x_num
#undef e_y_num
#undef e_x
#undef e_y
#undef get_v

static void moto_sphere_node_update(MotoNode *self)
{
    MotoSphereNode *sphere = (MotoSphereNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && 1)//moto_param_has_dests(param))
        moto_sphere_node_update_mesh(sphere);

    /* TODO: Implement NURBS objects =) */
    /*
    param = moto_node_get_param(self, "main", "nurbs");
    if(param && moto_param_has_dests(param))
        moto_sphere_update_nurbs(sphere);
    */
}

static void calc_bound(MotoSphereNode *self)
{
    gfloat radius_x = *(self->priv->radius_x_ptr);
    gfloat radius_y = *(self->priv->radius_y_ptr);
    gfloat radius_z = *(self->priv->radius_z_ptr);
    MotoAxis orientation = *(self->priv->orientation_ptr);

    gfloat rsx, rsy, rsz;
    switch(orientation)
    {
        case MOTO_AXIS_X:
            rsx = radius_x;
            rsy = radius_y;
            rsz = radius_z;
        break;
        case MOTO_AXIS_Y:
            rsx = radius_x;
            rsy = radius_y;
            rsz = radius_z;
        break;
        case MOTO_AXIS_Z:
            rsx = radius_x;
            rsy = radius_y;
            rsz = radius_z;
        break;
    }

    self->priv->bound->bound[0] = -rsx;
    self->priv->bound->bound[1] =  rsx;
    self->priv->bound->bound[2] = -rsy;
    self->priv->bound->bound[3] =  rsy;
    self->priv->bound->bound[4] = -rsz;
    self->priv->bound->bound[5] =  rsz;
}

static MotoBound *moto_sphere_node_get_bound(MotoGeometryNode *self)
{
    MotoSphereNode *sphere = (MotoSphereNode *)self;

    if( ! sphere->priv->bound_calculated)
    {
        calc_bound(sphere);
        sphere->priv->bound_calculated = TRUE;
    }

    return sphere->priv->bound;
}



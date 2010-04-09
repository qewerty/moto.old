#include <math.h>

#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-sphere-node.h"
#include "moto-mesh.h"
#include "moto-enums.h"
#include "libmotoutil/numdef.h"

/* forwards */

static void moto_sphere_node_update(MotoNode *self);
static MotoBound *moto_sphere_node_get_bound(MotoShapeNode *self);

/* class SphereNode */

typedef struct _MotoSphereNodePriv MotoSphereNodePriv;

#define MOTO_SPHERE_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_SPHERE_NODE, MotoSphereNodePriv)

static GObjectClass *sphere_node_parent_class = NULL;

struct _MotoSphereNodePriv
{
    gboolean disposed;

    MotoMesh *mesh;

    MotoBound *bound;
    gboolean bound_calculated;
};

static void
moto_sphere_node_dispose(GObject *obj)
{
    MotoSphereNodePriv *priv = MOTO_SPHERE_NODE_GET_PRIVATE(obj);

    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    g_object_unref(priv->bound);
    g_object_unref(priv->mesh);

    sphere_node_parent_class->dispose(obj);
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

    MotoSphereNodePriv *priv = MOTO_SPHERE_NODE_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->mesh = NULL;

    gfloat radius[3] = {1, 1, 1};
    gint   rc[2]     = {10, 10};

    MotoParamSpec *rc_spec = moto_param_spec_int_2_new(10, 3, 1000000, 1, 10,
                                                       10, 3, 1000000, 1, 10);

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "radius", "Radius",   MOTO_TYPE_FLOAT_3, MOTO_PARAM_MODE_INOUT, radius, pspec, "Form",
            "rc", "Rows/Columns", MOTO_TYPE_INT_2, MOTO_PARAM_MODE_INOUT, rc, rc_spec, "Form",
            "orientation", "Orientation",  MOTO_TYPE_AXIS, MOTO_PARAM_MODE_INOUT, MOTO_AXIS_Y, pspec, "Orientation",
            NULL);

    priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    priv->bound_calculated = FALSE;

    g_object_unref(rc_spec);
}

static void
moto_sphere_node_class_init(MotoSphereNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoShapeNodeClass *gnclass = (MotoShapeNodeClass *)klass;

    sphere_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    gnclass->get_bound = moto_sphere_node_get_bound;

    goclass->dispose    = moto_sphere_node_dispose;
    goclass->finalize   = moto_sphere_node_finalize;

    nclass->update = moto_sphere_node_update;

    g_type_class_add_private(klass, sizeof(MotoSphereNodePriv));
}

G_DEFINE_TYPE(MotoSphereNode, moto_sphere_node, MOTO_TYPE_SHAPE_NODE);

/* Methods of class SphereNode */

MotoSphereNode *moto_sphere_node_new(const gchar *name)
{
    MotoSphereNode *self = (MotoSphereNode *)g_object_new(MOTO_TYPE_SPHERE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    moto_node_set_name(node, name);

    /* params */

    return self;
}

#define get_v(r, c) ((r) ? (((r) != (rows-1)) ? ((r)-1)*cols + (c) + 1 : v_num-1) : 0)

static void moto_sphere_node_update_mesh(MotoSphereNode *self)
{
    MotoNode *node = (MotoNode*)self;
    MotoSphereNodePriv *priv = MOTO_SPHERE_NODE_GET_PRIVATE(self);

    // FIXME: Rewrite with moto_value_[g|s]et_[boolean|int|float]_[2|3|4] when them will be implemented!
    gfloat *radius = (gfloat *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "radius"));
    gint   *rc     = (gint *)g_value_peek_pointer(moto_node_get_param_value((MotoNode *)self, "rc"));

    gfloat radius_x, radius_y, radius_z;
    moto_node_get_param_3f(node, "radius", &radius_x, &radius_y, &radius_z);

    guint rows, cols;
    moto_node_get_param_2i(node, "rc", &rows, &cols);

    rows = (rows < 3) ? 3 : rows;
    cols = (cols < 3) ? 3 : cols;

    MotoAxis orientation;
    moto_node_get_param_enum((MotoNode *)self, "orientation", (gint*)&orientation);

    guint v_num = (rows-2)*cols + 2;
    guint e_num = (rows-2)*cols + (rows-1)*cols;
    guint f_num = (rows-1)*cols;
    guint f_v_num = (rows-2)*cols*4 + 6*cols;

    gboolean new_mesh = FALSE;
    if(priv->mesh)
    {
        if(v_num != priv->mesh->v_num || e_num != priv->mesh->e_num || f_num != priv->mesh->f_num)
        {
            g_object_unref(priv->mesh);
            priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);
            new_mesh = TRUE;
        }
    }
    else
    {
        priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);
        new_mesh = TRUE;
    }

    MotoMesh *mesh = priv->mesh;

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

    priv->bound_calculated = FALSE;
    moto_shape_prepare((MotoShape*)mesh);
    moto_node_set_param_object((MotoNode*)self, "out", (GObject*)mesh);
}
#undef get_v

static void moto_sphere_node_update(MotoNode *self)
{
    MotoSphereNode *sphere = (MotoSphereNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "out");
    if(param)
        moto_sphere_node_update_mesh(sphere);
}

static void calc_bound(MotoSphereNode *self)
{
    MotoNode *node = (MotoNode*)self;
    MotoSphereNodePriv *priv = MOTO_SPHERE_NODE_GET_PRIVATE(self);

    gfloat radius_x, radius_y, radius_z;
    moto_node_get_param_3f(node, "radius", &radius_x, &radius_y, &radius_z);

    MotoAxis orientation;
    moto_node_get_param_enum((MotoNode *)self, "orientation", (gint)&orientation);

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

    priv->bound->bound[0] = -rsx;
    priv->bound->bound[1] =  rsx;
    priv->bound->bound[2] = -rsy;
    priv->bound->bound[3] =  rsy;
    priv->bound->bound[4] = -rsz;
    priv->bound->bound[5] =  rsz;
}

static MotoBound *moto_sphere_node_get_bound(MotoShapeNode *self)
{
    MotoSphereNodePriv *priv = MOTO_SPHERE_NODE_GET_PRIVATE(self);

    if( ! priv->bound_calculated)
    {
        calc_bound((MotoSphereNode *)self);
        priv->bound_calculated = TRUE;
    }

    return priv->bound;
}



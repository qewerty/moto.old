#include "libmotoutil/numdef.h"
#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-cube-node.h"
#include "moto-mesh.h"

/* forwards */

static void moto_cube_node_update(MotoNode *self);
static MotoBound *moto_cube_node_get_bound(MotoGeometryNode *self);

/* class CubeNode */

typedef struct _MotoCubeNodePriv MotoCubeNodePriv;

#define MOTO_CUBE_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_CUBE_NODE, MotoCubeNodePriv)

static GObjectClass *cube_node_parent_class = NULL;

struct _MotoCubeNodePriv
{
    gboolean disposed;

    MotoMesh *mesh;

    MotoBound *bound;
    gboolean bound_calculated;
};

static void
moto_cube_node_dispose(GObject *obj)
{
    MotoCubeNodePriv *priv = MOTO_CUBE_NODE_GET_PRIVATE(obj);

    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    if(priv->mesh)
        g_object_unref(priv->mesh);

    cube_node_parent_class->dispose(obj);
}

static void
moto_cube_node_finalize(GObject *obj)
{
    cube_node_parent_class->finalize(obj);
}

static void
moto_cube_node_init(MotoCubeNode *self)
{
    MotoNode *node = (MotoNode *)self;

    MotoCubeNodePriv *priv = MOTO_CUBE_NODE_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->mesh = NULL;

    gfloat size[3] = {1, 1, 1};
    gint   divs[3] = {3, 3, 3};

    MotoParamSpec *divs_spec = moto_param_spec_int_3_new(3, 1, 1000000, 1, 10,
                                                         3, 1, 1000000, 1, 10,
                                                         3, 1, 1000000, 1, 10);

    moto_node_add_params(node,
            "size", "Size",               MOTO_TYPE_FLOAT_3, MOTO_PARAM_MODE_INOUT, size,       NULL,      "Form",
            "divs", "Divisions",          MOTO_TYPE_INT_3,   MOTO_PARAM_MODE_INOUT, divs,       divs_spec, "Form",
          // TODO: Implement.
          // "side_px", "Side +X",         G_TYPE_BOOLEAN,    MOTO_PARAM_MODE_INOUT, TRUE,       NULL,      "Sides",
          // "side_mx", "Side -X",         G_TYPE_BOOLEAN,    MOTO_PARAM_MODE_INOUT, TRUE,       NULL,      "Sides",
          // "side_py", "Side +Y",         G_TYPE_BOOLEAN,    MOTO_PARAM_MODE_INOUT, TRUE,       NULL,      "Sides",
          // "side_my", "Side -Y",         G_TYPE_BOOLEAN,    MOTO_PARAM_MODE_INOUT, TRUE,       NULL,      "Sides",
          // "side_pz", "Side +Z",         G_TYPE_BOOLEAN,    MOTO_PARAM_MODE_INOUT, TRUE,       NULL,      "Sides",
          // "side_mz", "Side -Z",         G_TYPE_BOOLEAN,    MOTO_PARAM_MODE_INOUT, TRUE,       NULL,      "Sides",
          // "bevel",    "Bevel",          G_TYPE_BOOLEAN,    MOTO_PARAM_MODE_INOUT, TRUE,       NULL,      "Bevel",
          // "bev_abs",  "Absolute Bevel", G_TYPE_BOOLEAN,    MOTO_PARAM_MODE_INOUT, TRUE,       NULL,      "Bevel",
          // "bev_size", "Bevel Size",     G_TYPE_FLOAT,      MOTO_PARAM_MODE_INOUT, 0.1f,       NULL,      "Bevel",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH,    MOTO_PARAM_MODE_OUT,   priv->mesh, NULL,      "Geometry",
            NULL);

    priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    priv->bound_calculated = FALSE;

    g_object_unref(divs_spec);
}

static void
moto_cube_node_class_init(MotoCubeNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoGeometryNodeClass *gnclass = (MotoGeometryNodeClass *)klass;

    cube_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    gnclass->get_bound = moto_cube_node_get_bound;

    goclass->dispose    = moto_cube_node_dispose;
    goclass->finalize   = moto_cube_node_finalize;

    nclass->update = moto_cube_node_update;

    g_type_class_add_private(klass, sizeof(MotoCubeNodePriv));
}

G_DEFINE_TYPE(MotoCubeNode, moto_cube_node, MOTO_TYPE_GEOMETRY_NODE);

/* Methods of class CubeNode */

MotoCubeNode *moto_cube_node_new(const gchar *name)
{
    MotoCubeNode *self = (MotoCubeNode *)g_object_new(MOTO_TYPE_CUBE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;
    moto_node_set_name(node, name);

    /* params */

    return self;
}

#define get_v(x, y, z) \
    ((0 == (x)) ? (y)*(div_z+1) + (z) : ((div_x) == (x)) ? v_num - (div_y+1)*(div_z+1) + (y)*(div_z+1) + (z) : \
        (div_y-1)*(div_z-1) + (div_y+div_z)*2*(x) + ((0 == (y)) ? (z) : ((div_y) == (y)) ? (div_z-1) + 2*(y) + (z) : \
            (div_z-1) + 2*(y) + ((z)?1:0) ))

static void moto_cube_node_update_mesh(MotoCubeNode *self)
{
    MotoNode *node = (MotoNode*)self;
    MotoCubeNodePriv *priv = MOTO_CUBE_NODE_GET_PRIVATE(self);

    gfloat size_x, size_y, size_z;
    moto_node_get_param_3f(node, "size", &size_x, &size_y, &size_z);

    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;

    gint div_x, div_y, div_z;
    moto_node_get_param_3i(node, "divs", &div_x, &div_y, &div_z);

    /* TODO: Implement.
    gboolean side_px = moto_node_get_param_boolean((MotoNode *)self, "side_px");
    gboolean side_mx = moto_node_get_param_boolean((MotoNode *)self, "side_mx");
    gboolean side_py = moto_node_get_param_boolean((MotoNode *)self, "side_py");
    gboolean side_my = moto_node_get_param_boolean((MotoNode *)self, "side_my");
    gboolean side_pz = moto_node_get_param_boolean((MotoNode *)self, "side_pz");
    gboolean side_mz = moto_node_get_param_boolean((MotoNode *)self, "side_mz");
    */

    guint v_num = (div_x + div_y)*2 * (div_z + 1) + (((div_x+1)*(div_y+1) - (div_x + div_y)*2) * 2);
    guint e_num = (div_x + div_y)*2 * (div_z + 1) + (div_x + div_y)*2*div_z + div_x*(div_y-1)*2 + div_y*(div_x-1)*2;
    guint f_num = div_x*div_y*2 + div_x*div_z*2 + div_y*div_z*2;

    gboolean new_mesh = FALSE;
    if(priv->mesh)
    {
        if(v_num != priv->mesh->v_num || e_num != priv->mesh->e_num || f_num != priv->mesh->f_num)
        {
            g_object_unref(priv->mesh);
            priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_num*4);
            new_mesh = TRUE;
        }
    }
    else
    {
        priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_num*4);
        new_mesh = TRUE;
    }

    MotoMesh *mesh = priv->mesh;

    if(mesh->b32)
    {
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)mesh->f_data;
        guint32 *f_verts = (guint32 *)mesh->f_verts;

        guint32 i, j, k, n = 4;
        for(i = 0; i < f_num; i++)
        {
            f_data[i].v_offset = n;
            n += 4;
        }

        guint32 vi = 0, fi = 0;
        for(i = 0; i < div_x+1; i++)
            for(j = 0; j < div_y+1; j++)
                for(k = 0; k < div_z+1; k++)
                {
                    if(0 != i && div_x != i)
                    {
                        if(( ! (0 == k || div_z == k)) && ( ! (0 == j || div_y == j)))
                            continue;
                    }

                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = -hsy + size_y/div_y * j;
                    mesh->v_coords[vi].z = -hsz + size_z/div_z * k;
                    vi++;
                }
        if(new_mesh)
        {
            fi = 0;
            // x-axis faces
            for(i = 0; i < div_y; i++)
                for(j = 0; j < div_z; j++)
                {
                    f_verts[fi*4]   = get_v(0, i, j);
                    f_verts[fi*4+1] = get_v(0, i, j+1);
                    f_verts[fi*4+2] = get_v(0, i+1, j+1);
                    f_verts[fi*4+3] = get_v(0, i+1, j);

                    fi++;
                }
            for(i = 0; i < div_y; i++)
                for(j = 0; j < div_z; j++)
                {
                    guint32 v0 = get_v(div_x, i, j),
                            v1 = get_v(div_x, i+1, j),
                            v2 = get_v(div_x, i+1, j+1),
                            v3 = get_v(div_x, i, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
            // y-axis faces
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_z; j++)
                {
                    guint32 v0 = get_v(i, 0, j),
                            v1 = get_v(i+1, 0, j),
                            v2 = get_v(i+1, 0, j+1),
                            v3 = get_v(i, 0, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_z; j++)
                {
                    f_verts[fi*4]   = get_v(i, div_y, j);
                    f_verts[fi*4+1] = get_v(i, div_y, j+1);
                    f_verts[fi*4+2] = get_v(i+1, div_y, j+1);
                    f_verts[fi*4+3] = get_v(i+1, div_y, j);

                    fi++;
                }
            // z-axis faces
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    f_verts[fi*4]   = get_v(i, j, 0);
                    f_verts[fi*4+1] = get_v(i, j+1, 0);
                    f_verts[fi*4+2] = get_v(i+1, j+1, 0);
                    f_verts[fi*4+3] = get_v(i+1, j, 0);

                    fi++;
                }
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    f_verts[fi*4]   = get_v(i, j, div_z);
                    f_verts[fi*4+1] = get_v(i+1, j, div_z);
                    f_verts[fi*4+2] = get_v(i+1, j+1, div_z);
                    f_verts[fi*4+3] = get_v(i, j+1, div_z);

                    fi++;
                }
        }
    }
    else
    {
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)mesh->f_data;
        guint16 *f_verts = (guint16 *)mesh->f_verts;

        guint16 i, j, k, n = 4;
        for(i = 0; i < f_num; i++)
        {
            f_data[i].v_offset = n;
            n += 4;
        }

        guint16 vi = 0, fi = 0;
        for(i = 0; i < div_x+1; i++)
            for(j = 0; j < div_y+1; j++)
                for(k = 0; k < div_z+1; k++)
                {
                    if(0 != i && div_x != i)
                    {
                        if(( ! (0 == k || div_z == k)) && ( ! (0 == j || div_y == j)))
                            continue;
                    }

                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = -hsy + size_y/div_y * j;
                    mesh->v_coords[vi].z = -hsz + size_z/div_z * k;
                    vi++;
                }
        if(new_mesh)
        {
            fi = 0;
            // x-axis faces
            for(i = 0; i < div_y; i++)
                for(j = 0; j < div_z; j++)
                {
                    f_verts[fi*4]   = get_v(0, i, j);
                    f_verts[fi*4+1] = get_v(0, i, j+1);
                    f_verts[fi*4+2] = get_v(0, i+1, j+1);
                    f_verts[fi*4+3] = get_v(0, i+1, j);

                    fi++;
                }
            for(i = 0; i < div_y; i++)
                for(j = 0; j < div_z; j++)
                {
                    guint16 v0 = get_v(div_x, i, j),
                            v1 = get_v(div_x, i+1, j),
                            v2 = get_v(div_x, i+1, j+1),
                            v3 = get_v(div_x, i, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
            // y-axis faces
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_z; j++)
                {
                    guint16 v0 = get_v(i, 0, j),
                            v1 = get_v(i+1, 0, j),
                            v2 = get_v(i+1, 0, j+1),
                            v3 = get_v(i, 0, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_z; j++)
                {
                    f_verts[fi*4]   = get_v(i, div_y, j);
                    f_verts[fi*4+1] = get_v(i, div_y, j+1);
                    f_verts[fi*4+2] = get_v(i+1, div_y, j+1);
                    f_verts[fi*4+3] = get_v(i+1, div_y, j);

                    fi++;
                }
            // z-axis faces
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    f_verts[fi*4]   = get_v(i, j, 0);
                    f_verts[fi*4+1] = get_v(i, j+1, 0);
                    f_verts[fi*4+2] = get_v(i+1, j+1, 0);
                    f_verts[fi*4+3] = get_v(i+1, j, 0);

                    fi++;
                }
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    f_verts[fi*4]   = get_v(i, j, div_z);
                    f_verts[fi*4+1] = get_v(i+1, j, div_z);
                    f_verts[fi*4+2] = get_v(i+1, j+1, div_z);
                    f_verts[fi*4+3] = get_v(i, j+1, div_z);

                    fi++;
                }
        }
    }

    priv->bound_calculated = FALSE;
    moto_geom_prepare((MotoGeom*)mesh);
    moto_node_set_param_object(node, "mesh", (GObject*)mesh);
}
#undef get_v

static void moto_cube_node_update(MotoNode *self)
{
    MotoCubeNode *cube = (MotoCubeNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && 1)//moto_param_has_dests(param))
        moto_cube_node_update_mesh(cube);
}

static void calc_bound(MotoCubeNode *self)
{
    MotoNode *node = (MotoNode*)self;
    MotoCubeNodePriv *priv = MOTO_CUBE_NODE_GET_PRIVATE(self);

    gfloat size_x, size_y, size_z;
    moto_node_get_param_3f(node, "size", &size_x, &size_y, &size_z);

    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;

    priv->bound->bound[0] = -hsx;
    priv->bound->bound[1] =  hsx;
    priv->bound->bound[2] = -hsy;
    priv->bound->bound[3] =  hsy;
    priv->bound->bound[4] = -hsz;
    priv->bound->bound[5] =  hsz;
}

static MotoBound *moto_cube_node_get_bound(MotoGeometryNode *self)
{
    MotoCubeNodePriv *priv = MOTO_CUBE_NODE_GET_PRIVATE(self);

    if( ! priv->bound_calculated)
    {
        calc_bound((MotoCubeNode *)self);
        priv->bound_calculated = TRUE;
    }

    return priv->bound;
}


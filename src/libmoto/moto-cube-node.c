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

    gfloat *size_x_ptr;
    gfloat *size_y_ptr;
    gfloat *size_z_ptr;

    guint *div_x_ptr;
    guint *div_y_ptr;
    guint *div_z_ptr;

    gboolean *side_px_ptr;
    gboolean *side_mx_ptr;
    gboolean *side_py_ptr;
    gboolean *side_my_ptr;
    gboolean *side_pz_ptr;
    gboolean *side_mz_ptr;

    MotoMesh *mesh;
    MotoMesh **mesh_ptr;

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

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "size_x", "Size X", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 8.0f, pspec, "Size", "Size",
            "size_y", "Size Y", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 8.0f, pspec, "Size", "Size",
            "size_z", "Size Z", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 8.0f, pspec, "Size", "Size",
            "div_x", "Divisions by X",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 3u, pspec, "Divisions", "Divisions",
            "div_y", "Divisions by Y",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 3u, pspec, "Divisions", "Divisions",
            "div_z", "Divisions by Z",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 3u, pspec, "Divisions", "Divisions",
            "side_px", "Side +X",  G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Sides", "Sides",
            "side_mx", "Side -X",  G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Sides", "Sides",
            "side_py", "Side +Y",  G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Sides", "Sides",
            "side_my", "Side -Y",  G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Sides", "Sides",
            "side_pz", "Side +Z",  G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Sides", "Sides",
            "side_mz", "Side -Z",  G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Sides", "Sides",
            "bevel",    "Bevel",  G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Bevel", "Bevel",
            "bev_abs",  "Absolute Bevel",  G_TYPE_BOOLEAN, MOTO_PARAM_MODE_INOUT, TRUE, pspec, "Bevel", "Bevel",
            "bev_size", "Bevel Size",  G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.1f, pspec, "Bevel", "Bevel",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, priv->mesh, pspec, "Geometry", "Geometry",
            NULL);

    priv->size_x_ptr = moto_node_param_value_pointer(node, "size_x", gfloat);
    priv->size_y_ptr = moto_node_param_value_pointer(node, "size_y", gfloat);
    priv->size_z_ptr = moto_node_param_value_pointer(node, "size_z", gfloat);

    priv->div_x_ptr = moto_node_param_value_pointer(node, "div_x", guint);
    priv->div_y_ptr = moto_node_param_value_pointer(node, "div_y", guint);
    priv->div_z_ptr = moto_node_param_value_pointer(node, "div_z", guint);

    priv->side_px_ptr = moto_node_param_value_pointer(node, "side_px", gboolean);
    priv->side_mx_ptr = moto_node_param_value_pointer(node, "side_mx", gboolean);
    priv->side_py_ptr = moto_node_param_value_pointer(node, "side_py", gboolean);
    priv->side_my_ptr = moto_node_param_value_pointer(node, "side_my", gboolean);
    priv->side_pz_ptr = moto_node_param_value_pointer(node, "side_pz", gboolean);
    priv->side_mz_ptr = moto_node_param_value_pointer(node, "side_mz", gboolean);

    priv->mesh_ptr = moto_node_param_value_pointer(node, "mesh", MotoMesh*);

    priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    priv->bound_calculated = FALSE;
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

/* methods of class CubeNode */

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
    MotoCubeNodePriv *priv = MOTO_CUBE_NODE_GET_PRIVATE(self);

    gfloat size_x = *(priv->size_x_ptr);
    gfloat size_y = *(priv->size_y_ptr);
    gfloat size_z = *(priv->size_z_ptr);
    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;

    guint div_x = *(priv->div_x_ptr);
    guint div_y = *(priv->div_y_ptr);
    guint div_z = *(priv->div_z_ptr);

    if(div_x < 1)
        div_x = 1;
    if(div_y < 1)
        div_y = 1;
    if(div_z < 1)
        div_z = 1;

    gboolean side_px = *(priv->side_px_ptr);
    gboolean side_mx = *(priv->side_mx_ptr);
    gboolean side_py = *(priv->side_py_ptr);
    gboolean side_my = *(priv->side_my_ptr);
    gboolean side_pz = *(priv->side_pz_ptr);
    gboolean side_mz = *(priv->side_mz_ptr);

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

    MotoParam *pm = moto_node_get_param((MotoNode *)self, "mesh");
    g_value_set_object(moto_param_get_value(pm), mesh);

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
    moto_mesh_prepare(mesh);
    moto_param_update_dests(pm);
}
#undef get_v

static void moto_cube_node_update(MotoNode *self)
{
    MotoCubeNode *cube = (MotoCubeNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && moto_param_has_dests(param))
        moto_cube_node_update_mesh(cube);

    /* TODO: Implement NURBS objects =) */
    /*
    param = moto_node_get_param(self, "main", "nurbs");
    if(param && moto_param_has_dests(param))
        moto_cube_update_nurbs(cube);
    */
}

static void calc_bound(MotoCubeNode *self)
{
    MotoCubeNodePriv *priv = MOTO_CUBE_NODE_GET_PRIVATE(self);

    gfloat size_x = *(priv->size_x_ptr);
    gfloat size_y = *(priv->size_y_ptr);
    gfloat size_z = *(priv->size_z_ptr);
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


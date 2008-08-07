#include "moto-cube-node.h"
#include "moto-mesh.h"

/* forwards */

static void moto_cube_node_update(MotoNode *self);
static MotoBound *moto_cube_node_get_bound(MotoGeometryNode *self);

/* class CubeNode */

static GObjectClass *cube_node_parent_class = NULL;

struct _MotoCubeNodePriv
{
    gfloat *size_x_ptr;
    gfloat *size_y_ptr;
    gfloat *size_z_ptr;

    guint *div_x_ptr;
    guint *div_y_ptr;
    guint *div_z_ptr;

    MotoMesh *mesh;
    MotoMesh **mesh_ptr;

    MotoBound *bound;
    gboolean bound_calculated;
};

static void
moto_cube_node_dispose(GObject *obj)
{
    MotoCubeNode *self = (MotoCubeNode *)obj;

    g_object_unref(self->priv->bound);
    g_slice_free(MotoCubeNodePriv, self->priv);

    G_OBJECT_CLASS(cube_node_parent_class)->dispose(obj);
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

    self->priv = g_slice_new(MotoCubeNodePriv);

    self->priv->mesh = NULL;

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "size_x", "Size X", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 4.0f, pspec, "Size", "Size",
            "size_y", "Size Y", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 4.0f, pspec, "Size", "Size",
            "size_z", "Size Z", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 4.0f, pspec, "Size", "Size",
            "div_x", "Size X",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 50u, pspec, "Divisions", "Divisions",
            "div_y", "Size Y",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 50u, pspec, "Divisions", "Divisions",
            "div_z", "Size Z",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 50u, pspec, "Divisions", "Divisions",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, self->priv->mesh, pspec, "Geometry", "Geometry",
            NULL);

    self->priv->size_x_ptr = moto_node_param_value_pointer(node, "size_x", gfloat);
    self->priv->size_y_ptr = moto_node_param_value_pointer(node, "size_y", gfloat);
    self->priv->size_z_ptr = moto_node_param_value_pointer(node, "size_z", gfloat);

    self->priv->div_x_ptr = moto_node_param_value_pointer(node, "div_x", guint);
    self->priv->div_y_ptr = moto_node_param_value_pointer(node, "div_y", guint);
    self->priv->div_z_ptr = moto_node_param_value_pointer(node, "div_z", guint);

    self->priv->mesh_ptr = moto_node_param_value_pointer(node, "mesh", MotoMesh*);

    self->priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    self->priv->bound_calculated = FALSE;
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

static void moto_cube_node_update_mesh(MotoCubeNode *self)
{
    if(self->priv->mesh)
        g_object_unref(self->priv->mesh);

    /* TODO: Temporary solution! */

    gfloat size_x = *(self->priv->size_x_ptr);
    gfloat size_y = *(self->priv->size_y_ptr);
    gfloat size_z = *(self->priv->size_z_ptr);
    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;
    g_print("size_x, size_y, size_z: %f, %f, %f\n", size_x, size_y, size_z);
    g_print("hsx, hsy, hsz: %f, %f, %f\n", hsx, hsy, hsz);

    guint div_x = *(self->priv->div_x_ptr);
    guint div_y = *(self->priv->div_y_ptr);
    guint div_z = *(self->priv->div_z_ptr);
    g_print("div_x, div_y, div_z: %d, %d, %d\n", div_x, div_y, div_z);

    guint v_num = (div_x + div_y)*2 * (div_z + 1) + (((div_x+1)*(div_y+1) - (div_x + div_y)*2) * 2);
    guint e_num = (div_x + div_y)*2 * (div_z + 1) + (div_x + div_y)*2*div_z + div_x*(div_y-1)*2 + div_y*(div_x-1)*2;
    guint f_num = div_x*div_y*2 + div_x*div_z*2 + div_y*div_z*2;
    MotoMesh *mesh = self->priv->mesh = moto_mesh_new(v_num, e_num, f_num);
    g_print("v_num, e_num, f_num: %d, %d, %d\n", v_num, e_num, f_num);

    guint f_mem = moto_mesh_get_index_size(mesh) * mesh->f_num * 4;
    mesh->f_verts = g_try_malloc(f_mem);

    MotoParam *pm = moto_node_get_param((MotoNode *)self, "mesh");
    g_value_set_object(moto_param_get_value(pm), mesh);
    moto_param_update_dests(pm);

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32*)mesh->v_data;
        MotoMeshEdge32 *e_data  = (MotoMeshEdge32*)mesh->e_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)mesh->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32*)mesh->he_data;
        guint32 *e_verts = (guint32 *)mesh->e_verts;
        guint32 *f_verts = (guint32 *)mesh->f_verts;
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16*)mesh->v_data;
        MotoMeshEdge16 *e_data  = (MotoMeshEdge16*)mesh->e_data;
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)mesh->f_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16*)mesh->he_data;
        guint16 *e_verts = (guint16 *)mesh->e_verts;
        guint16 *f_verts = (guint16 *)mesh->f_verts;

        guint16 i, j, k, n = 4;
        for(i = 0; i < f_num; i++)
        {
            f_data[i].v_num = n;
            n += 4;
        }

        guint16 cube[div_x+1][div_y+1][div_z+1];

        guint16 vi = 0, ei = 0, fi = 0;
        for(i = 0; i < div_x+1; i++)
            for(j = 0; j < div_y+1; j++)
                for(k = 0; k < div_z+1; k++)
                {
                    if(0 != i && div_x != i)
                    {
                        if(( ! (0 == k || div_z == k)) && ( ! (0 == j || div_y == j)))
                            continue;
                    }

                    cube[i][j][k] = vi;
                    // g_print("i, j, k: %d, %d, %d\n", i, j, k);
                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = -hsy + size_y/div_y * j;
                    mesh->v_coords[vi].z = -hsz + size_z/div_z * k;
                    vi++;

                    if(i != 0 && (j == 0 || j == div_y || k == 0 || k == div_z ))
                    {
                        e_verts[ei*2]   = cube[i-1][j][k];
                        e_verts[ei*2+1] = cube[i][j][k];
                        // g_print("ei: %d (%d, %d)\n", ei, e_verts[ei*2], e_verts[ei*2+1]);
                        ei++;
                    }
                    if(j != 0 && (i == 0 || i == div_x || k == 0 || k == div_z ))
                    {
                        e_verts[ei*2]   = cube[i][j-1][k];
                        e_verts[ei*2+1] = cube[i][j][k];
                        // g_print("ei: %d (%d, %d)\n", ei, e_verts[ei*2], e_verts[ei*2+1]);
                        ei++;
                    }
                    if(k != 0 && (i == 0 || i == div_x || j == 0 || j == div_y ))
                    {
                        e_verts[ei*2]   = cube[i][j][k-1];
                        e_verts[ei*2+1] = cube[i][j][k];
                        // g_print("ei: %d (%d, %d)\n", ei, e_verts[ei*2], e_verts[ei*2+1]);
                        ei++;
                    }

                    // faces
                    if(i != 0 && j != 0 && (k == 0 || k == div_z))
                    {
                        if(0 != k)
                        {
                            f_verts[fi*4]   = cube[i-1][j-1][k];
                            f_verts[fi*4+1] = cube[i][j-1][k];
                            f_verts[fi*4+2] = cube[i][j][k];
                            f_verts[fi*4+3] = cube[i-1][j][k];
                        }
                        else
                        {
                            f_verts[fi*4]   = cube[i-1][j-1][k];
                            f_verts[fi*4+1] = cube[i-1][j][k];
                            f_verts[fi*4+2] = cube[i][j][k];
                            f_verts[fi*4+3] = cube[i][j-1][k];
                        }
                        fi++;
                    }
                    if(j != 0 && k != 0 && (i == 0 || i == div_x))
                    {
                        if(0 != i)
                        {
                            f_verts[fi*4]   = cube[i][j-1][k-1];
                            f_verts[fi*4+1] = cube[i][j][k-1];
                            f_verts[fi*4+2] = cube[i][j][k];
                            f_verts[fi*4+3] = cube[i][j-1][k];
                        }
                        else
                        {
                            f_verts[fi*4]   = cube[i][j-1][k-1];
                            f_verts[fi*4+1] = cube[i][j-1][k];
                            f_verts[fi*4+2] = cube[i][j][k];
                            f_verts[fi*4+3] = cube[i][j][k-1];
                        }
                        fi++;
                    }
                    if(k != 0 && i != 0 && (j == 0 || j == div_y))
                    {
                        if(0 != j)
                        {
                            f_verts[fi*4]   = cube[i-1][j][k-1];
                            f_verts[fi*4+1] = cube[i-1][j][k];
                            f_verts[fi*4+2] = cube[i][j][k];
                            f_verts[fi*4+3] = cube[i][j][k-1];
                        }
                        else
                        {
                            f_verts[fi*4]   = cube[i-1][j][k-1];
                            f_verts[fi*4+1] = cube[i][j][k-1];
                            f_verts[fi*4+2] = cube[i][j][k];
                            f_verts[fi*4+3] = cube[i-1][j][k];
                        }
                        fi++;
                    }
                }
    }

    g_print("BEFORE\n");

    moto_mesh_prepare(mesh);
}

static void moto_cube_node_update(MotoNode *self)
{
    MotoCubeNode *cube = (MotoCubeNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && 1)//moto_param_has_dests(param))
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
    gfloat size_x = *(self->priv->size_x_ptr);
    gfloat size_y = *(self->priv->size_y_ptr);
    gfloat size_z = *(self->priv->size_z_ptr);
    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;

    self->priv->bound->bound[0] = -hsx;
    self->priv->bound->bound[1] =  hsx;
    self->priv->bound->bound[2] = -hsy;
    self->priv->bound->bound[3] =  hsy;
    self->priv->bound->bound[4] = -hsz;
    self->priv->bound->bound[5] =  hsz;
}

static MotoBound *moto_cube_node_get_bound(MotoGeometryNode *self)
{
    MotoCubeNode *cube = (MotoCubeNode *)self;

    if( ! cube->priv->bound_calculated)
        calc_bound(cube);

    return cube->priv->bound;
}


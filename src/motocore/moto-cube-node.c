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
            "size_x", "Size X", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "size_y", "Size Y", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "size_z", "Size Z", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 1.0f, pspec, "Size", "Size",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, self->priv->mesh, pspec, "Geometry", "Geometry",
            NULL);

    self->priv->size_x_ptr = moto_node_param_value_pointer(node, "size_x", gfloat);
    self->priv->size_y_ptr = moto_node_param_value_pointer(node, "size_y", gfloat);
    self->priv->size_z_ptr = moto_node_param_value_pointer(node, "size_z", gfloat);

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

    MotoMesh *mesh = self->priv->mesh = moto_mesh_new(8, 12, 6);

    MotoParam *pm = moto_node_get_param((MotoNode *)self, "mesh");
    g_value_set_object(moto_param_get_value(pm), mesh);
    moto_param_update_dests(pm);

    gfloat size_x = *(self->priv->size_x_ptr);
    gfloat size_y = *(self->priv->size_y_ptr);
    gfloat size_z = *(self->priv->size_z_ptr);
    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;

    mesh->v_coords[0].x =  hsx;
    mesh->v_coords[0].y = -hsy;
    mesh->v_coords[0].z = -hsz;
    mesh->v_coords[1].x = -hsx;
    mesh->v_coords[1].y = -hsy;
    mesh->v_coords[1].z = -hsz;
    mesh->v_coords[2].x = -hsx;
    mesh->v_coords[2].y =  hsy;
    mesh->v_coords[2].z = -hsz;
    mesh->v_coords[3].x =  hsx;
    mesh->v_coords[3].y =  hsy;
    mesh->v_coords[3].z = -hsz;
    mesh->v_coords[4].x =  hsx;
    mesh->v_coords[4].y = -hsy;
    mesh->v_coords[4].z =  hsz;
    mesh->v_coords[5].x = -hsx;
    mesh->v_coords[5].y = -hsy;
    mesh->v_coords[5].z =  hsz;
    mesh->v_coords[6].x = -hsx;
    mesh->v_coords[6].y =  hsy;
    mesh->v_coords[6].z =  hsz;
    mesh->v_coords[7].x =  hsx;
    mesh->v_coords[7].y =  hsy;
    mesh->v_coords[7].z =  hsz;

    guint f_mem = moto_mesh_get_index_size(mesh) * mesh->f_num * 4;
    mesh->f_verts = g_try_malloc(f_mem);

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32*)mesh->v_data;
        MotoMeshEdge32 *e_data  = (MotoMeshEdge32*)mesh->e_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)mesh->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32*)mesh->he_data;
        guint32 *f_verts = (guint32 *)mesh->f_verts;

        f_data[0].v_num = 4;
        f_verts[0] = 0;
        f_verts[1] = 1;
        f_verts[2] = 2;
        f_verts[3] = 3;

        f_data[1].v_num = 8;
        f_verts[4] = 7;
        f_verts[5] = 6;
        f_verts[6] = 5;
        f_verts[7] = 4;

        f_data[2].v_num = 12;
        f_verts[8]  = 4;
        f_verts[9]  = 5;
        f_verts[10] = 1;
        f_verts[11] = 0;

        f_data[3].v_num = 16;
        f_verts[12] = 2;
        f_verts[13] = 6;
        f_verts[14] = 7;
        f_verts[15] = 3;

        f_data[4].v_num = 20;
        f_verts[16] = 5;
        f_verts[17] = 6;
        f_verts[18] = 2;
        f_verts[19] = 1;

        f_data[5].v_num = 24;
        f_verts[20] = 7;
        f_verts[21] = 4;
        f_verts[22] = 0;
        f_verts[23] = 3;

        guint32 *e_verts = (guint32 *)mesh->e_verts;

        e_verts[0]  = 0;
        e_verts[1]  = 1;
        e_verts[2]  = 1;
        e_verts[3]  = 2;
        e_verts[4]  = 2;
        e_verts[5]  = 3;
        e_verts[6]  = 3;
        e_verts[7]  = 0;
        e_verts[8]  = 4;
        e_verts[9]  = 5;
        e_verts[10] = 5;
        e_verts[11] = 6;
        e_verts[12] = 6;
        e_verts[13] = 7;
        e_verts[14] = 7;
        e_verts[15] = 4;
        e_verts[16] = 0;
        e_verts[17] = 4;
        e_verts[18] = 1;
        e_verts[19] = 5;
        e_verts[20] = 2;
        e_verts[21] = 6;
        e_verts[22] = 3;
        e_verts[23] = 7;
    }
    else
    {
        MotoMeshVert16 *v_data  = (MotoMeshVert16*)mesh->v_data;
        MotoMeshEdge16 *e_data  = (MotoMeshEdge16*)mesh->e_data;
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)mesh->f_data;
        MotoHalfEdge16 *he_data = (MotoHalfEdge16*)mesh->he_data;
        guint16 *f_verts = (guint16 *)mesh->f_verts;

        f_data[0].v_num = 4;
        f_verts[0] = 0;
        f_verts[1] = 1;
        f_verts[2] = 2;
        f_verts[3] = 3;

        f_data[1].v_num = 8;
        f_verts[4] = 7;
        f_verts[5] = 6;
        f_verts[6] = 5;
        f_verts[7] = 4;

        f_data[2].v_num = 12;
        f_verts[8]  = 4;
        f_verts[9]  = 5;
        f_verts[10] = 1;
        f_verts[11] = 0;

        f_data[3].v_num = 16;
        f_verts[12] = 2;
        f_verts[13] = 6;
        f_verts[14] = 7;
        f_verts[15] = 3;

        f_data[4].v_num = 20;
        f_verts[16] = 5;
        f_verts[17] = 6;
        f_verts[18] = 2;
        f_verts[19] = 1;

        f_data[5].v_num = 24;
        f_verts[20] = 7;
        f_verts[21] = 4;
        f_verts[22] = 0;
        f_verts[23] = 3;

        guint16 *e_verts = (guint16 *)mesh->e_verts;

        e_verts[0]  = 0;
        e_verts[1]  = 1;
        e_verts[2]  = 1;
        e_verts[3]  = 2;
        e_verts[4]  = 2;
        e_verts[5]  = 3;
        e_verts[6]  = 3;
        e_verts[7]  = 0;
        e_verts[8]  = 4;
        e_verts[9]  = 5;
        e_verts[10] = 5;
        e_verts[11] = 6;
        e_verts[12] = 6;
        e_verts[13] = 7;
        e_verts[14] = 7;
        e_verts[15] = 4;
        e_verts[16] = 0;
        e_verts[17] = 4;
        e_verts[18] = 1;
        e_verts[19] = 5;
        e_verts[20] = 2;
        e_verts[21] = 6;
        e_verts[22] = 3;
        e_verts[23] = 7;
    }

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


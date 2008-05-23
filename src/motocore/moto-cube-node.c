#include "moto-cube-node.h"
#include "moto-mesh.h"
#include "moto-mesh-param-data.h"

/* forwards */

static void moto_cube_node_update(MotoNode *self);
static MotoBound *moto_cube_node_get_bound(MotoGeometryNode *self);

/* class CubeNode */

static GObjectClass *cube_node_parent_class = NULL;

struct _MotoCubeNodePriv
{
    gfloat size_x;
    gfloat size_y;
    gfloat size_z;

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
    self->priv = g_slice_new(MotoCubeNodePriv);

    self->priv->size_x = 1;
    self->priv->size_y = 1;
    self->priv->size_z = 1;

    self->priv->size_x_ptr = & self->priv->size_x;
    self->priv->size_y_ptr = & self->priv->size_y;
    self->priv->size_z_ptr = & self->priv->size_z;

    self->priv->mesh = NULL;
    self->priv->mesh_ptr = & self->priv->mesh;

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

static gpointer get_mesh(MotoParam *param)
{
    MotoCubeNode *node = (MotoCubeNode *)moto_param_get_node(param);

    return & node->priv->mesh;
}

MotoCubeNode *moto_cube_node_new(const gchar *name)
{
    MotoCubeNode *self = (MotoCubeNode *)g_object_new(MOTO_TYPE_CUBE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    MotoParamBlock *pb;
    MotoParamData *pdata;

    /* params */

    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("mesh", "Polygonal Mesh", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_mesh_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, get_mesh, NULL);

    moto_node_update((MotoNode *)self);

    return self;
}

static void moto_cube_node_update_mesh(MotoCubeNode *self)
{
    if(self->priv->mesh)
        g_object_unref(self->priv->mesh);

    /* TODO: Temporary solution! */

    MotoMesh *mesh = self->priv->mesh = moto_mesh_new(8, 12, 6);

    gfloat size_x = *(self->priv->size_x_ptr);
    gfloat size_y = *(self->priv->size_y_ptr);
    gfloat size_z = *(self->priv->size_z_ptr);
    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;

    mesh->verts[0].xyz[0]   =  hsx;
    mesh->verts[0].xyz[1]   = -hsy;
    mesh->verts[0].xyz[2]   = -hsz;
    mesh->verts[1].xyz[0]   = -hsx;
    mesh->verts[1].xyz[1]   = -hsy;
    mesh->verts[1].xyz[2]   = -hsz;
    mesh->verts[2].xyz[0]   = -hsx;
    mesh->verts[2].xyz[1]   =  hsy;
    mesh->verts[2].xyz[2]   = -hsz;
    mesh->verts[3].xyz[0]   =  hsx;
    mesh->verts[3].xyz[1]   =  hsy;
    mesh->verts[3].xyz[2]   = -hsz;
    mesh->verts[4].xyz[0]   =  hsx;
    mesh->verts[4].xyz[1]   = -hsy;
    mesh->verts[4].xyz[2]   =  hsz;
    mesh->verts[5].xyz[0]   = -hsx;
    mesh->verts[5].xyz[1]   = -hsy;
    mesh->verts[5].xyz[2]   =  hsz;
    mesh->verts[6].xyz[0]   = -hsx;
    mesh->verts[6].xyz[1]   =  hsy;
    mesh->verts[6].xyz[2]   =  hsz;
    mesh->verts[7].xyz[0]   =  hsx;
    mesh->verts[7].xyz[1]   =  hsy;
    mesh->verts[7].xyz[2]   =  hsz;

    mesh->faces[0].verts_num = 4;
    moto_mesh_face_alloc(& mesh->faces[0]);
    mesh->faces[0].indecies[0] = 0;
    mesh->faces[0].indecies[1] = 1;
    mesh->faces[0].indecies[2] = 2;
    mesh->faces[0].indecies[3] = 3;
    moto_mesh_face_calc_normal(& mesh->faces[0], mesh);

    mesh->faces[1].verts_num = 4;
    moto_mesh_face_alloc(& mesh->faces[1]);
    mesh->faces[1].indecies[0] = 7;
    mesh->faces[1].indecies[1] = 6;
    mesh->faces[1].indecies[2] = 5;
    mesh->faces[1].indecies[3] = 4;
    moto_mesh_face_calc_normal(& mesh->faces[1], mesh);

    mesh->faces[2].verts_num = 4;
    moto_mesh_face_alloc(& mesh->faces[2]);
    mesh->faces[2].indecies[0] = 4;
    mesh->faces[2].indecies[1] = 5;
    mesh->faces[2].indecies[2] = 1;
    mesh->faces[2].indecies[3] = 0;
    moto_mesh_face_calc_normal(& mesh->faces[2], mesh);

    mesh->faces[3].verts_num = 4;
    moto_mesh_face_alloc(& mesh->faces[3]);
    mesh->faces[3].indecies[0] = 2;
    mesh->faces[3].indecies[1] = 6;
    mesh->faces[3].indecies[2] = 7;
    mesh->faces[3].indecies[3] = 3;
    moto_mesh_face_calc_normal(& mesh->faces[3], mesh);

    mesh->faces[4].verts_num = 4;
    moto_mesh_face_alloc(& mesh->faces[4]);
    mesh->faces[4].indecies[0] = 5;
    mesh->faces[4].indecies[1] = 6;
    mesh->faces[4].indecies[2] = 2;
    mesh->faces[4].indecies[3] = 1;
    moto_mesh_face_calc_normal(& mesh->faces[4], mesh);

    mesh->faces[5].verts_num = 4;
    moto_mesh_face_alloc(& mesh->faces[5]);
    mesh->faces[5].indecies[0] = 7;
    mesh->faces[5].indecies[1] = 4;
    mesh->faces[5].indecies[2] = 0;
    mesh->faces[5].indecies[3] = 3;
    moto_mesh_face_calc_normal(& mesh->faces[5], mesh);

    mesh->edges[0].a    = 0;
    mesh->edges[0].b    = 1;
    mesh->edges[1].a    = 1;
    mesh->edges[1].b    = 2;
    mesh->edges[2].a    = 2;
    mesh->edges[2].b    = 3;
    mesh->edges[3].a    = 3;
    mesh->edges[3].b    = 0;
    mesh->edges[4].a    = 4;
    mesh->edges[4].b    = 5;
    mesh->edges[5].a    = 5;
    mesh->edges[5].b    = 6;
    mesh->edges[6].a    = 6;
    mesh->edges[6].b    = 7;
    mesh->edges[7].a    = 7;
    mesh->edges[7].b    = 4;
    mesh->edges[8].a    = 0;
    mesh->edges[8].b    = 4;
    mesh->edges[9].a    = 1;
    mesh->edges[9].b    = 5;
    mesh->edges[10].a   = 2;
    mesh->edges[10].b   = 6;
    mesh->edges[11].a   = 3;
    mesh->edges[11].b   = 7;
}

static void moto_cube_node_update(MotoNode *self)
{
    MotoCubeNode *cube = (MotoCubeNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "main", "mesh");
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
    gfloat hsx = size_x / 2 + 0.2;
    gfloat hsy = size_y / 2 + 0.2;
    gfloat hsz = size_z / 2 + 0.2;

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

/* class CubeNodeFactory */

static GType moto_cube_node_factory_get_node_type(MotoNodeFactory *self);

MotoNode *
moto_cube_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name);

static GObjectClass *cube_node_factory_parent_class = NULL;

static void
moto_cube_node_factory_dispose(GObject *obj)
{
    G_OBJECT_CLASS(cube_node_factory_parent_class)->dispose(obj);
}

static void
moto_cube_node_factory_finalize(GObject *obj)
{
    cube_node_factory_parent_class->finalize(obj);
}

static void
moto_cube_node_factory_init(MotoCubeNodeFactory *self)
{}

static void
moto_cube_node_factory_class_init(MotoCubeNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeFactoryClass *nfclass = (MotoNodeFactoryClass *)klass;

    cube_node_factory_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_cube_node_factory_dispose;
    goclass->finalize   = moto_cube_node_factory_finalize;


    nfclass->get_node_type  = moto_cube_node_factory_get_node_type;
    nfclass->create_node    = moto_cube_node_factory_create_node;
}

G_DEFINE_TYPE(MotoCubeNodeFactory, moto_cube_node_factory, MOTO_TYPE_NODE_FACTORY);

/* methods of class CubeNodeFactory */

static MotoNodeFactory *cube_node_factory = NULL;

MotoNodeFactory *moto_cube_node_factory_new()
{
    if( ! cube_node_factory)
        cube_node_factory = \
            (MotoNodeFactory *)g_object_new(MOTO_TYPE_CUBE_NODE_FACTORY, NULL);

    return cube_node_factory;
}

static GType moto_cube_node_factory_get_node_type(MotoNodeFactory *self)
{
    return MOTO_TYPE_CUBE_NODE;
}

MotoNode *moto_cube_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name)
{
    return (MotoNode *)moto_cube_node_new(name);
}

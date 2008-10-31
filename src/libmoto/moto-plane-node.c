#include "moto-plane-node.h"
#include "moto-mesh.h"

/* enums */

GType moto_orientation_get_type(void)
{
    static GType type = 0;
    if(0 == type)
    {
        static GEnumValue values[] = {
            {MOTO_ORIENTATION_XY, "ORIENTATION_XY", "XY"},
            {MOTO_ORIENTATION_YZ, "ORIENTATION_YZ", "YZ"},
            {MOTO_ORIENTATION_ZX, "ORIENTATION_ZX", "ZX"},
            {0, NULL, NULL},
        };
        type = g_enum_register_static("MotoOrientation", values);
    }
    return type;
}

/* forwards */

static void moto_plane_node_update(MotoNode *self);
static MotoBound *moto_plane_node_get_bound(MotoGeometryNode *self);

/* class PlaneNode */

typedef struct _MotoPlaneNodePriv MotoPlaneNodePriv;

#define MOTO_PLANE_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_PLANE_NODE, MotoPlaneNodePriv)

static GObjectClass *plane_node_parent_class = NULL;

struct _MotoPlaneNodePriv
{
    gboolean disposed;

    gfloat *size_x_ptr;
    gfloat *size_y_ptr;

    guint *div_x_ptr;
    guint *div_y_ptr;

    MotoOrientation *orientation_ptr;

    MotoMesh *mesh;
    MotoMesh **mesh_ptr;

    MotoBound *bound;
    gboolean bound_calculated;
};

static void
moto_plane_node_dispose(GObject *obj)
{
    MotoPlaneNodePriv *priv = MOTO_PLANE_NODE_GET_PRIVATE(obj);

    if(priv->disposed)
        return;
    priv->disposed = TRUE;

    g_object_unref(priv->mesh);
    g_object_unref(priv->bound);

    G_OBJECT_CLASS(plane_node_parent_class)->dispose(obj);
}

static void
moto_plane_node_finalize(GObject *obj)
{
    plane_node_parent_class->finalize(obj);
}

static void
moto_plane_node_init(MotoPlaneNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoPlaneNodePriv *priv = MOTO_PLANE_NODE_GET_PRIVATE(self);
    priv->disposed = FALSE;

    priv->mesh = NULL;

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "size_x", "Size X", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 10.0f, pspec, "Size", "Size",
            "size_y", "Size Y", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 10.0f, pspec, "Size", "Size",
            "div_x", "Divisions by X",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 10u, pspec, "Divisions", "Divisions",
            "div_y", "Divisions by Y",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 10u, pspec, "Divisions", "Divisions",
            "orientation", "Orientation",  MOTO_TYPE_ORIENTATION, MOTO_PARAM_MODE_INOUT, MOTO_ORIENTATION_ZX, pspec, "Orientation", "Orientation",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, priv->mesh, pspec, "Geometry", "Geometry",
            NULL);

    priv->size_x_ptr = moto_node_param_value_pointer(node, "size_x", gfloat);
    priv->size_y_ptr = moto_node_param_value_pointer(node, "size_y", gfloat);

    priv->div_x_ptr = moto_node_param_value_pointer(node, "div_x", guint);
    priv->div_y_ptr = moto_node_param_value_pointer(node, "div_y", guint);

    priv->orientation_ptr = moto_node_param_value_pointer(node, "orientation", MotoOrientation);

    priv->mesh_ptr = moto_node_param_value_pointer(node, "mesh", MotoMesh*);

    priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    priv->bound_calculated = FALSE;
}

static void
moto_plane_node_class_init(MotoPlaneNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoGeometryNodeClass *gnclass = (MotoGeometryNodeClass *)klass;

    plane_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    gnclass->get_bound = moto_plane_node_get_bound;

    goclass->dispose    = moto_plane_node_dispose;
    goclass->finalize   = moto_plane_node_finalize;

    nclass->update = moto_plane_node_update;

    g_type_class_add_private(klass, sizeof(MotoPlaneNodePriv));
}

G_DEFINE_TYPE(MotoPlaneNode, moto_plane_node, MOTO_TYPE_GEOMETRY_NODE);

/* methods of class PlaneNode */

MotoPlaneNode *moto_plane_node_new(const gchar *name)
{
    MotoPlaneNode *self = (MotoPlaneNode *)g_object_new(MOTO_TYPE_PLANE_NODE, NULL);
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

#define get_v(x, y) ((x)*(div_y+1) + (y))

static void moto_plane_node_update_mesh(MotoPlaneNode *self)
{
    MotoPlaneNodePriv *priv = MOTO_PLANE_NODE_GET_PRIVATE(self);

    gfloat size_x = *(priv->size_x_ptr);
    gfloat size_y = *(priv->size_y_ptr);
    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;

    guint div_x = *(priv->div_x_ptr);
    guint div_y = *(priv->div_y_ptr);

    div_x = (div_x < 1) ? 1 : div_x;
    div_y = (div_y < 1) ? 1 : div_y;

    MotoOrientation orientation = *(priv->orientation_ptr);

    guint v_num = (div_x+1)*(div_y+1);
    guint e_num = div_x*(div_y+1) + (div_x+1)*div_y;
    guint f_num = div_x*div_y;
    g_print("Plane: v_num, e_num, f_num: %d, %d, %d\n", v_num, e_num, f_num);

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

        guint32 i, j, n = 4;
        for(i = 0; i < f_num; i++)
        {
            f_data[i].v_offset = n;
            n += 4;
        }

        guint32 vi = 0, fi = 0;
        if(MOTO_ORIENTATION_YZ == orientation)
        {
            for(i = 0; i < div_x+1; i++)
                for(j = 0; j < div_y+1; j++)
                {
                    mesh->v_coords[vi].x = 0;
                    mesh->v_coords[vi].y = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].z = -hsy + size_y/div_y * j;
                    vi++;
                }
        }
        else if(MOTO_ORIENTATION_ZX == orientation)
        {
            for(i = 0; i < div_x+1; i++)
                for(j = 0; j < div_y+1; j++)
                {
                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = 0;
                    mesh->v_coords[vi].z = -hsy + size_y/div_y * j;
                    vi++;
                }
        }
        else if(MOTO_ORIENTATION_XY == orientation)
        {
            for(i = 0; i < div_x+1; i++)
                for(j = 0; j < div_y+1; j++)
                {
                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = -hsy + size_y/div_y * j;
                    mesh->v_coords[vi].z = 0;
                    vi++;
                }
        }

        if(new_mesh)
        {
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    guint32 v0 = get_v(i, j),
                            v1 = get_v(i+1, j),
                            v2 = get_v(i+1, j+1),
                            v3 = get_v(i, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
        }
    }
    else
    {
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)mesh->f_data;
        guint16 *f_verts = (guint16 *)mesh->f_verts;

        guint16 i, j, n = 4;
        for(i = 0; i < f_num; i++)
        {
            f_data[i].v_offset = n;
            n += 4;
        }

        guint16 vi = 0, fi = 0;
        if(MOTO_ORIENTATION_YZ == orientation)
        {
            for(i = 0; i < div_x+1; i++)
                for(j = 0; j < div_y+1; j++)
                {
                    mesh->v_coords[vi].x = 0;
                    mesh->v_coords[vi].y = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].z = -hsy + size_y/div_y * j;
                    vi++;
                }
        }
        else if(MOTO_ORIENTATION_ZX == orientation)
        {
            for(i = 0; i < div_x+1; i++)
                for(j = 0; j < div_y+1; j++)
                {
                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = 0;
                    mesh->v_coords[vi].z = -hsy + size_y/div_y * j;
                    vi++;
                }
        }
        else if(MOTO_ORIENTATION_XY == orientation)
        {
            for(i = 0; i < div_x+1; i++)
                for(j = 0; j < div_y+1; j++)
                {
                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = -hsy + size_y/div_y * j;
                    mesh->v_coords[vi].z = 0;
                    vi++;
                }
        }

        if(new_mesh)
        {
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    guint16 v0 = get_v(i, j),
                            v1 = get_v(i+1, j),
                            v2 = get_v(i+1, j+1),
                            v3 = get_v(i, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
        }
    }

    priv->bound_calculated = FALSE;
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

static void moto_plane_node_update(MotoNode *self)
{
    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && 1)//moto_param_has_dests(param))
        moto_plane_node_update_mesh((MotoPlaneNode *)self);

    /* TODO: Implement NURBS objects =) */
    /*
    param = moto_node_get_param(self, "main", "nurbs");
    if(param && moto_param_has_dests(param))
        moto_plane_update_nurbs(plane);
    */
}

static void calc_bound(MotoPlaneNode *self)
{
    MotoPlaneNodePriv *priv = MOTO_PLANE_NODE_GET_PRIVATE(self);

    gfloat size_x = *(priv->size_x_ptr);
    gfloat size_y = *(priv->size_y_ptr);
    MotoOrientation orientation = *(priv->orientation_ptr);

    gfloat hsx, hsy, hsz;
    switch(orientation)
    {
        case MOTO_ORIENTATION_XY:
            hsx = size_x / 2;
            hsy = size_y / 2;
            hsz = 0.05;
        break;
        case MOTO_ORIENTATION_YZ:
            hsx = 0.05;
            hsy = size_x / 2;
            hsz = size_y / 2;
        break;
        case MOTO_ORIENTATION_ZX:
            hsx = size_x / 2;
            hsy = 0.05;
            hsz = size_y / 2;
        break;
    }

    priv->bound->bound[0] = -hsx;
    priv->bound->bound[1] =  hsx;
    priv->bound->bound[2] = -hsy;
    priv->bound->bound[3] =  hsy;
    priv->bound->bound[4] = -hsz;
    priv->bound->bound[5] =  hsz;
}

static MotoBound *moto_plane_node_get_bound(MotoGeometryNode *self)
{
    MotoPlaneNodePriv *priv = MOTO_PLANE_NODE_GET_PRIVATE(self);

    if( ! priv->bound_calculated)
    {
        calc_bound((MotoPlaneNode *)self);
        priv->bound_calculated = TRUE;
    }

    return priv->bound;
}

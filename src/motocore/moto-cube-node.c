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

    self->priv = g_slice_new(MotoCubeNodePriv);

    self->priv->mesh = NULL;

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "size_x", "Size X", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 8.0f, pspec, "Size", "Size",
            "size_y", "Size Y", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 8.0f, pspec, "Size", "Size",
            "size_z", "Size Z", G_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 8.0f, pspec, "Size", "Size",
            "div_x", "Divisions by X",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 50u, pspec, "Divisions", "Divisions",
            "div_y", "Divisions by Y",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 50u, pspec, "Divisions", "Divisions",
            "div_z", "Divisions by Z",  G_TYPE_UINT, MOTO_PARAM_MODE_INOUT, 50u, pspec, "Divisions", "Divisions",
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

#define e_x_num (div_x*(div_y+1)*2 + div_x*2*(div_z-1))
#define e_y_num (div_y*(div_z+1)*2 + div_y*2*(div_x-1))
#define e_z_num (div_z*(div_x+1)*2 + div_z*2*(div_y-1))

#define e_x(x, y, z) \
    ((0 == (z)) ? (x) + div_x*(y) : ((div_z) == (z)) ? e_x_num - div_x*(div_y+1) + (x) + div_x*(y) : div_x*(div_y-1) + div_x*2*(z) + div_x*((y)?1:0) + (x))
#define e_y_(x, y, z) \
    ((0 == (x)) ? (y) + div_y*(z) : ((div_x) == (x)) ? e_y_num - div_y*(div_z+1) + (y) + div_y*(z) : div_y*(div_z-1) + div_y*2*(x) + div_y*((z)?1:0) + (y))
#define e_z_(x, y, z) \
    ((0 == (y)) ? (z) + div_z*(x) : ((div_y) == (y)) ? e_z_num - div_z*(div_x+1) + (z) + div_z*(x) : div_z*(div_x-1) + div_z*2*(y) + div_z*((x)?1:0) + (z))
#define e_y(x, y, z) (e_x_num + e_y_(x, y, z))
#define e_z(x, y, z) (e_x_num + e_y_num + e_z_(x, y, z))

#define get_v(x, y, z) \
    ((0 == (x)) ? (y)*(div_z+1) + (z) : ((div_x) == (x)) ? v_num - (div_y+1)*(div_z+1) + (y)*(div_z+1) + (z) : \
        (div_y-1)*(div_z-1) + (div_y+div_z)*2*(x) + ((0 == (y)) ? (z) : ((div_y) == (y)) ? (div_z-1) + 2*(y) + (z) : \
            (div_z-1) + 2*(y) + ((z)?1:0) ))

static guint32 get_he(MotoMeshSelection *sel, guint32 ei)
{
    if( ! moto_mesh_selection_is_edge_selected(sel, ei))
    {
        // moto_mesh_selection_select_edge(sel, ei);
        return ei*2;
    }
    return ei*2 + 1;
}

static guint32 get_he_pair(MotoMeshSelection *sel, guint32 ei)
{
    if( ! moto_mesh_selection_is_edge_selected(sel, ei))
    {
        moto_mesh_selection_select_edge(sel, ei);
        return ei*2+1;
    }
    return ei*2;
}

static void moto_cube_node_update_mesh(MotoCubeNode *self)
{
    /* TODO: Temporary solution! */

    gfloat size_x = *(self->priv->size_x_ptr);
    gfloat size_y = *(self->priv->size_y_ptr);
    gfloat size_z = *(self->priv->size_z_ptr);
    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;

    guint div_x = *(self->priv->div_x_ptr);
    guint div_y = *(self->priv->div_y_ptr);
    guint div_z = *(self->priv->div_z_ptr);

    guint v_num = (div_x + div_y)*2 * (div_z + 1) + (((div_x+1)*(div_y+1) - (div_x + div_y)*2) * 2);
    guint e_num = (div_x + div_y)*2 * (div_z + 1) + (div_x + div_y)*2*div_z + div_x*(div_y-1)*2 + div_y*(div_x-1)*2;
    guint f_num = div_x*div_y*2 + div_x*div_z*2 + div_y*div_z*2;
    g_print("Cube: v_num, e_num, f_num: %d, %d, %d\n", v_num, e_num, f_num);

    if(self->priv->mesh)
    {
        if(v_num != self->priv->mesh->v_num || e_num != self->priv->mesh->e_num || f_num != self->priv->mesh->f_num)
        {
            g_object_unref(self->priv->mesh);
            self->priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_num*4);
        }
    }
    else
    {
        self->priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_num*4);
    }

    MotoMesh *mesh = self->priv->mesh;
    MotoMeshSelection *sel = moto_mesh_selection_for_mesh(mesh);

    MotoParam *pm = moto_node_get_param((MotoNode *)self, "mesh");
    g_value_set_object(moto_param_get_value(pm), mesh);
    moto_param_update_dests(pm);

    if(mesh->b32)
    {
        MotoMeshVert32 *v_data  = (MotoMeshVert32 *)mesh->v_data;
        MotoMeshEdge32 *e_data  = (MotoMeshEdge32 *)mesh->e_data;
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)mesh->f_data;
        MotoHalfEdge32 *he_data = (MotoHalfEdge32 *)mesh->he_data;
        guint32 *e_verts = (guint32 *)mesh->e_verts;
        guint32 *f_verts = (guint32 *)mesh->f_verts;

        guint32 i, j, k, n = 4;
        for(i = 0; i < f_num; i++)
        {
            f_data[i].v_num = n;
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
        fi = 0;
        // x-axis faces
        for(i = 0; i < div_y; i++)
            for(j = 0; j < div_z; j++)
            {
                f_verts[fi*4]   = get_v(0, i, j);
                f_verts[fi*4+1] = get_v(0, i, j+1);
                f_verts[fi*4+2] = get_v(0, i+1, j+1);
                f_verts[fi*4+3] = get_v(0, i+1, j);

                guint32 e0 = e_z(0, i, j),
                        e1 = e_y(0, i, j+1),
                        e2 = e_z(0, i+1, j),
                        e3 = e_y(0, i, j);

                e_verts[e0*2]   = f_verts[fi*4];
                e_verts[e0*2+1] = f_verts[fi*4+1];
                e_verts[e1*2]   = f_verts[fi*4+1];
                e_verts[e1*2+1] = f_verts[fi*4+2];
                e_verts[e2*2]   = f_verts[fi*4+2];
                e_verts[e2*2+1] = f_verts[fi*4+3];
                e_verts[e3*2]   = f_verts[fi*4+3];
                e_verts[e3*2+1] = f_verts[fi*4];

                guint32 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

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

                guint32 e0 = e_y(div_x, i, j),
                        e1 = e_z(div_x, i+1, j),
                        e2 = e_y(div_x, i, j+1),
                        e3 = e_z(div_x, i, j);

                e_verts[e0*2]   = v0;
                e_verts[e0*2+1] = v1;
                e_verts[e1*2]   = v1;
                e_verts[e1*2+1] = v2;
                e_verts[e2*2]   = v2;
                e_verts[e2*2+1] = v3;
                e_verts[e3*2]   = v3;
                e_verts[e3*2+1] = v0;

                guint32 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

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

                guint32 e0 = e_x(i, 0, j),
                        e1 = e_z(i+1, 0, j),
                        e2 = e_x(i, 0, j+1),
                        e3 = e_z(i, 0, j);

                e_verts[e0*2]   = v0;
                e_verts[e0*2+1] = v1;
                e_verts[e1*2]   = v1;
                e_verts[e1*2+1] = v2;
                e_verts[e2*2]   = v2;
                e_verts[e2*2+1] = v3;
                e_verts[e3*2]   = v3;
                e_verts[e3*2+1] = v0;

                guint32 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[v0].half_edge = he0;
                v_data[v1].half_edge = he1;
                v_data[v2].half_edge = he2;
                v_data[v3].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

                fi++;
            }
        for(i = 0; i < div_x; i++)
            for(j = 0; j < div_z; j++)
            {
                f_verts[fi*4]   = get_v(i, div_y, j);
                f_verts[fi*4+1] = get_v(i, div_y, j+1);
                f_verts[fi*4+2] = get_v(i+1, div_y, j+1);
                f_verts[fi*4+3] = get_v(i+1, div_y, j);

                guint32 e0 = e_z(i, div_y, j),
                        e1 = e_x(i, div_y, j+1),
                        e2 = e_z(i+1, div_y, j),
                        e3 = e_x(i, div_y, j);

                e_verts[e0*2]   = f_verts[fi*4];
                e_verts[e0*2+1] = f_verts[fi*4+1];
                e_verts[e1*2]   = f_verts[fi*4+1];
                e_verts[e1*2+1] = f_verts[fi*4+2];
                e_verts[e2*2]   = f_verts[fi*4+2];
                e_verts[e2*2+1] = f_verts[fi*4+3];
                e_verts[e3*2]   = f_verts[fi*4+3];
                e_verts[e3*2+1] = f_verts[fi*4];

                guint32 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

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

                guint32 e0 = e_y(i, j, 0),
                        e1 = e_x(i, j+1, 0),
                        e2 = e_y(i+1, j, 0),
                        e3 = e_x(i, j, 0);

                e_verts[e0*2]   = f_verts[fi*4];
                e_verts[e0*2+1] = f_verts[fi*4+1];
                e_verts[e1*2]   = f_verts[fi*4+1];
                e_verts[e1*2+1] = f_verts[fi*4+2];
                e_verts[e2*2]   = f_verts[fi*4+2];
                e_verts[e2*2+1] = f_verts[fi*4+3];
                e_verts[e3*2]   = f_verts[fi*4+3];
                e_verts[e3*2+1] = f_verts[fi*4];

                guint32 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

                fi++;
            }
        for(i = 0; i < div_x; i++)
            for(j = 0; j < div_y; j++)
            {
                f_verts[fi*4]   = get_v(i, j, div_z);
                f_verts[fi*4+1] = get_v(i+1, j, div_z);
                f_verts[fi*4+2] = get_v(i+1, j+1, div_z);
                f_verts[fi*4+3] = get_v(i, j+1, div_z);

                guint32 e0 = e_x(i, j, div_z),
                        e1 = e_y(i+1, j, div_z),
                        e2 = e_x(i, j+1, div_z),
                        e3 = e_y(i, j, div_z);

                e_verts[e0*2]   = f_verts[fi*4];
                e_verts[e0*2+1] = f_verts[fi*4+1];
                e_verts[e1*2]   = f_verts[fi*4+1];
                e_verts[e1*2+1] = f_verts[fi*4+2];
                e_verts[e2*2]   = f_verts[fi*4+2];
                e_verts[e2*2+1] = f_verts[fi*4+3];
                e_verts[e3*2]   = f_verts[fi*4+3];
                e_verts[e3*2+1] = f_verts[fi*4];

                guint32 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

                fi++;
            }
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
        fi = 0;
        // x-axis faces
        for(i = 0; i < div_y; i++)
            for(j = 0; j < div_z; j++)
            {
                f_verts[fi*4]   = get_v(0, i, j);
                f_verts[fi*4+1] = get_v(0, i, j+1);
                f_verts[fi*4+2] = get_v(0, i+1, j+1);
                f_verts[fi*4+3] = get_v(0, i+1, j);

                guint16 e0 = e_z(0, i, j),
                        e1 = e_y(0, i, j+1),
                        e2 = e_z(0, i+1, j),
                        e3 = e_y(0, i, j);

                e_verts[e0*2]   = f_verts[fi*4];
                e_verts[e0*2+1] = f_verts[fi*4+1];
                e_verts[e1*2]   = f_verts[fi*4+1];
                e_verts[e1*2+1] = f_verts[fi*4+2];
                e_verts[e2*2]   = f_verts[fi*4+2];
                e_verts[e2*2+1] = f_verts[fi*4+3];
                e_verts[e3*2]   = f_verts[fi*4+3];
                e_verts[e3*2+1] = f_verts[fi*4];

                guint16 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

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

                guint16 e0 = e_y(div_x, i, j),
                        e1 = e_z(div_x, i+1, j),
                        e2 = e_y(div_x, i, j+1),
                        e3 = e_z(div_x, i, j);

                e_verts[e0*2]   = v0;
                e_verts[e0*2+1] = v1;
                e_verts[e1*2]   = v1;
                e_verts[e1*2+1] = v2;
                e_verts[e2*2]   = v2;
                e_verts[e2*2+1] = v3;
                e_verts[e3*2]   = v3;
                e_verts[e3*2+1] = v0;

                guint16 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

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

                guint16 e0 = e_x(i, 0, j),
                        e1 = e_z(i+1, 0, j),
                        e2 = e_x(i, 0, j+1),
                        e3 = e_z(i, 0, j);

                e_verts[e0*2]   = v0;
                e_verts[e0*2+1] = v1;
                e_verts[e1*2]   = v1;
                e_verts[e1*2+1] = v2;
                e_verts[e2*2]   = v2;
                e_verts[e2*2+1] = v3;
                e_verts[e3*2]   = v3;
                e_verts[e3*2+1] = v0;

                guint16 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[v0].half_edge = he0;
                v_data[v1].half_edge = he1;
                v_data[v2].half_edge = he2;
                v_data[v3].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

                fi++;
            }
        for(i = 0; i < div_x; i++)
            for(j = 0; j < div_z; j++)
            {
                f_verts[fi*4]   = get_v(i, div_y, j);
                f_verts[fi*4+1] = get_v(i, div_y, j+1);
                f_verts[fi*4+2] = get_v(i+1, div_y, j+1);
                f_verts[fi*4+3] = get_v(i+1, div_y, j);

                guint16 e0 = e_z(i, div_y, j),
                        e1 = e_x(i, div_y, j+1),
                        e2 = e_z(i+1, div_y, j),
                        e3 = e_x(i, div_y, j);

                e_verts[e0*2]   = f_verts[fi*4];
                e_verts[e0*2+1] = f_verts[fi*4+1];
                e_verts[e1*2]   = f_verts[fi*4+1];
                e_verts[e1*2+1] = f_verts[fi*4+2];
                e_verts[e2*2]   = f_verts[fi*4+2];
                e_verts[e2*2+1] = f_verts[fi*4+3];
                e_verts[e3*2]   = f_verts[fi*4+3];
                e_verts[e3*2+1] = f_verts[fi*4];

                guint16 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

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

                guint16 e0 = e_y(i, j, 0),
                        e1 = e_x(i, j+1, 0),
                        e2 = e_y(i+1, j, 0),
                        e3 = e_x(i, j, 0);

                e_verts[e0*2]   = f_verts[fi*4];
                e_verts[e0*2+1] = f_verts[fi*4+1];
                e_verts[e1*2]   = f_verts[fi*4+1];
                e_verts[e1*2+1] = f_verts[fi*4+2];
                e_verts[e2*2]   = f_verts[fi*4+2];
                e_verts[e2*2+1] = f_verts[fi*4+3];
                e_verts[e3*2]   = f_verts[fi*4+3];
                e_verts[e3*2+1] = f_verts[fi*4];

                guint16 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

                fi++;
            }
        for(i = 0; i < div_x; i++)
            for(j = 0; j < div_y; j++)
            {
                f_verts[fi*4]   = get_v(i, j, div_z);
                f_verts[fi*4+1] = get_v(i+1, j, div_z);
                f_verts[fi*4+2] = get_v(i+1, j+1, div_z);
                f_verts[fi*4+3] = get_v(i, j+1, div_z);

                guint16 e0 = e_x(i, j, div_z),
                        e1 = e_y(i+1, j, div_z),
                        e2 = e_x(i, j+1, div_z),
                        e3 = e_y(i, j, div_z);

                e_verts[e0*2]   = f_verts[fi*4];
                e_verts[e0*2+1] = f_verts[fi*4+1];
                e_verts[e1*2]   = f_verts[fi*4+1];
                e_verts[e1*2+1] = f_verts[fi*4+2];
                e_verts[e2*2]   = f_verts[fi*4+2];
                e_verts[e2*2+1] = f_verts[fi*4+3];
                e_verts[e3*2]   = f_verts[fi*4+3];
                e_verts[e3*2+1] = f_verts[fi*4];

                guint16 he0 = get_he(sel, e0),
                        he1 = get_he(sel, e1),
                        he2 = get_he(sel, e2),
                        he3 = get_he(sel, e3);

                f_data[fi].half_edge = he0;
                e_data[e0].half_edge = he0;
                e_data[e1].half_edge = he1;
                e_data[e2].half_edge = he2;
                e_data[e3].half_edge = he3;
                v_data[f_verts[fi*4]].half_edge   = he0;
                v_data[f_verts[fi*4+1]].half_edge = he1;
                v_data[f_verts[fi*4+2]].half_edge = he2;
                v_data[f_verts[fi*4+3]].half_edge = he3;

                he_data[he0].pair = get_he_pair(sel, e0);
                he_data[he1].pair = get_he_pair(sel, e1);
                he_data[he2].pair = get_he_pair(sel, e2);
                he_data[he3].pair = get_he_pair(sel, e3);
                he_data[he0].next = he1;
                he_data[he1].next = he2;
                he_data[he2].next = he3;
                he_data[he3].next = he0;
                he_data[he0].v_origin = f_verts[fi*4];
                he_data[he1].v_origin = f_verts[fi*4+1];
                he_data[he2].v_origin = f_verts[fi*4+2];
                he_data[he3].v_origin = f_verts[fi*4+3];
                he_data[he0].edge = e0;
                he_data[he1].edge = e1;
                he_data[he2].edge = e2;
                he_data[he3].edge = e3;
                he_data[he0].f_left = fi;
                he_data[he1].f_left = fi;
                he_data[he2].f_left = fi;
                he_data[he3].f_left = fi;

                fi++;
            }
    }

    moto_mesh_selection_free(sel);

    moto_mesh_prepare(mesh);
}
#undef e_x_num
#undef e_y_num
#undef e_z_num
#undef e_x
#undef e_y
#undef e_z
#undef get_v

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

